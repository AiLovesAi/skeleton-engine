#pragma once

#include <string>

namespace game {
class CPUID {
    uint32_t regs[4];

    public:
    explicit CPUID(unsigned funcId, unsigned subFuncId) {
        asm volatile
            ("cpuid" : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
             : "a" (funcId), "c" (subFuncId));
        // ECX is set to zero for CPUID function 4
    }

    const uint32_t &RAX() const {return regs[0];}
    const uint32_t &RBX() const {return regs[1];}
    const uint32_t &RCX() const {return regs[2];}
    const uint32_t &RDX() const {return regs[3];}
};
class File {
    public:
        static void init();

        static void const ensureParentDir(std::string path);
        static std::string const asAscii(std::string str);
        static bool const isAscii(std::string str);
        static std::string const getStack();
        
        static std::string OSStr;
        static std::string CPUStr;
        static std::string executableDir;
    private:
        static void getOS();
        static void getCPU();
        static void getExecutableDir();
};
}
