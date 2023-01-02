#include "gm_bkv_parser_state_string.hpp"

#include "../gm_bkv.hpp"
#include "../gm_bkv_parser.hpp"
#include "../../gm_endianness.hpp"
#include "../../gm_buffer_memory.hpp"

#include <sstream>
#include <stdexcept>

namespace game {
    char BKV_Parser_State_String::getBreakChar(const char c) {
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
    
    void BKV_Parser_State_String::continueStr(BKV_Parser& parser, const char c) {
        // Build string
        if (strLen_ >= BKV::BKV_STR_MAX) {
            std::stringstream msg;
            msg << "Too many characters in SBKV string at index " << parser.charactersRead_ << ": " << strLen_ + 1 << "/" << BKV::BKV_STR_MAX << " characters.";
            reset();
            throw std::runtime_error(msg.str());
        }

        try {
            BufferMemory::checkResize(str_, strLen_ + 1, strLen_, strCapacity_);
        } catch (std::runtime_error &e) {
            reset();
            throw;
        }

        // Toggle break character after it breaks once
        if (breakChar_) {
            breakChar_ = false;

            char b = BKV_Parser_State_String::getBreakChar(c);
            if (b < 0) {
                std::stringstream msg;
                msg << "Invalid break character in SBKV string at index " << parser.charactersRead_ << ": 0x" << std::hex << ((c & 0xf0) >> 4) << std::hex << (c & 0xf);
                reset();
                throw std::runtime_error(msg.str());
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

    void BKV_Parser_State_String::checkForBool(BKV_Parser& parser) {
        // Check if this is a boolean "true" or "false"
        if ((strLen_ == 4) && (
            ((str_[0] == 't') || (str_[0] == 'T')) &&
            ((str_[1] == 'r') || (str_[1] == 'R')) &&
            ((str_[2] == 'u') || (str_[2] == 'U')) &&
            ((str_[3] == 'e') || (str_[3] == 'E'))
        )) {
            parser.tag_ &= ~BKV::BKV_STR;
            parser.tag_ |= BKV::BKV_BOOL;
            try {
                BufferMemory::checkResize(parser.buffer_.bkv_, parser.buffer_.head_ + 1, parser.buffer_.head_, parser.buffer_.capacity_);
            } catch (std::runtime_error &e) {
                reset();
                throw;
            }

            // 0x01 for true
            parser.buffer_.bkv_[parser.buffer_.head_] = 0x01;
            parser.buffer_.head_++;
        Logger::log(LOG_INFO, "String state found true bool.");
        } else if ((strLen_ == 5) && (
            ((str_[0] == 'f') || (str_[0] == 'F')) &&
            ((str_[1] == 'a') || (str_[1] == 'A')) &&
            ((str_[2] == 'l') || (str_[2] == 'L')) &&
            ((str_[3] == 's') || (str_[3] == 'S')) &&
            ((str_[4] == 'e') || (str_[4] == 'E'))
        )) {
            parser.tag_ &= ~BKV::BKV_STR;
            parser.tag_ |= BKV::BKV_BOOL;
            try {
                BufferMemory::checkResize(parser.buffer_.bkv_, parser.buffer_.head_ + 1, parser.buffer_.head_, parser.buffer_.capacity_);
            } catch (std::runtime_error &e) {
                reset();
                throw;
            }

            // 0x01 for false
            parser.buffer_.bkv_[parser.buffer_.head_] = 0x00;
            parser.buffer_.head_++;
        Logger::log(LOG_INFO, "String state found false bool.");
        }
    }

    void BKV_Parser_State_String::completeStr(BKV_Parser& parser, const char c) {
        if (!strChar_) {
            // Only check for bool if it is not a string. "True" is a string and True is a boolean.
            try { checkForBool(parser); } catch (std::runtime_error &e) { throw; }
        }

        if (!(parser.tag_ & BKV::BKV_BOOL)) {
            // Must be string, copy to buffer
            parser.tag_ |= BKV::BKV_STR;
            const uint16_t len = Endianness::hton(static_cast<uint16_t>(strLen_));
            try {
                BufferMemory::checkResize(parser.buffer_.bkv_, parser.buffer_.head_ + BKV::BKV_STR_SIZE + strLen_, parser.buffer_.head_, parser.buffer_.capacity_);
            } catch (std::runtime_error &e) {
                reset();
                throw;
            }

            std::memcpy(parser.buffer_.bkv_ + parser.buffer_.head_, &len, BKV::BKV_STR_SIZE);
            parser.buffer_.head_ += BKV::BKV_STR_SIZE;
            std::memcpy(parser.buffer_.bkv_ + parser.buffer_.head_, str_, strLen_);
            parser.buffer_.head_ += strLen_;
        char* key = static_cast<char*>(std::malloc(strLen_ + 1));
        std::memcpy(key, str_, strLen_);
        key[strLen_] = '\0';
        std::stringstream m;
        m << "String state finished parsing: " << key << " with net len: " << len;
        Logger::log(LOG_INFO, m.str());
        }

        reset();
        try { parser.endKV(c); } catch (std::runtime_error &e) { throw; }
    }
    
    void BKV_Parser_State_String::parse(BKV_Parser& parser, const char c) {
        std::stringstream m;
        m << "String state parsing character: " << c;
        Logger::log(LOG_INFO, m.str());
        parser.charactersRead_++;
        
        if (!strLen_ && ((c == '\'') || (c == '"'))) {
            strChar_ = c;
        } else if (strChar_ > 0) { // Any UTF-8 string allowed
            // NOTE: Checking to see if a UTF-8 character piece matches strChar is unnecessary because all UTF-8 characters
            // start with the first bit set, which is the signed bit. All ASCII characters have the signed bit cleared.
            if (c == strChar_ && !breakChar_) {
                strChar_ = -1; // Wait for the following KV end character to call complete string
            } else {
                continueStr(parser, c);
            }
        } else { // Only alphanumeric ASCII allowed
            if (!strChar_ && (std::isalnum(c) || std::isdigit(c) || c == '_' || c == '.' || c == '+' || c == '-')) {
                continueStr(parser, c);
            } else if ((c == '}') || (c == ',') || (c == ']')) {
                if (!strLen_) {
                    std::stringstream msg;
                    msg << "Empty BKV string at index: " << parser.charactersRead_;
                    reset();
                    throw std::runtime_error(msg.str());
                }
                completeStr(parser, c);
            } else if (std::isspace(c)) {
                // Whitespace, ignore
                return;
            } else {
                std::stringstream msg;
                msg << "Invalid character in SBKV string at index " << parser.charactersRead_ << ": 0x" << std::hex << ((c & 0xf0) >> 4) << std::hex << (c & 0xf);
                reset();
                throw std::runtime_error(msg.str());
            }
        }
    }
}
