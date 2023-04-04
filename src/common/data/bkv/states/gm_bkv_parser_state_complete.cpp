#include "gm_bkv_parser_state_complete.hpp"

#include "../gm_bkv_parser.hpp"
#include "../../../headers/string.hpp"

#include <cctype>
#include <stdexcept>

namespace game {
    void BKV_Parser_State_Complete::parse(BKV_Parser& parser, const char c) {
        parser._charactersRead++;
        
        if (std::isspace(c) || (c == '\0')) {
            return;
        } else {
            UTF8Str msg = FormatString::formatString("Enclosing BKV compound has already been closed and the bkv is finished at index: %d.",
                parser._charactersRead
            );
            throw std::runtime_error(msg.get());
        }
    }
}
