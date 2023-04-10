#include "gm_compression.hpp"

#include "gm_logger.hpp"
#include "../../gm_core.hpp"
#include "../../headers/string.hpp"
#include "../../system/gm_system.hpp"

#include <lzma.h>

#include <memory>
#include <mutex>

namespace game {
    inline void initDecoder(lzma_stream *stream, const char*__restrict__ filepath) {
        // The .xz format allows concatenating compressed files as is:
        //
        //     echo foo | xz > foobar.xz
        //     echo bar | xz >> foobar.xz
        //
        // When decompressing normal standalone .xz files, LZMA_CONCATENATED
        // should always be used to support decompression of concatenated
        // .xz files. If LZMA_CONCATENATED isn't used, the decoder will stop
        // after the first .xz stream. This can be useful when .xz data has
        // been embedded inside another file format.
        //
        // Flags other than LZMA_CONCATENATED are supported too, and can
        // be combined with bitwise-or. See lzma/container.h
        // (src/liblzma/api/lzma/container.h in the source package or e.g.
        // /usr/include/lzma/container.h depending on the install prefix)
        // for details.
        lzma_ret ret = lzma_stream_decoder(stream, UINT64_MAX, LZMA_CONCATENATED);

        if (ret != LZMA_OK) {
            switch (ret) {
                case LZMA_MEM_ERROR: {
                    Logger::crash(FormatString::formatString("Ran out of memory while decompressing file: %s", filepath));
                }

                case LZMA_OPTIONS_ERROR: {
                    Logger::crash(FormatString::formatString("Unsupported decompressor flags for file: %s", filepath));
                }

                default: {
                    Logger::crash(FormatString::formatString("Unknown error occurred while decompressing file: %s", filepath));
                }
            }
        }
    }

    File::FileContents const Compression::decompressFile(const char*__restrict__ filepath) {
        lzma_stream stream = LZMA_STREAM_INIT;
        initDecoder(&stream, filepath);
	    lzma_action action = LZMA_RUN;

        // Open file
        File::_fileMtx.lock();
        FILE* f = std::fopen(filepath, "rb");
        if (!f) Logger::crash(FormatString::formatString("Error opening file: %s", filepath));
        
        size_t head = 0, c = 0;
        uint8_t inbuf[BUFSIZ], outbuf[BUFSIZ];
        size_t capacity = sizeof(inbuf);
        uint8_t* data = static_cast<uint8_t*>(std::malloc(capacity));

        stream.next_in = nullptr;
        stream.avail_in = 0;
        stream.next_out = outbuf;
        stream.avail_out = sizeof(outbuf);

        while (true) {
            // Fill input buffer
            if (stream.avail_in == 0 && !std::feof(f)) {
                stream.next_in = inbuf;
                stream.avail_in = std::fread(inbuf, 1, sizeof(inbuf), f);

                if (std::feof(f)) action = LZMA_FINISH;
            }

            // Decompress
            lzma_ret ret = lzma_code(&stream, action);

            // Fill output data
            if (stream.avail_out == 0 || ret == LZMA_STREAM_END) {
                c = sizeof(outbuf) - stream.avail_out;
                head += c;
                
                // Check if data can fit buffer
                if (capacity < head) {
                    capacity = head * 2;
                    data = static_cast<uint8_t*>(std::realloc(data, capacity));
                }
                std::memcpy(data + (head - c), outbuf, c);
                
                stream.next_out = outbuf;
                stream.avail_out = sizeof(outbuf);
            }

            if (ret != LZMA_OK) {
                if (ret == LZMA_STREAM_END) break;

                switch (ret) {
                    case LZMA_MEM_ERROR: {
                        Logger::crash(FormatString::formatString("Ran out of memory while decompressing file: %s", filepath));
                    }

                    case LZMA_DATA_ERROR: {
                        Logger::crash(FormatString::formatString("Corrupted data encountered while decompressing file: %s", filepath));
                    }

                    case LZMA_FORMAT_ERROR: {
                        Logger::crash(FormatString::formatString("Input file for decompression is not in xz format: %s", filepath));
                    }

                    case LZMA_OPTIONS_ERROR: {
                        Logger::crash(FormatString::formatString("Unsupported compression options for file: %s", filepath));
                    }

                    case LZMA_BUF_ERROR: {
                        Logger::crash(FormatString::formatString("Compressed file is truncated or otherwise corrupt: %s", filepath));
                    }

                    default: {
                        Logger::crash(FormatString::formatString("Unknown error occurred while decompressing file: %s", filepath));
                    }
                }
            }
	    }

        // Close file
        std::fclose(f);
	    lzma_end(&stream);
        File::_fileMtx.unlock();
	    data = static_cast<uint8_t*>(std::realloc(data, head));

        return File::FileContents{head, std::shared_ptr<const uint8_t>(data, std::free)};;
    }

