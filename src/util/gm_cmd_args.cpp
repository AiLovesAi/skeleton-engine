#include "gm_cmd_args.hpp"

#include <algorithm>

namespace game {
    char* CmdArgs::getCmdOption(char** begin, char** end, const std::string& option) {
        char** itr = std::find(begin, end, option);
        if (itr != end && ++itr != end)
        {
            return *itr;
        }
        return 0;
    }

    bool CmdArgs::cmdOptionExists(char** begin, char** end, const std::string& option) {
        return std::find(begin, end, option) != end;
    }
}