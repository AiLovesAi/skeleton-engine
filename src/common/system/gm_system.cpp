#include "gm_system.hpp"

#include "../gm_core.hpp"
#include "../data/file/gm_logger.hpp"
#include "../headers/string.hpp"

#include <lzma.h>

#if defined(_WIN32)
  #include <windows.h>
#elif defined(__linux__)
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
        StringBuffer OS;
#if defined(_WIN32)
        OS.append("Windows");

        OSVERSIONINFOA versionInfo{};
        versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
        GetVersionExA(&versionInfo);

        switch (versionInfo.dwMajorVersion) {
            case 10:
                switch (versionInfo.dwMinorVersion) {
                    case 0:
                        OS.append(" 10");
                    break;
                    default:
                        OS.append(" v10 Other");
                    break;
                }
            break;
            case 6:
                switch (versionInfo.dwMinorVersion) {
                    case 3:
                        OS.append(" 8.1");
                    break;
                    case 2:
                        OS.append(" 8");
                    break;
                    case 1:
                        OS.append(" 7");
                    break;
                    case 0:
                        OS.append(" Vista");
                    break;
                    default:
                        OS.append(" v6 Other");
                    break;
                }
            break;
            case 5:
                OS.append(" XP");
            break;
            default:
                OS.append(" Other");
            break;
        }

        if (String::isAscii(versionInfo.szCSDVersion)) OS.append(versionInfo.szCSDVersion);
#else
        struct utsname unameData;
        uname(&unameData);
        OS.append(nameData.sysname);
        OS.append(' ');
        OS.append(nameData.release);
#endif
        System::OS_ = OS.str();
    }

    void System::findCPU() {
        StringBuffer mModelName{16 * 3 + 1};

        for(unsigned int i = 0x80000002; i <= 0x80000004; i++) {
            CPUID cpuID(i, 0);
            mModelName.append(String::asAscii((const char*) &cpuID.RAX(), 4));
            mModelName.append(String::asAscii((const char*) &cpuID.RBX(), 4));
            mModelName.append(String::asAscii((const char*) &cpuID.RCX(), 4));
            mModelName.append(String::asAscii((const char*) &cpuID.RDX(), 4));
        }

        System::CPU_ = mModelName.str();
    }
}
