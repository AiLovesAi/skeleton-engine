#include "gm_bkv_state_find_tag.hpp"

#include "../gm_bkv.hpp"
#include "../gm_bkv_buffer.hpp"

#include <sstream>
#include <stdexcept>

namespace game {
    void BKV_State_Find_Tag::parseStr(BKV_Buffer& buf, const char c) {
        buf.tag_ |= BKV::BKV_STR;
        buf.stateTree_.pop();
        buf.stateTree_.push(&buf.stringState_);
        buf.charactersRead_--; // String state will increment for us, do not duplicate.
        try {
            buf.state()->parse(buf, c);
        } catch (std::runtime_error &e) {
            throw;
        }
    }
    
    void BKV_State_Find_Tag::parse(BKV_Buffer& buf, const char c) {
        std::stringstream m;
        m << "Find tag state parsing character: " << c;
        Logger::log(LOG_INFO, m.str());
        buf.charactersRead_++;

        if (c == '\'' || c == '"') {
            // String that allows UTF-8 since it must be closed
            parseStr(buf, c);
        } else if (std::isdigit(c) || c == '-' || c == '.') {
            buf.stateTree_.pop();
            buf.stateTree_.push(&buf.numberState_);
            buf.charactersRead_--; // Number state will increment for us, do not duplicate.
            try { buf.state()->parse(buf, c); } catch (std::runtime_error &e) { throw; }
        } else if (c == '[') {
            if (buf.tag_ & BKV::BKV_ARRAY) {
                std::stringstream msg;
                msg << "New array in unclosed BKV array at index " << buf.charactersRead_ << ".";
                throw std::runtime_error(msg.str());
            }

            buf.tag_ |= BKV::BKV_ARRAY;
            buf.stateTree_.pop();
            buf.stateTree_.push(&buf.arrayState_);
        } else if (c == '{') {
            buf.openCompound();
            buf.stateTree_.pop();
            buf.stateTree_.push(&buf.keyState_);
        } else if (std::isalnum(c)) {
            // String that only allows ASCII
            parseStr(buf, c);
        } else if (std::isspace(c)) {
            // Whitespace, ignore
            return;
        } else {
            std::stringstream msg;
            msg << "Invalid character in SBKV at index " << buf.charactersRead_ << ": 0x" << std::hex << ((c & 0xf0) >> 4) << std::hex << (c & 0xf);
            throw std::runtime_error(msg.str());
        }
    }
}
