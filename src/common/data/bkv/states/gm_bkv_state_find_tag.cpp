#include "gm_bkv_state_find_tag.hpp"

#include <sstream>
#include <stdexcept>

namespace game {
    static void parseStr(BKV_Buffer& buf, const char c) {
        buf.tag = BKV::BKV_STR;
        buf.state = BKV_State::stringState();
        buf.parentState = BKV_State::stringState();
        try {
            buf.state->parse(buf, c);
        } catch (std::exception e) {
            throw e;
        }
    }
    
    void BKV_State_Find_Tag::parse(BKV_Buffer& buf, const char c) {
        if (c == '\'' || c == '"') {
            // String that allows UTF-8 since it must be closed
            parseStr(buf, c);
        } else if (std::isdigit(c) || c == '-' || c == '.') {
            buf.state = BKV_State::numberState();
            buf.parentState = BKV_State::numberState();
            try { buf.state->parse(buf, c); } catch (std::exception e) { throw e; }
        } else if (c == '[') {
            if (buf.tag & BKV::BKV_ARRAY) {
                std::stringstream msg;
                msg << "New array in unclosed BKV array at index " << buf.head + 1 << ".";
                throw std::invalid_argument(msg.str());
            }
            buf.tag |= BKV::BKV_ARRAY;
            buf.state = BKV_State::arrayState();
        } else if (c == '{') {
            if (buf.tag & BKV::BKV_ARRAY) {
                std::stringstream msg;
                msg << "Compound in unclosed BKV array at index " << buf.head + 1 << ".";
                throw std::invalid_argument(msg.str());
            }
            // Increase compound depth and return to name state for next input
            buf.bkv[buf.tagHead] = BKV::BKV_COMPOUND;
            buf.depth++;
            if (buf.depth > UINT8_MAX) {
                std::stringstream msg;
                msg << "Reached maximum compound depth in BKV at index " << buf.head + 1 << ": " << buf.depth << "/255.";
                throw std::overflow_error(msg.str());
            }
            buf.state = BKV_State::keyState();
        } else if (std::isalnum(c)) {
            // String that only allows ASCII
            parseStr(buf, c);
        } else {
            std::stringstream msg;
            msg << "Invalid character in BKV string at index " << buf.head + 1 << ": 0x" << ((c & 0xf0) >> 4) << (c & 0xf);
            throw std::invalid_argument(msg.str());
        }
    }
}
