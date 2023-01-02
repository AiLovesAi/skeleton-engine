#include "gm_bkv_parser_state_find_tag.hpp"

#include "../gm_bkv.hpp"
#include "../gm_bkv_parser.hpp"

#include <sstream>
#include <stdexcept>

namespace game {
    void BKV_Parser_State_Find_Tag::parseStr(BKV_Parser& parser, const char c) {
        parser.tag_ |= BKV::BKV_STR;
        parser.stateTree_.pop();
        parser.stateTree_.push(&parser.stringState_);
        parser.charactersRead_--; // String state will increment for us, do not duplicate.
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
        parser.charactersRead_++;

        if (c == '\'' || c == '"') {
            // String that allows UTF-8 since it must be closed
            parseStr(parser, c);
        } else if (std::isdigit(c) || c == '-' || c == '.') {
            parser.stateTree_.pop();
            parser.stateTree_.push(&parser.numberState_);
            parser.charactersRead_--; // Number state will increment for us, do not duplicate.
            try { parser.state()->parse(parser, c); } catch (std::runtime_error &e) { throw; }
        } else if (c == '[') {
            if (parser.tag_ & BKV::BKV_ARRAY) {
                std::stringstream msg;
                msg << "New array in unclosed BKV array at index " << parser.charactersRead_ << ".";
                throw std::runtime_error(msg.str());
            }

            parser.tag_ |= BKV::BKV_ARRAY;
            parser.stateTree_.pop();
            parser.stateTree_.push(&parser.arrayState_);
        } else if (c == '{') {
            parser.openCompound();
            parser.stateTree_.pop();
            parser.stateTree_.push(&parser.keyState_);
        } else if (std::isalnum(c)) {
            // String that only allows ASCII
            parseStr(parser, c);
        } else if (std::isspace(c)) {
            // Whitespace, ignore
            return;
        } else {
            std::stringstream msg;
            msg << "Invalid character in SBKV at index " << parser.charactersRead_ << ": 0x" << std::hex << ((c & 0xf0) >> 4) << std::hex << (c & 0xf);
            throw std::runtime_error(msg.str());
        }
    }
}
