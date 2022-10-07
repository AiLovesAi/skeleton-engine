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

    File::FileContents const File::readFile(const char* filepath) {
        if (!fs::exists(filepath)) {
            std::stringstream msg;
            msg << "File not found: " << filepath;
            Logger::crash(msg.str());
        }
        
        // Allocate
        uint8_t buf[BUFSIZ];
        size_t capacity = sizeof(buf);
        uint8_t* data = static_cast<uint8_t*>(std::malloc(capacity));

        // Open file
        mtx.lock();
        FILE* f = std::fopen(filepath, "rb");
        if (!f) {
            std::stringstream msg;
            msg << "Could not open file: " << filepath;
            Logger::crash(msg.str());
        }

        // Read
        size_t head = 0, c = 0;
        while ((c = std::fread(buf, 1, sizeof(buf), f)) > 0) {
            head += c;

            // Resize out buffer if needed
            if (capacity < head) {
                capacity *= 2;
                data = static_cast<uint8_t*>(std::realloc(data, capacity));
            }

            std::memcpy(data + (head - c), buf, c);
        }

        // Close file
        std::fclose(f);
        mtx.unlock();
        data = static_cast<uint8_t*>(std::realloc(data, head));

        FileContents contents;
        contents.len = head; 
        contents.data = std::shared_ptr<uint8_t>(data, std::free);
        return contents;
    }

    void const File::writeFile(const char* filepath, const FileContents& contents, const bool append) {
        if (append && !fs::exists(filepath)) {
            std::stringstream msg;
            msg << "File not found: " << filepath;
            Logger::crash(msg.str());
        }


        // Open file
        mtx.lock();
        FILE* f = std::fopen(filepath, append ? "ab" : "wb");
        if (!f) {
            std::stringstream msg;
            msg << "Could not open file: " << filepath;
            Logger::crash(msg.str());
        }

        // Write
        const size_t len = contents.len;
        const uint8_t* data = contents.data.get();
        size_t c = 0;
        for (size_t head = 0; head < len; head += c) {
            c = std::min(static_cast<size_t>(BUFSIZ), len - head);
            std::fwrite(data + head, 1, c, f);
        }

        // Close file
        std::fclose(f);
        mtx.unlock();
    }

    File::FileContents const File::decompressFile(const char* filepath) {
        lzma_stream stream = LZMA_STREAM_INIT;
	    lzma_action action = LZMA_RUN;

        // Open file
        mtx.lock();
        FILE* f = std::fopen(filepath, "rb");
        if (f == nullptr) {
            std::stringstream msg;
            msg << "Error opening file: " << filepath;
            Logger::crash(msg.str());
        }
        
        size_t head = 0, c = 0;
        uint8_t buf[BUFSIZ];
        size_t capacity = sizeof(buf);
        uint8_t* data = static_cast<uint8_t*>(std::malloc(capacity));

        stream.next_in = nullptr;
        stream.avail_in = 0;
        stream.next_out = buf;
        stream.avail_out = sizeof(buf);

        while (true) {
            // Fill input buffer
            if (stream.avail_in == 0 && !std::feof(f)) {
                stream.next_in = buf;
                stream.avail_in = std::fread(buf, 1, sizeof(buf), f);

                if (std::feof(f)) action = LZMA_FINISH;
            }

            // Decompress
            lzma_ret ret = lzma_code(&stream, action);

            // Fill output data
            if (stream.avail_out == 0 || ret == LZMA_STREAM_END) {
                c = sizeof(buf) - stream.avail_out;
                head += c;
                
                // Check if next cycle can store enough data
                if (capacity < head + sizeof(buf)) {
                    capacity *= 2;
                    data = static_cast<uint8_t*>(std::realloc(data, capacity));
                }
                
                stream.next_out = data + head;
                stream.avail_out = sizeof(buf);
            }

            if (ret != LZMA_OK) {
                if (ret == LZMA_STREAM_END) break;

                std::stringstream msg;
                switch (ret) {
                    case LZMA_MEM_ERROR:
                        msg << "Ran out of memory while decompressing file: " << filepath;
                        break;

                    case LZMA_DATA_ERROR:
                        msg << "Corrupted data encountered while decompressing file: " << filepath;
                        break;

                    case LZMA_FORMAT_ERROR:
                        msg << "Input file for decompression is not in xz format: " << filepath;
                        break;

                    case LZMA_OPTIONS_ERROR:
                        msg << "Unsupported compression options for file: " << filepath;
                        break;

                    case LZMA_BUF_ERROR:
                        msg << "Compressed file is truncated or otherwise corrupt: " << filepath;
                        break;

                    default:
                        msg << "Unknown error occurred while decompressing file: " << filepath;
                        break;
                }
                Logger::crash(msg.str());
            }
	    }

        // Close file
        std::fclose(f);
	    lzma_end(&stream);
        mtx.unlock();
	    data = static_cast<uint8_t*>(std::realloc(data, head));

        FileContents contents;
	    contents.len = head;
        contents.data = std::shared_ptr<uint8_t>(data, std::free);
        return contents;
    }

    void const File::compressFile(const char* filepath, const FileContents& contents, const bool append) {
        lzma_stream stream = LZMA_STREAM_INIT;
	    lzma_action action = LZMA_RUN;

        // Open file
        mtx.lock();
        FILE* f = fopen(filepath, append ? "ab" : "wb");
        if (f == nullptr) {
            std::stringstream msg;
            msg << "Error opening file: " << filepath;
            Logger::crash(msg.str());
        }

        const size_t len = contents.len;
        const uint8_t* data = contents.data.get();

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

                std::stringstream msg;
                switch (ret) {
                    case LZMA_MEM_ERROR:
                        msg << "Ran out of memory while compressing file: " << filepath;
                        break;

                    case LZMA_DATA_ERROR:
                        msg << "File size is greater than maximum (2^63 bytes): " << filepath;
                        break;

                    default:
                        msg << "Unknown error occurred while compressing file: " << filepath;
                        break;
                }
                Logger::crash(msg.str());
            }
        }

        // Close file
        std::fclose(f);
	    lzma_end(&stream);
        mtx.unlock();
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
