#pragma once

#include "../string/gm_utf8.hpp"

#include <cstdint>
#include <memory>

namespace game {
    class File {
        public:
            // Types
            typedef struct FileContents_ {
                size_t len = 0;
                std::shared_ptr<const uint8_t> data = nullptr;
            } FileContents;

            // Functions
            static void init();

            static void const ensureParentDir(const UTF8Str& path);
            
            static FileContents const readFile(const char* filepath);
            static void const writeFile(const char* filepath, const FileContents& contents) { writeFile(filepath, contents, false); }
            static void const writeFile(const char* filepath, const FileContents& contents, const bool append);
            
            static UTF8Str executableDir() { return executableDir_; }

            // Variables
            // Level 3 seems to be a good compromise between compression size and speed.
            // Level 2-3 has a noticeable difference in size and moderate increase in compression time.
            // Level 4 has very little increase but double the time of 3.
            static constexpr uint32_t COMPRESSION_PRESET = 3;
        
        protected:
            friend class Compression;
            // Variables
            static std::mutex fileMtx;
            
        private:
            // Functions
            static void findExecutableDir();

            // Variables
            static UTF8Str executableDir_;
    };
}
