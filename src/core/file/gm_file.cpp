#include "gm_file.hpp"

#include "../gm_core.hpp"
#include "../logger/gm_logger.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <bit>
#include <cstring>
#include <vector>
#include <mutex>

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
        std::stringstream data;

        if (!fs::exists(file)) {
            data << "File not found: " << file;
            Logger::crash(data.str());
        }

        mtx.lock();
        std::ifstream f;
        f.open(file, std::ios::in | std::ios::binary);

        std::string line;
        while (std::getline(f, line)) {
            data << line;
        }

        f.close();
        mtx.unlock();

        return data.str();
    }

    void const File::writeFile(const std::string& file, const std::string& data, const bool append) {
        if (append && !fs::exists(file)) {
            std::stringstream msg;
            msg << "File not found: " << file;
            Logger::crash(msg.str());
        }

        mtx.lock();
        std::ofstream f;
        f.open(file, std::ios::out | std::ios::binary | (append ? std::ios::app : std::ios::trunc));
        f << data;
        f.close();
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
