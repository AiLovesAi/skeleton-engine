#pragma once

#include <string>

namespace game {
    class File {
        public:
            // Functions
            static void init();

            static void const ensureParentDir(const std::string& path);
            static std::string const readFile(const std::string& file);
            static void const writeFile(const std::string& file, const std::string& data) { writeFile(file, data, false); }
            static void const writeFile(const std::string& file, const std::string& data, const bool append);
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
