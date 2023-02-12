#pragma once

#include "../data/string/gm_utf8.hpp"

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

            static void setGPU(const UTF8Str GPU) { GPU_ = GPU; }

            static UTF8Str OS() { return OS_; }
            static UTF8Str CPU() { return CPU_; }
            static UTF8Str GPU() { return GPU_; }
            static uint32_t cpuThreadCount() { return CPU_THREAD_COUNT_; }
            static size_t physicalMemory() { return PHYSICAL_MEMORY_; }
        
        private:
            // Functions
            static void findOS();
            static void findCPU();

            // Variables
            static uint32_t CPU_THREAD_COUNT_;
            static size_t PHYSICAL_MEMORY_;
            static UTF8Str OS_;
            static UTF8Str CPU_;
            static UTF8Str GPU_;
    };
}
