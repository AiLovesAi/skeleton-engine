#pragma once

#include <string>

namespace game {
    class File {
        public:
            // Functions
            static void init();

            static void const ensureParentDir(const std::string& path);
            static void const ensureExistence(const std::string& file);
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
