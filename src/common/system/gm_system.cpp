#include "gm_system.hpp"

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
    uint32_t System::_CPU_THREAD_COUNT;
    size_t System::_PHYSICAL_MEMORY;
    UTF8Str System::_OS = EMPTY_STR;
    UTF8Str System::_CPU = EMPTY_STR;
    UTF8Str System::_GPU = EMPTY_STR;

    void System::init() {
        findOS();
        findCPU();
        _CPU_THREAD_COUNT = lzma_cputhreads();
        _PHYSICAL_MEMORY = lzma_physmem();
        if (_CPU_THREAD_COUNT < 1) {
            _CPU_THREAD_COUNT = 1;
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
        System::_OS = OS.str();
    }

    void System::findCPU() {
        StringBuffer mModelName{16 * 3 + 1};

        for(unsigned int i = 0x80000002; i <= 0x80000004; i++) {
            CPUID cpuID(i, 0);
            mModelName.append(String::asAscii(reinterpret_cast<const char*>(&cpuID.RAX()), 4));
            mModelName.append(String::asAscii(reinterpret_cast<const char*>(&cpuID.RBX()), 4));
            mModelName.append(String::asAscii(reinterpret_cast<const char*>(&cpuID.RCX()), 4));
            mModelName.append(String::asAscii(reinterpret_cast<const char*>(&cpuID.RDX()), 4));
        }

        System::_CPU = mModelName.str();
    }
}
