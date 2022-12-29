#include "gm_bkv_state_string.hpp"

#include "../gm_bkv.hpp"
#include "../gm_bkv_buffer.hpp"
#include "../../gm_endianness.hpp"
#include "../../gm_buffer_memory.hpp"

#include <sstream>
#include <stdexcept>

namespace game {
    char BKV_State_String::getBreakChar(const char c) {
        switch (c) {
            case '\"': return '\"';
            case '\'': return '\'';
            case '\\': return '\\';
            case '0': return '\0';
            case '1': return '\1';
            case '2': return '\2';
            case '3': return '\3';
            case '4': return '\4';
            case '5': return '\5';
            case '6': return '\6';
            case '7': return '\7';
            case 'a': return '\0';
            case 'b': return '\b';
            case 'c': return '\6';
            case 'f': return '\f';
            case 'n': return '\n';
            case 'r': return '\r';
            case 't': return '\t';
            case 'v': return '\v';
            default: return -1;
        }
    }
    
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
        } catch (std::exception &e) {
            reset();
            throw e;
        }

        // Toggle break character after it breaks once
        if (breakChar_) {
            breakChar_ = false;

            char b = BKV_State_String::getBreakChar(c);
            if (b < 0) {
                reset();
                std::stringstream msg;
                msg << "Invalid break character in BKV string at index " << strLen_ << ": 0x" << ((c & 0xf0) >> 4) << (c & 0xf);
                throw std::invalid_argument(msg.str());
            }
            
            str_[strLen_] = c;
            strLen_++;
        } else if (c == '\\') {
            breakChar_ = true;
        } else {
            str_[strLen_] = c;
            strLen_++;
        }
    }

    void BKV_State_String::checkForBool(BKV_Buffer& buf) {
        // Check if this is a boolean "true" or "false"
        if ((strLen_ == 4) && (
            std::tolower(str_[0]) == 't' &&
            std::tolower(str_[1]) == 'r' &&
            std::tolower(str_[2]) == 'u' &&
            std::tolower(str_[3]) == 'e'
        )) {
            buf.tag_ &= ~BKV::BKV_STR;
            buf.tag_ |= BKV::BKV_BOOL;
            try {
                BufferMemory::checkResize(buf.bkv_, buf.head_ + 1, buf.head_, buf.capacity_);
            } catch (std::exception &e) {
                reset();
                throw e;
            }

            // 0x01 for true
            buf.bkv_[buf.head_] = 0x01;
            buf.head_++;
        Logger::log(LOG_INFO, "String state found true bool.");
        } else if ((strLen_ == 5) && (
            std::tolower(str_[0]) == 'f' &&
            std::tolower(str_[1]) == 'a' &&
            std::tolower(str_[2]) == 'l' &&
            std::tolower(str_[3]) == 's' &&
            std::tolower(str_[4]) == 'e'
        )) {
            buf.tag_ &= ~BKV::BKV_STR;
            buf.tag_ |= BKV::BKV_BOOL;
            try {
                BufferMemory::checkResize(buf.bkv_, buf.head_ + 1, buf.head_, buf.capacity_);
            } catch (std::exception &e) {
                reset();
                throw e;
            }

            // 0x01 for false
            buf.bkv_[buf.head_] = 0x00;
            buf.head_++;
        Logger::log(LOG_INFO, "String state found false bool.");
        }
    }

    void BKV_State_String::completeStr(BKV_Buffer& buf, const char c) {
        try { checkForBool(buf); } catch (std::exception &e) { throw e; }

        if (!(buf.tag_ & BKV::BKV_BOOL)) {
            // Must be string, copy to buffer
            buf.tag_ |= BKV::BKV_STR;
            const size_t len = Endianness::hton(strLen_);
            try {
                BufferMemory::checkResize(buf.bkv_, buf.head_ + (int64_t) sizeof(uint16_t) + strLen_, buf.head_, buf.capacity_);
            } catch (std::exception &e) {
                reset();
                throw e;
            }

            std::memcpy(buf.bkv_ + buf.head_, &len, sizeof(uint16_t));
            buf.head_ += sizeof(uint16_t);
            std::memcpy(buf.bkv_ + buf.head_, str_, strLen_);
            buf.head_ += strLen_;
        char* key = static_cast<char*>(std::malloc(strLen_ + 1));
        std::memcpy(key, str_, strLen_);
        key[strLen_] = '\0';
        std::stringstream m;
        m << "String state finished parsing: " << key;
        Logger::log(LOG_INFO, m.str());
        }

        reset();
        try { buf.endKV(c); } catch (std::exception &e) { throw e; }
    }
    
    void BKV_State_String::parse(BKV_Buffer& buf, const char c) {
        std::stringstream m;
        m << "String state parsing character: " << c;
        Logger::log(LOG_INFO, m.str());
        if (!strLen_ && ((c == '\'') || (c == '"'))) {
            strChar_ = c;
        } else if (strChar_ > 0) { // Any UTF-8 string allowed
            if (c == strChar_ && !breakChar_) {
                strChar_ = -1; // Wait for the following KV end character to call complete string
            } else {
                continueStr(buf, c);
            }
        } else { // Only alphanumeric ASCII allowed
            if (!strChar_ && (std::isalnum(c) || std::isdigit(c) || c == '_' || c == '.' || c == '+' || c == '-')) {
                continueStr(buf, c);
            } else if ((c == '}') || (c == ',') || (c == ']')) {
                completeStr(buf, c);
            } else if (std::isspace(c)) {
                if (!strLen_) {
                    // Whitespace, ignore
                    return;
                } else {
                    checkForBool(buf);
                    if (!(buf.tag_ & BKV::BKV_BOOL)) {
                        std::stringstream msg;
                        msg << "Invalid character in BKV string at index " << strLen_ << ": 0x" << ((c & 0xf0) >> 4) << (c & 0xf);
                        throw std::invalid_argument(msg.str());
                    }
                }
            } else {
                reset();
                std::stringstream msg;
                msg << "Invalid character in BKV string at index " << strLen_ << ": 0x" << ((c & 0xf0) >> 4) << (c & 0xf);
                throw std::invalid_argument(msg.str());
            }
        }
    }
}
