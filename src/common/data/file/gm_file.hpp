#pragma once

#include "../string/gm_utf8.hpp"

#include <cstdint>
#include <memory>

namespace game {
    class File {
        public:
            // Types
            typedef struct FileContents_ {
                private:
                    size_t len;
                    std::shared_ptr<const uint8_t> data;
                
                public:
                    FileContents_() : len{0}, data{nullptr} {}
                    FileContents_(size_t len, std::shared_ptr<const uint8_t> data) :
                        len{len}, data{data} {}

                    inline size_t length() const {
                        return len;
                    }

                    inline const uint8_t* get() const {
                        return data.get();
                    }
            } FileContents;

            // Functions
            static void init();

            static void const ensureParentDir(const UTF8Str& path);
            
            static FileContents const readFile(const char* filepath);
            static inline void const writeFile(const char* filepath, const FileContents& contents) {
                writeFile(filepath, contents, false);
            }
            static void const writeFile(const char* filepath, const FileContents& contents, const bool append);
            
            static UTF8Str executableDir() { return _executableDir; }

            // Variables
            // Level 3 seems to be a good compromise between compression size and speed.
            // Level 2-3 has a noticeable difference in size and moderate increase in compression time.
            // Level 4 has very little increase but double the time of 3.
            static constexpr uint32_t COMPRESSION_PRESET = 3;
        
        protected:
            friend class Compression;
            // Variables
            static std::mutex _fileMtx;
            
        private:
            // Functions
            static void findExecutableDir();

            // Variables
            static UTF8Str _executableDir;
    };
}
