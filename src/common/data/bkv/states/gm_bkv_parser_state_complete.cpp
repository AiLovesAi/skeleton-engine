#include "gm_bkv_parser_state_complete.hpp"

#include "../gm_bkv_parser.hpp"

#include <cctype>
#include <sstream>
#include <stdexcept>

namespace game {
    void BKV_Parser_State_Complete::parse(BKV_Parser& parser, const char c) {
        parser._charactersRead++;
        
        if (std::isspace(c) || (c == '\0')) {
            return;
        } else {
            std::stringstream msg;
            msg << "Enclosing BKV compound has already been closed and the bkv is finished at index: " << parser._charactersRead << ".";
            throw std::runtime_error(msg.str());
        }
    }
}
