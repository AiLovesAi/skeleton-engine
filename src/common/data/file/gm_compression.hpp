#pragma once

#include "gm_file.hpp"

namespace game {
    class Compression {
        public:
            // Functions
            static File::FileContents const decompressFile(const char* filepath);
            static void const compressFile(const char* filepath, const File::FileContents& contents) { compressFile(filepath, contents, false); }
            static void const compressFile(const char* filepath, const File::FileContents& contents, const bool append);
    };
}
