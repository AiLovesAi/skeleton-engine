#include "gm_file.hpp"

#include "../gm_core.hpp"
#include "../logger/gm_logger.hpp"

#include <lzma.h>

#include <algorithm>
#include <bit>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <memory>
#include <mutex>
#include <sstream>
#include <vector>

#if defined(_WIN32)
  #include <windows.h>
#elif defined(__linux__)
  #include <sstream>
  #include <unistd.h>
  #include <iterator>
  #include <execinfo.h>
  #include <stdlib.h>
#elif defined(__APPLE__)
  #include <mach-o/dyld.h>
  #include <iterator>
  #include <execinfo.h>
  #include <stdlib.h>
#endif

namespace fs = std::filesystem;

namespace game {
    std::string File::executableDir_ = Core::EMPTYSTR;
    std::mutex mtx;

    void File::init() {
        findExecutableDir();
        // TODO Get title and version for Core
    }

    void const File::ensureParentDir(const std::string& path) {
        if (!fs::exists(path)) {
            fs::path p = path;
            fs::create_directories(p.parent_path());
        }
    }

    std::string const File::readFile(const std::string& file) {
        if (!fs::exists(file)) {
            std::stringstream msg;
            msg << "File not found: " << file;
            Logger::crash(msg.str());
        }
        
        // Allocate
        char buf[BUFSIZ];
        size_t capacity = sizeof(buf);
        char* out = static_cast<char*>(std::malloc(capacity));

        // Open file
        mtx.lock();
        FILE* f = std::fopen(file.c_str(), "rb");
        if (!f) {
            std::stringstream msg;
            msg << "Could not open file: " << file;
            Logger::crash(msg.str());
        }

        // Read
        size_t len = 0, c = 0;
        while ((c = std::fread(buf, sizeof(char), sizeof(buf), f)) > 0) {
            len += c;

            // Resize out buffer if needed
            // Note: Subtract 1 from capacity to allow room for null terminator.
            if (capacity - 1 < len) {
                capacity = std::max(capacity * 2, len);
                out = static_cast<char*>(std::realloc(out, capacity));
            }

            std::memcpy(out + (len - c), buf, c);
        }

        out[len] = '\0';
        out = static_cast<char*>(std::realloc(out, len + 1));

        // Close file
        std::fclose(f);
        mtx.unlock();

        // Free
        std::string data(out);
        std::free(out);
        return data;
    }

    std::string const File::decompressFile(const std::string& file) {
        std::string data;
        lzma_stream stream = LZMA_STREAM_INIT;
	    /*lzma_action action = LZMA_RUN;

        // Open file
        mtx.lock();
        FILE* f = fopen(file.c_str(), "rb");
        if (f == nullptr) {
            std::stringstream msg;
            msg << "Error opening file: " << file;
        }
        
        uint8_t inbuf[BUFSIZ];
        uint8_t outbuf[BUFSIZ];

        stream.next_in = nullptr;
        stream.avail_in = 0;
        stream.next_out = outbuf;
        stream.avail_out = sizeof(outbuf);

        while (true) {
            if (stream.avail_in == 0 && !feof(infile)) {
                stream.next_in = inbuf;
                stream.avail_in = fread(inbuf, 1, sizeof(inbuf),
                        infile);

                if (ferror(infile)) {
                    fprintf(stderr, "%s: Read error: %s\n",
                            inname, strerror(errno));
                    return false;
                }

                // Once the end of the input file has been reached,
                // we need to tell lzma_code() that no more input
                // will be coming. As said before, this isn't required
                // if the LZMA_CONCATENATED flag isn't used when
                // initializing the decoder.
                if (feof(infile))
                    action = LZMA_FINISH;
            }

            lzma_ret ret = lzma_code(strm, action);

            if (stream.avail_out == 0 || ret == LZMA_STREAM_END) {
                size_t write_size = sizeof(outbuf) - stream.avail_out;

                if (fwrite(outbuf, 1, write_size, outfile)
                        != write_size) {
                    fprintf(stderr, "Write error: %s\n",
                            strerror(errno));
                    return false;
                }

                stream.next_out = outbuf;
                stream.avail_out = sizeof(outbuf);
            }

            if (ret != LZMA_OK) {
                if (ret == LZMA_STREAM_END) break;

                std::stringstream msg;
                switch (ret) {
                    case LZMA_MEM_ERROR:
                        msg << "Ran out of memory while decompressing file: " << file;
                        break;

                    case LZMA_DATA_ERROR:
                        msg << "Corrupted data encountered while decompressing file: " << file;
                        break;

                    case LZMA_FORMAT_ERROR:
                        msg << "Input file for decompression is not in xz format: " << file;
                        break;

                    case LZMA_OPTIONS_ERROR:
                        msg << "Unsupported compression options for file: " << file;
                        break;

                    case LZMA_BUF_ERROR:
                        msg << "Compressed file is truncated or otherwise corrupt: " << file;
                        break;

                    default:
                        msg << "Unknown error occurred while decompressing file: " << file;
                        break;
                }
                Logger::crash(msg.str());
            }
	    }

        // Close file
        fclose(f);
        mtx.unlock();*/
	    lzma_end(&stream);

        return data;
    }