    inline void initEncoder(lzma_stream* stream, const char*__restrict__ filepath) {
        lzma_mt mt = {
            .flags = 0,
            .threads = std::min(System::cpuThreadCount(), static_cast<uint32_t>(8)),
            .block_size = 0,
            .timeout = 0,
            .preset = File::COMPRESSION_PRESET,
            .filters = nullptr, // Filters must be null with a preset
            .check = LZMA_CHECK_CRC64,
        };

        lzma_ret ret = lzma_stream_encoder_mt(stream, &mt);

        if (ret != LZMA_OK) {
            switch (ret) {
                case LZMA_MEM_ERROR: {
                    Logger::crash(FormatString::formatString("Ran out of memory while decompressing file: %s", filepath));
                }

                case LZMA_OPTIONS_ERROR: {
                    Logger::crash(FormatString::formatString("Unsupported decompressor flags for file: %s", filepath));
                }
                
                case LZMA_UNSUPPORTED_CHECK: {
                    Logger::crash(FormatString::formatString("Unsupported integrity check for file: %s", filepath));
                }

                default: {
                    Logger::crash(FormatString::formatString("Unknown error occurred while decompressing file: %s", filepath));
                }
            }
        }
    }

    void const Compression::compressFile(const char*__restrict__ filepath, const File::FileContents& contents, const bool append) {
        lzma_stream stream = LZMA_STREAM_INIT;
        initEncoder(&stream, filepath);
	    lzma_action action = LZMA_RUN;

        // Open file
        File::_fileMtx.lock();
        FILE* f = fopen(filepath, append ? "ab" : "wb");
        if (!f) Logger::crash(FormatString::formatString("Error opening file: %s", filepath));

        const size_t len = contents.length();
        const uint8_t* data = contents.get();

        size_t head = 0, c = 0;
        uint8_t buf[BUFSIZ];
        
        stream.next_in = nullptr;
        stream.avail_in = 0;
        stream.next_out = buf;
        stream.avail_out = sizeof(buf);

        // Compress and write
        while (true) {
            // Fill the input buffer if it is empty.
            if (stream.avail_in == 0 && head < len) {
                c = std::min(sizeof(buf), len - head);
                stream.next_in = data + head;
                stream.avail_in = c;
                head += c;

                if (head >= len) action = LZMA_FINISH;
            }

            // Compress
		    lzma_ret ret = lzma_code(&stream, action);
            
            if (stream.avail_out == 0 || ret == LZMA_STREAM_END) {
                // When lzma_code() has returned LZMA_STREAM_END, the output buffer is likely to be only
                // partially full. Calculate how much new data there is to be written to the output file.
                c = sizeof(buf) - stream.avail_out;

                std::fwrite(buf, 1, c, f);

                // Reset next_out and avail_out.
                stream.next_out = buf;
                stream.avail_out = sizeof(buf);
            }

            if (ret != LZMA_OK) {
                // Once everything has been encoded successfully, the return value of
                // lzma_code() will be LZMA_STREAM_END.
                if (ret == LZMA_STREAM_END) break;

                switch (ret) {
                    case LZMA_MEM_ERROR: {
                        Logger::crash(FormatString::formatString("Ran out of memory while compressing file: %s", filepath));
                    }

                    case LZMA_DATA_ERROR: {
                        Logger::crash(FormatString::formatString("File size is greater than maximum (2^63 bytes): %s", filepath));
                    }

                    default: {
                        Logger::crash(FormatString::formatString("Unknown error occurred while compressing file: %s", filepath));
                    }
                }
            }
        }

        // Close file
        std::fclose(f);
	    lzma_end(&stream);
        File::_fileMtx.unlock();
    }
}
