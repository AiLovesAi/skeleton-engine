#include "gm_bkv_state_string.hpp"

#include "../../gm_endianness.hpp"
#include "../../gm_buffer_memory.hpp"

#include <sstream>
#include <stdexcept>

namespace game {
    void BKV_State_String::continueStr(BKV_Buffer& buf, const char c) {
        // Build string
        if (strLen_ >= UINT16_MAX) {
            reset();
            std::stringstream msg;
            msg << "Too many characters in BKV string: " << strLen_ + 1 << "/65535 characters.";
            throw std::length_error(msg.str());
        }

        try {
            BufferMemory::checkResize(str_, strLen_ + 1, strLen_, strCapacity_);
        } catch (std::exception e) {
            reset();
            throw e;
        }

        str_[strLen_] = c;
        strLen_++;

        // Toggle break character after it breaks once
        if (breakChar_) {
            breakChar_ = false;
        } else if (c == '\\') {
            breakChar_ = true;
        }
    }

    void BKV_State_String::completeStr(BKV_Buffer& buf, const char c) {
        if (!(buf.tag & BKV::BKV_ARRAY)) {
            // Check if this is a boolean "true" or "false"
            if ((strLen_ == 4) && (
                std::tolower(str_[0]) == 't' &&
                std::tolower(str_[1]) == 'r' &&
                std::tolower(str_[2]) == 'u' &&
                std::tolower(str_[3]) == 'e'
            )) {
                buf.tag = BKV::BKV_BOOL;
                try {
                    BufferMemory::checkResize(buf.bkv, buf.head + 1, buf.head, buf.capacity);
                } catch (std::exception e) {
                    reset();
                    throw e;
                }

                // 0x01 for true
                buf.bkv[buf.head] = 0x01;
                buf.head++;
            } else if ((strLen_ == 5) && (
                std::tolower(str_[0]) == 'f' &&
                std::tolower(str_[1]) == 'a' &&
                std::tolower(str_[2]) == 'l' &&
                std::tolower(str_[3]) == 's' &&
                std::tolower(str_[4]) == 'e'
            )) {
                buf.tag = BKV::BKV_BOOL;
                try {
                    BufferMemory::checkResize(buf.bkv, buf.head + 1, buf.head, buf.capacity);
                } catch (std::exception e) {
                    reset();
                    throw e;
                }

                // 0x01 for false
                buf.bkv[buf.head] = 0x00;
                buf.head++;
            }
        }
        if (buf.tag != BKV::BKV_BOOL) {
            // Must be string, copy to buffer
            buf.tag |= BKV::BKV_STR;
            const size_t len = Endianness::hton(strLen_);
            try {
                BufferMemory::checkResize(buf.bkv, buf.head + (int64_t) sizeof(uint16_t) + strLen_, buf.head, buf.capacity);
            } catch (std::exception e) {
                reset();
                throw e;
            }

            std::memcpy(buf.bkv + buf.head, &len, sizeof(uint16_t));
            buf.head += sizeof(uint16_t);
            std::memcpy(buf.bkv + buf.head, str_, strLen_);
            buf.head += strLen_;
        }

        reset();
        try { buf.endKV(c); } catch (std::exception e) { throw e; }
    }
    
    void BKV_State_String::parse(BKV_Buffer& buf, const char c) {
        if (strChar_ != DEFAULT_CHAR) { // Any UTF-8 string allowed
            if (c == strChar_ && !breakChar_) {
                completeStr(buf, c);
            } else {
                continueStr(buf, c);
            }
        } else { // Only alphanumeric ASCII allowed
            if (std::isalnum(c)) {
                continueStr(buf, c);
            } else if ((c == '}') || (c == ',') || ((buf.tag & BKV::BKV_ARRAY) && (c == ']'))) {
                completeStr(buf, c);
            } else {
                reset();
                std::stringstream msg;
                msg << "Invalid character in BKV string at index " << strLen_ << ": 0x" << ((c & 0xf0) >> 4) << (c & 0xf);
                throw std::invalid_argument(msg.str());
            }
        }
    }
}
