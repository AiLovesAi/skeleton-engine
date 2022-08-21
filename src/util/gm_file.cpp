#include "gm_file.hpp"

#include "gm_logger.hpp"

#include <filesystem>
#include <sstream>
#include <bit>
#include <cstring>
#include <vector>

#if defined(_WIN32)
  #include <windows.h>
#elif defined(__linux__)
  #include <sstream>
  #include <unistd.h>
  #include <sys/utsname.h>
  #include <iterator>
  #include <execinfo.h>
  #include <stdlib.h>
#elif defined(__APPLE__)
  #include <mach-o/dyld.h>
  #include <sys/utsname.h>
  #include <iterator>
  #include <execinfo.h>
  #include <stdlib.h>
#endif

namespace fs = std::filesystem;

namespace game {
    std::string File::OSStr = "NULL";
    std::string File::CPUStr = "NULL";
    std::string File::executableDir = "NULL";
    
    void File::init() {
        getOS();
        getCPU();
        getExecutableDir();
    }

    void File::getOS() {
#if defined(_WIN32)
        OSStr = "Windows";
        OSVERSIONINFOA versionInfo{};
        versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
        GetVersionExA(&versionInfo);

        switch (versionInfo.dwMajorVersion) {
            case 10:
                switch (versionInfo.dwMinorVersion) {
                    case 0:
                        OSStr += " 10";
                    break;
                    default:
                        OSStr += " v10 Other";
                    break;
                }
            break;
            case 6:
                switch (versionInfo.dwMinorVersion) {
                    case 3:
                        OSStr += " 8.1";
                    break;
                    case 2:
                        OSStr += " 8";
                    break;
                    case 1:
                        OSStr += " 7";
                    break;
                    case 0:
                        OSStr += " Vista";
                    break;
                    default:
                        OSStr += " v6 Other";
                    break;
                }
            break;
            case 5:
                OSStr += " XP";
            break;
            default:
                OSStr += " Other";
            break;
        }

        OSStr += versionInfo.szCSDVersion;
#else
        struct utsname unameData;
        uname(&unameData);
        OSStr += nameData.sysname;
        OSStr += " ";
        OSStr += nameData.release;
#endif
    }

    std::string const File::asAscii(std::string str) {
        std::stringstream res;

        char c = '\0';
        for (size_t i = 0; i < str.length(); i++) {
            c = str[i];
            if (c < ' ' || c > '~') break;
            res << c;
        }

        return res.str();
    }

    bool const File::isAscii(std::string str) {
        char c = '\0';
        for (size_t i = 0; i < str.length(); i++) {
            c = str[i];
            if (c < ' ' || c > '~') return false;
        }

        return true;
    }

    void File::getCPU() {
        std::string mModelName;

        for(unsigned int i=0x80000002; i<=0x80000004; ++i) {
            CPUID cpuID(i, 0);
            mModelName += asAscii(std::string((const char*)&cpuID.RAX(), 4));
            mModelName += asAscii(std::string((const char*)&cpuID.RBX(), 4));
            mModelName += asAscii(std::string((const char*)&cpuID.RCX(), 4));
            mModelName += asAscii(std::string((const char*)&cpuID.RDX(), 4));
        }

        CPUStr = mModelName;
    }

    void File::getExecutableDir()
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
        if(count == -1) McpLogger::crash("Could not read symbolic link");
        buffer[count] = '\0';

#endif

        std::string path = &buffer[0];
        fs::path p = path;
#ifdef __WIN32__
        path = p.parent_path().string() + "\\";
#else
        path = p.parent_path().string() + "/";
#endif
        executableDir = path;
    }

    std::string const File::getStack() {
#ifdef __WIN32__
        return "   EMPTY   "; // TODO
#else
        std::ostringstream buffer;
        void *stack[32];
        std::size_t depth = backtrace(stack, 32);
        if (!depth)
                buffer << "   EMPTY   ");
        else {
                char **symbols = backtrace_symbols(stack, depth);
                for (std::size_t i = 1; i < depth; i++) {
                std::string symbol = symbols[i];
                std::istringstream iss(symbol);
                std::vector<std::string> strs{std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};
                for (auto const &x : strs) buffer << " " << demangle(x);
                buffer);
                }
                free(symbols);
        }
        return buffer.str();
#endif
    }

    void const File::ensureParentDir(std::string path) {
        if (!fs::exists(path)) {
            fs::path p = path;
            fs::create_directories(p.parent_path());
        }
    }
    
}
