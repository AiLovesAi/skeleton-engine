#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace game {
    // Types
    typedef struct FileContents_ {
        size_t len = 0;
        std::shared_ptr<uint8_t> data = nullptr;
    } FileContents;
    
    class File {
        public:

            // Functions
            static void init();

            static void const ensureParentDir(const std::string& path);
            
            static FileContents const readFile(const char* filepath);
            static void const writeFile(const char* filepath, const FileContents& contents) { writeFile(filepath, contents, false); }
            static void const writeFile(const char* filepath, const FileContents& contents, const bool append);
            
            static FileContents const decompressFile(const char* filepath);
            static void const compressFile(const char* filepath, const FileContents& contents) { compressFile(filepath, contents, false); }
            static void const compressFile(const char* filepath, const FileContents& contents, const bool append);
            
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
