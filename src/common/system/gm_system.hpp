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

            static void setGPU(const UTF8Str GPU) { _GPU = GPU; }
            static void setMonitor(const UTF8Str monitor) { _monitor = monitor; }

            static UTF8Str OS() { return _OS; }
            static UTF8Str CPU() { return _CPU; }
            static UTF8Str GPU() { return _GPU; }
            static UTF8Str monitor() { return _monitor; }
            static uint32_t cpuThreadCount() { return _CPU_THREAD_COUNT; }
            static size_t physicalMemory() { return _PHYSICAL_MEMORY; }
        
        private:
            // Functions
            static void findOS();
            static void findCPU();

            // Variables
            static uint32_t _CPU_THREAD_COUNT;
            static size_t _PHYSICAL_MEMORY;
            static UTF8Str _OS;
            static UTF8Str _CPU;
            static UTF8Str _GPU;
            static UTF8Str _monitor;
    };
}
