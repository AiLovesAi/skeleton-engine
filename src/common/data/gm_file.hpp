#pragma once

#include <string>

extern "C" {
    #include <stdint.h>

    char* readFileC(const char* file);
    void writeFileC(const char* file, const char* data, const uint8_t append);
}

namespace game {
    class File {
        public:
            // Functions
            static void init();

            static void const ensureParentDir(const std::string& path);
            static std::string const readFile(const std::string& file);
            static std::string const decompressFile(const std::string& file);
            static void const writeFile(const std::string& file, const std::string& data) { writeFile(file, data, false); }
            static void const writeFile(const std::string& file, const std::string& data, const bool append);
            static void const compressFile(const std::string& file, const std::string& data) { compressFile(file, data, false); }
            static void const compressFile(const std::string& file, const std::string& data, const bool append);
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
