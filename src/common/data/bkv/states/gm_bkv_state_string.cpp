#include "gm_bkv_state_string.hpp"

#include "../../gm_endianness.hpp"
#include "../../gm_buffer_memory.hpp"

#include <sstream>
#include <stdexcept>

namespace game {
    void BKV_State_String::continueStr(BKV_Buffer& buf, const char c) {
        strLen_++;
        if (strLen_ >= UINT16_MAX) {
            std::stringstream msg;
            msg << "Too many characters in BKV string: " << strLen_ << "/65535 characters.";
            throw std::length_error(msg.str());
        }
        BufferMemory::checkResize(str_, strLen_, strCapacity_);
        str_[strLen_ - 1] = c;
        lastChar_ = c;
    }
    void BKV_State_String::completeStr(BKV_Buffer& buf) {
        const size_t len = Endianness::hton(strLen_);
        std::memcpy(buf.bkv + buf.head, &len, sizeof(uint8_t));
        buf.head ++;
        std::memcpy(buf.bkv + buf.head, str_, strLen_);
        buf.head += strLen_;
        buf.valHead = buf.head;
        buf.state = (buf.tag == BKV::BKV_STR_ARRAY) ? BKV_State::arrayState() : BKV_State::nameState();
        reset();
    }
    
    void BKV_State_String::parse(BKV_Buffer& buf, const char c) {
        if (strChar_ != DEFAULT_CHAR) { // Any UTF-8 string allowed
            if (c == strChar_ && lastChar_ != '\\') {
                completeStr(buf);
            } else {
                continueStr(buf, c);
            }
        } else { // Only alphanumeric ASCII allowed
            if (std::isalnum(c)) {
                continueStr(buf, c);
            } else if (c == ',' || c == '}') {
                completeStr(buf);
            } else {
                std::stringstream msg;
                msg << "Invalid character in BKV string at index " << strLen_ << ": 0x" << ((c & 0xf0) >> 4) << (c & 0xf);
                throw std::invalid_argument(msg.str());
            }
        }
    }
}
