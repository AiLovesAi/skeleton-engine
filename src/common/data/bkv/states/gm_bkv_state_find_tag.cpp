#include "gm_bkv_state_find_tag.hpp"

#include "gm_bkv_states.hpp"
#include "../gm_bkv.hpp"
#include "../gm_bkv_buffer.hpp"

#include <sstream>
#include <stdexcept>

namespace game {
    void BKV_State_Find_Tag::parseStr(BKV_Buffer& buf, const char c) {
        buf.tag_ = BKV::BKV_STR;
        buf.stateTree_.pop();
        buf.stateTree_.push(BKV_States::stringState());
        try {
            buf.state()->parse(buf, c);
        } catch (std::exception &e) {
            throw e;
        }
    }
    
    void BKV_State_Find_Tag::parse(BKV_Buffer& buf, const char c) {
        if (c == '\'' || c == '"') {
            // String that allows UTF-8 since it must be closed
            parseStr(buf, c);
        } else if (std::isdigit(c) || c == '-' || c == '.') {
            buf.stateTree_.pop();
            buf.stateTree_.push(BKV_States::numberState());
            try { buf.state()->parse(buf, c); } catch (std::exception &e) { throw e; }
        } else if (c == '[') {
            if (buf.tag_ & BKV::BKV_ARRAY) {
                std::stringstream msg;
                msg << "New array in unclosed BKV array at index " << buf.head_ + 1 << ".";
                throw std::invalid_argument(msg.str());
            }

            buf.tag_ |= BKV::BKV_ARRAY;
            buf.stateTree_.pop();
            buf.stateTree_.push(BKV_States::arrayState());
        } else if (c == '{') {
            if (buf.tag_ & BKV::BKV_ARRAY) {
                std::stringstream msg;
                msg << "Compound in unclosed BKV array at index " << buf.head_ + 1 << ".";
                throw std::invalid_argument(msg.str());
            }
            
            // Increase compound depth and return to name state for next input
            buf.bkv_[buf.tagHead_] = BKV::BKV_COMPOUND;
            buf.depth_++;
            if (buf.depth_ > UINT8_MAX) {
                std::stringstream msg;
                msg << "Reached maximum compound depth in BKV at index " << buf.head_ + 1 << ": " << buf.depth_ << "/255.";
                throw std::overflow_error(msg.str());
            }

            buf.stateTree_.pop();
            buf.stateTree_.push(BKV_States::keyState());
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
