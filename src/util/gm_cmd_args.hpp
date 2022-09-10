#pragma once

#include <string>

namespace game {
    class CmdArgs {
        public:
            static char* cmdOption(char** begin, char** end, const std::string& option);
            static bool cmdOptionExists(char** begin, char** end, const std::string& option);
    };
}