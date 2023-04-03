#include "gm_bkv_parser_state_find_tag.hpp"

#include "../gm_bkv.hpp"
#include "../gm_bkv_parser.hpp"

#include <sstream>
#include <stdexcept>

namespace game {
    void BKV_Parser_State_Find_Tag::parseStr(BKV_Parser& parser, const char c) {
        parser._tag |= BKV::BKV_STR;
        parser._stateTree.pop();
        parser._stateTree.push(&parser._stringState);
        parser._charactersRead--; // String state will increment for us, do not duplicate.
        try {
            parser.state()->parse(parser, c);
        } catch (std::runtime_error &e) {
            throw;
        }
    }
    
    void BKV_Parser_State_Find_Tag::parse(BKV_Parser& parser, const char c) {
        std::stringstream m;
        m << "Find tag state parsing character: " << c;
        Logger::log(LOG_INFO, m.str());
        parser._charactersRead++;

        if (c == '\'' || c == '"') {
            // String that allows UTF-8 since it must be closed
            parseStr(parser, c);
        } else if (std::isdigit(c) || c == '-' || c == '.') {
            parser._stateTree.pop();
            parser._stateTree.push(&parser._numberState);
            parser._charactersRead--; // Number state will increment for us, do not duplicate.
            try { parser.state()->parse(parser, c); } catch (std::runtime_error &e) { throw; }
        } else if (c == '[') {
            if (parser._tag & BKV::BKV_ARRAY) {
                std::stringstream msg;
                msg << "New array in unclosed BKV array at index " << parser._charactersRead << ".";
                throw std::runtime_error(msg.str());
            }

            parser._tag |= BKV::BKV_ARRAY;
            parser._stateTree.pop();
            parser._stateTree.push(&parser._arrayState);
        } else if (c == '{') {
            parser.openCompound();
            parser._stateTree.pop();
            parser._stateTree.push(&parser._keyState);
        } else if (std::isalnum(c)) {
            // String that only allows ASCII
            parseStr(parser, c);
        } else if (std::isspace(c) || c == ':') {
            // Whitespace or colon, ignore
            return;
        } else {
            std::stringstream msg;
            msg << "Invalid character in SBKV at index " << parser._charactersRead << ": 0x" << std::hex << ((c & 0xf0) >> 4) << std::hex << (c & 0xf);
            throw std::runtime_error(msg.str());
        }
    }
}
