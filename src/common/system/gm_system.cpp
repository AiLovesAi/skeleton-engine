#include "gm_system.hpp"

#include "../gm_core.hpp"
#include "../data/gm_logger.hpp"
#include "../data/gm_file.hpp"

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
    std::string System::OS_ = Core::EMPTYSTR;
    std::string System::CPU_ = Core::EMPTYSTR;
    std::string System::GPU_ = Core::EMPTYSTR;

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
        std::string OS;
#if defined(_WIN32)
        OS = "Windows";
        OSVERSIONINFOA versionInfo{};
        versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
        GetVersionExA(&versionInfo);

        switch (versionInfo.dwMajorVersion) {
            case 10:
                switch (versionInfo.dwMinorVersion) {
                    case 0:
                        OS += " 10";
                    break;
                    default:
                        OS += " v10 Other";
                    break;
                }
            break;
            case 6:
                switch (versionInfo.dwMinorVersion) {
                    case 3:
                        OS += " 8.1";
                    break;
                    case 2:
                        OS += " 8";
                    break;
                    case 1:
                        OS += " 7";
                    break;
                    case 0:
                        OS += " Vista";
                    break;
                    default:
                        OS += " v6 Other";
                    break;
                }
            break;
            case 5:
                OS += " XP";
            break;
            default:
                OS += " Other";
            break;
        }

        OS += versionInfo.szCSDVersion;
#else
        struct utsname unameData;
        uname(&unameData);
        OS += nameData.sysname;
        OS += " ";
        OS += nameData.release;
#endif
        System::OS_ = OS;
    }

    void System::findCPU() {
        std::string mModelName;

        for(unsigned int i=0x80000002; i<=0x80000004; ++i) {
            CPUID cpuID(i, 0);
            mModelName += File::asAscii(std::string((const char*)&cpuID.RAX(), 4));
            mModelName += File::asAscii(std::string((const char*)&cpuID.RBX(), 4));
            mModelName += File::asAscii(std::string((const char*)&cpuID.RCX(), 4));
            mModelName += File::asAscii(std::string((const char*)&cpuID.RDX(), 4));
        }

        System::CPU_ = mModelName;
    }
}
