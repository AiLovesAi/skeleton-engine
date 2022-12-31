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

        if (c == '\'' || c == '"') {
            // String that allows UTF-8 since it must be closed
            parseStr(buf, c);
        } else if (std::isdigit(c) || c == '-' || c == '.') {
            buf.stateTree_.pop();
            buf.stateTree_.push(&buf.numberState_);
            try { buf.state()->parse(buf, c); } catch (std::runtime_error &e) { throw; }
        } else if (c == '[') {
            if (buf.tag_ & BKV::BKV_ARRAY) {
                std::stringstream msg;
                msg << "New array in unclosed BKV array at index " << buf.head_ + 1 << ".";
                throw std::invalid_argument(msg.str());
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
            msg << "Invalid character in BKV string at index " << buf.head_ + 1 << ": 0x" << ((c & 0xf0) >> 4) << (c & 0xf);
            throw std::invalid_argument(msg.str());
        }
    }
}
