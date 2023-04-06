#include "gm_file.hpp"

#include "gm_logger.hpp"
#include "../../system/gm_system.hpp"
#include "../../headers/string.hpp"

#include <algorithm>
#include <bit>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <memory>
#include <mutex>
#include <vector>

#if defined(_WIN32)
  #include <windows.h>
#elif defined(__linux__)
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
    UTF8Str File::_executableDir = UTF8Str{sizeof("NULL") - 1, std::shared_ptr<const char>("NULL", [](const char*){})};;
    std::mutex File::_fileMtx;

    void File::init() {
        findExecutableDir();
        // TODO Get title and version for Core
    }

    void const File::ensureParentDir(const UTF8Str& path) {
        if (!fs::exists(path.get())) {
            fs::path p = path.get();
            fs::create_directories(p.parent_path());
        }
    }

    File::FileContents const File::readFile(const char* filepath) {
        if (!fs::exists(filepath)) {
            UTF8Str msg = FormatString::formatString("File not found: %s", filepath);
            Logger::crash(msg);
        }
        
        // Allocate
        uint8_t buf[BUFSIZ];
        size_t capacity = sizeof(buf);
        uint8_t* data = static_cast<uint8_t*>(std::malloc(capacity));

        // Open file
        _fileMtx.lock();
        FILE* f = std::fopen(filepath, "rb");
        if (!f) {
            UTF8Str msg = FormatString::formatString("Could not open file: %s", filepath);
            Logger::crash(msg);
        }

        // Read
        size_t head = 0, c = 0;
        while ((c = std::fread(buf, 1, sizeof(buf), f)) > 0) {
            head += c;

            // Resize out buffer if needed
            if (capacity < head) {
                capacity <<= 1;
                data = static_cast<uint8_t*>(std::realloc(data, capacity));
            }

            std::memcpy(data + (head - c), buf, c);
        }

        // Close file
        std::fclose(f);
        _fileMtx.unlock();
        data = static_cast<uint8_t*>(std::realloc(data, head));

        return FileContents{head, std::shared_ptr<const uint8_t>(data, std::free)};
    }

    void const File::writeFile(const char* filepath, const FileContents& contents, const bool append) {
        if (append && !fs::exists(filepath)) {
            UTF8Str msg = FormatString::formatString("File not found: %s", filepath);
            Logger::crash(msg);
        }


        // Open file
        _fileMtx.lock();
        FILE* f = std::fopen(filepath, append ? "ab" : "wb");
        if (!f) {
            UTF8Str msg = FormatString::formatString("Could not open file: %s", filepath);
            Logger::crash(msg);
        }

        // Write
        const size_t len = contents.length();
        const uint8_t* data = contents.get();
        size_t c = 0;
        for (size_t head = 0; head < len; head += c) {
            c = std::min(static_cast<size_t>(BUFSIZ), len - head);
            std::fwrite(data + head, 1, c, f);
        }

        // Close file
        std::fclose(f);
        _fileMtx.unlock();
    }

    void File::findExecutableDir()
    {
        unsigned int bufferSize = 1024;
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
        UTF8Str link = FormatString::formatString("/proc/%d/exe", pid);

        // Read the contents of the link.
        int count = readlink(link.get(), &buffer[0], bufferSize);
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
        char* pathStr = static_cast<char*>(std::malloc(path.length() + 1));
        std::memcpy(pathStr, path.c_str(), path.length());
        pathStr[path.length()] = '\0';
        File::_executableDir = UTF8Str{static_cast<int64_t>(path.length()), std::shared_ptr<const char>(pathStr, std::free)};
    }
}
