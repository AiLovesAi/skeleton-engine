#include "gm_file.hpp"

#include "../gm_core.hpp"
#include "../logger/gm_logger.hpp"

#include <lzma.h>

#include <bit>
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

extern "C" {
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    inline size_t max(size_t a, const size_t b) {
        a -= b;
        a &= (~a) >> 63;
        a += b;
        return a;
    }
    inline size_t min(size_t a, const size_t b) {
        a -= b;
        a &= a >> 63;
        a += b;
        return a;
    }
    
    char* readFileC(const char* file) {
        char buf[1024];
        size_t capacity = sizeof(buf);
        char* out = (char*) malloc(capacity);

        FILE* f = fopen(file, "rb");
        if (!f) return NULL;

        size_t len = 0, c = 0;
        while ((c = fread(buf, sizeof(char), sizeof(buf), f)) > 0) {
            len += c;

            // Resize out buffer if needed
            // Note: Subtract 1 from capacity to allow room for null terminator
            if (capacity - 1 < len) {
                capacity = max(capacity * 2, len);
                out = (char*) realloc(out, capacity);
            }

            strncpy(out + (len - c), buf, c);
        }

        out[len] = '\0';
        out = (char*) realloc(out, len + 1);

        fclose(f);

        return out;
    }

    void writeFileC(const char* file, const char* data, const uint8_t append) {
        FILE* f = fopen(file, append ? "ab" : "wb");
        if (!f) return;

        size_t len = strlen(data);
        size_t c = 0;
        while (len > 0) {
            c = min(1024, len);
            len -= fwrite(data, 1, c, f);
        }

        fclose(f);
    }
}

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
        std::stringstream data;

        if (!fs::exists(file)) {
            data << "File not found: " << file;
            Logger::crash(data.str());
        }

        mtx.lock();

        char* input = readFileC(file.c_str());
        data << input;
        std::free(input);

        mtx.unlock();

        return data.str();
    }

    std::string const File::decompressFile(const std::string& file) {
        lzma_stream stream = LZMA_STREAM_INIT;
        
        // TODO

	    lzma_end(&stream);

        return "";
    }

    void const File::writeFile(const std::string& file, const std::string& data, const bool append) {
        if (append && !fs::exists(file)) {
            std::stringstream msg;
            msg << "File not found: " << file;
            Logger::crash(msg.str());
        }

        mtx.lock();

        writeFileC(file.c_str(), data.c_str(), append);

        mtx.unlock();
    }

    void const File::compressFile(const std::string& file, const std::string& data, const bool append) {
        lzma_stream stream = LZMA_STREAM_INIT;

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