    void const File::writeFile(const std::string& file, const std::string& data, const bool append) {
        if (append && !fs::exists(file)) {
            std::stringstream msg;
            msg << "File not found: " << file;
            Logger::crash(msg.str());
        }


        // Open file
        mtx.lock();
        FILE* f = std::fopen(file.c_str(), append ? "ab" : "wb");
        if (!f) {
            std::stringstream msg;
            msg << "Could not open file: " << file;
            Logger::crash(msg.str());
        }

        // Write
        // Note: Add one to length for null terminator.
        size_t len = data.length() + 1;
        size_t c = 0;
        const char* cstr = data.c_str();
        for (size_t head = 0; head < len; head += c) {
            c = std::min(static_cast<size_t>(BUFSIZ), len - head);
            std::fwrite(cstr + head, 1, c, f);
        }

        // Close file
        std::fclose(f);
        mtx.unlock();
    }

    void const File::compressFile(const std::string& file, const std::string& data, const bool append) {
        lzma_stream stream = LZMA_STREAM_INIT;
	    lzma_action action = LZMA_RUN;

        // Open file
        mtx.lock();
        FILE* f = fopen(file.c_str(), append ? "ab" : "wb");
        if (f == nullptr) {
            std::stringstream msg;
            msg << "Error opening file: " << file;
        }

        // TODO Use uint8_t* to store data - data can contain '\0', which would prematurely null terminate a string
        size_t len = data.length();
        size_t head = 0, c = 0;
        const char* cstr = data.c_str();
        uint8_t inbuf[BUFSIZ];
        uint8_t outbuf[BUFSIZ];
        
        stream.next_in = nullptr;
        stream.avail_in = 0;
        stream.next_out = outbuf;
        stream.avail_out = sizeof(outbuf);

        // Compress and write
        while (true) {
            // Fill the input buffer if it is empty.
            if (stream.avail_in == 0 && head < len) {
                c = std::min(sizeof(inbuf), len - head);
                stream.next_in = inbuf;
                stream.avail_in = c;
                std::memcpy(inbuf, cstr + head, c);
                head += c;

                if (head >= len) action = LZMA_FINISH;
            }

            // Compress
		    lzma_ret ret = lzma_code(&stream, action);
            
            if (stream.avail_out == 0 || ret == LZMA_STREAM_END) {
                // When lzma_code() has returned LZMA_STREAM_END, the output buffer is likely to be only
                // partially full. Calculate how much new data there is to be written to the output file.
                c = sizeof(outbuf) - stream.avail_out;

                fwrite(outbuf, 1, c, f);

                // Reset next_out and avail_out.
                stream.next_out = outbuf;
                stream.avail_out = sizeof(outbuf);
            }

            if (ret != LZMA_OK) {
                // Once everything has been encoded successfully, the return value of
                // lzma_code() will be LZMA_STREAM_END.
                if (ret == LZMA_STREAM_END) break;

                std::stringstream msg;
                switch (ret) {
                    case LZMA_MEM_ERROR:
                        msg << "Ran out of memory while compressing file: " << file;
                        break;

                    case LZMA_DATA_ERROR:
                        msg << "File size is greater than maximum (2^63 bytes): " << file;
                        break;

                    default:
                        msg << "Unknown error occurred while compressing file: " << file;
                        break;
                }
                Logger::crash(msg.str());
            }
        }

        // Close file
        fclose(f);
        mtx.unlock();
	    lzma_end(&stream);
    }

    std::string const File::asAscii(const std::string& str) {
        std::stringstream res;

        char c = '\0';
        for (size_t i = 0; i < str.length(); i++) {
            c = str[i];
            if (c < ' ' || c > '~') break;
            res << c;
        }

        return res.str();
    }

    bool const File::isAscii(const std::string& str) {
        char c = '\0';
        for (size_t i = 0; i < str.length(); i++) {
            c = str[i];
            if (c < ' ' || c > '~') return false;
        }

        return true;
    }

    void File::findExecutableDir()
    {
        unsigned int bufferSize = 512;
        std::vector<char> buffer(bufferSize + 1);

#if defined(_WIN32)
        ::GetModuleFileNameA(NULL, &buffer[0], bufferSize);

#elif defined(__APPLE__)
        if(_NSGetExecutablePath(&buffer[0], &bufferSize))
        {
            buffer.resize(bufferSize);
            _NSGetExecutablePath(&buffer[0], &bufferSize);
        }

#else
        // Get the process ID.
        int pid = getpid();

        // Construct a path to the symbolic link pointing to the process executable.
        // This is at /proc/<pid>/exe on Linux systems (we hope).
        std::ostringstream oss;
        oss << "/proc/" << pid << "/exe";
        std::string link = oss.str();

        // Read the contents of the link.
        int count = readlink(link.c_str(), &buffer[0], bufferSize);
        if(count == -1) Logger::crash("Could not read symbolic link");
        buffer[count] = '\0';

#endif

        std::string path = &buffer[0];
        fs::path p = path;
#ifdef __WIN32__
        path = p.parent_path().string() + "\\";
#else
        path = p.parent_path().string() + "/";
#endif
        File::executableDir_ = path;
    }
}
