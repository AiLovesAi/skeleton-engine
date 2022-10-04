#pragma once

#include <cstdint>
#include <string>

namespace game {
    class File {
        public:
            // Functions
            static void init();

            static void const ensureParentDir(const std::string& path);
            
            static uint8_t* const readFile(const char* filepath, size_t* len) __attribute__((warn_unused_result));
            static void const writeFile(const char* filepath, const uint8_t* data, const size_t len) { writeFile(filepath, data, len, false); }
            static void const writeFile(const char* filepath, const uint8_t* data, const size_t len, const bool append);
            
            static uint8_t* const decompressFile(const char* filepath, size_t* len) __attribute__((warn_unused_result));
            static void const compressFile(const char* filepath, const uint8_t* data, const size_t len) { compressFile(file, data, false); }
            static void const compressFile(const char* filepath, const uint8_t* data, const size_t len, const bool append);
            
            static std::string const asAscii(const std::string& str);
            static bool const isAscii(const std::string& str);

            static std::string executableDir() { return executableDir_; }
            
        private:
            // Functions
            static void findExecutableDir();

            // Variables
            static std::string executableDir_;
    };
}
