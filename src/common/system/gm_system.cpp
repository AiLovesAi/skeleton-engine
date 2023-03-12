#include "gm_system.hpp"

#include "../gm_core.hpp"
#include "../data/file/gm_logger.hpp"
#include "../data/string/gm_string.hpp"

#include <lzma.h>

#if defined(_WIN32)
  #include <windows.h>
#elif defined(__linux__)
  #include <sstream>
  #include <unistd.h>
  #include <sys/utsname.h>
  #include <iterator>
  #include <stdlib.h>
#elif defined(__APPLE__)
  #include <mach-o/dyld.h>
  #include <sys/utsname.h>
  #include <iterator>
  #include <stdlib.h>
#endif

namespace game {
    uint32_t System::CPU_THREAD_COUNT_;
    size_t System::PHYSICAL_MEMORY_;
    UTF8Str System::OS_ = Core::EMPTYSTR;
    UTF8Str System::CPU_ = Core::EMPTYSTR;
    UTF8Str System::GPU_ = Core::EMPTYSTR;

    void System::init() {
        findOS();
        findCPU();
        CPU_THREAD_COUNT_ = lzma_cputhreads();
        PHYSICAL_MEMORY_ = lzma_physmem();
        if (CPU_THREAD_COUNT_ < 1) {
            CPU_THREAD_COUNT_ = 1;
            Logger::log(LOG_WARN, "Could not get CPU thread count.");
        }
    }

    void System::findOS() {
        char* OS = static_cast<char*>(std::malloc(BUFSIZ));
        int64_t len = 0;
#if defined(_WIN32)
        std::memcpy(OS, "Windows", sizeof("Windows") - 1);
        len += sizeof("Windows") - 1;

        OSVERSIONINFOA versionInfo{};
        versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
        GetVersionExA(&versionInfo);

        switch (versionInfo.dwMajorVersion) {
            case 10:
                switch (versionInfo.dwMinorVersion) {
                    case 0:
                        std::memcpy(OS + len, " 10", sizeof(" 10") - 1);
                        len += sizeof(" 10") - 1;
                    break;
                    default:
                        std::memcpy(OS + len, " v10 Other", sizeof(" v10 Other") - 1);
                        len += sizeof(" v10 Other") - 1;
                    break;
                }
            break;
            case 6:
                switch (versionInfo.dwMinorVersion) {
                    case 3:
                        std::memcpy(OS + len, " 8.1", sizeof(" 8.1") - 1);
                        len += sizeof(" 8.1") - 1;
                    break;
                    case 2:
                        std::memcpy(OS + len, " 8", sizeof(" 8") - 1);
                        len += sizeof(" 8") - 1;
                    break;
                    case 1:
                        std::memcpy(OS + len, " 7", sizeof(" 7") - 1);
                        len += sizeof(" 7") - 1;
                    break;
                    case 0:
                        std::memcpy(OS + len, " Vista", sizeof(" Vista") - 1);
                        len += sizeof(" Vista") - 1;
                    break;
                    default:
                        std::memcpy(OS + len, " v6 Other", sizeof(" v6 Other") - 1);
                        len += sizeof(" v6 Other") - 1;
                    break;
                }
            break;
            case 5:
                    std::memcpy(OS + len, " XP", sizeof(" XP") - 1);
                    len += sizeof(" XP") - 1;
            break;
            default:
                    std::memcpy(OS + len, " Other", sizeof(" Other") - 1);
                    len += sizeof(" Other") - 1;
            break;
        }

        int64_t versionLen = std::strlen(versionInfo.szCSDVersion);
        std::memcpy(OS + len, versionInfo.szCSDVersion, versionLen);
        len += versionLen;
#else
        struct utsname unameData;
        uname(&unameData);
        int64_t nameLen = std::strlen(nameData.sysname);
        std::memcpy(OS + len, nameData.sysname, nameLen);
        len += nameLen;
        OS[len++] = ' ';
        nameLen = std::strlen(nameData.release);
        std::memcpy(OS + len, nameData.release, nameLen);
        len += nameLen;
#endif
        System::OS_ = UTF8Str{len, std::shared_ptr<const char>(OS, std::free)};
    }

    void System::findCPU() {
        char* mModelName = static_cast<char*>(std::malloc(16 * 3 + 1));

        int64_t len = 0;
        UTF8Str part;
        for(unsigned int i = 0x80000002; i <= 0x80000004; i++) {
            CPUID cpuID(i, 0);
            part = String::asAscii((const char*) &cpuID.RAX(), 4);
            std::memcpy(mModelName + len, part.get(), part.length());
            len += part.length();
            part = String::asAscii((const char*) &cpuID.RBX(), 4);
            std::memcpy(mModelName + len, part.get(), part.length());
            len += part.length();
            part = String::asAscii((const char*) &cpuID.RCX(), 4);
            std::memcpy(mModelName + len, part.get(), part.length());
            len += part.length();
            part = String::asAscii((const char*) &cpuID.RDX(), 4);
            std::memcpy(mModelName + len, part.get(), part.length());
            len += part.length();
        }

        mModelName[len] = '\0';
        mModelName = static_cast<char*>(std::realloc(mModelName, len + 1));
        System::CPU_ = UTF8Str{len, std::shared_ptr<const char>(mModelName, std::free)};
    }
}
