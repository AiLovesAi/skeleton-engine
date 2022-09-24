#pragma once

#include <cstdint>
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

    class System {
        public:
            // Functions
            static void init();

            static void setGPU(const std::string& GPU) { GPU_ = GPU; }

            static std::string OS() { return OS_; }
            static std::string CPU() { return CPU_; }
            static std::string GPU() { return GPU_; }
        
        private:
            // Functions
            static void findOS();
            static void findCPU();

            // Variables
            static std::string OS_;
            static std::string CPU_;
            static std::string GPU_;
    };
}
