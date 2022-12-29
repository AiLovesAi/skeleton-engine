#include "gm_bkv_state_key.hpp"

#include "gm_bkv_state_string.hpp"
#include "../gm_bkv.hpp"
#include "../gm_bkv_buffer.hpp"
#include "../../gm_endianness.hpp"
#include "../../gm_buffer_memory.hpp"

#include <sstream>
#include <stdexcept>

namespace game {
    void BKV_State_Key::completeKey(BKV_Buffer& buf, const char c) {
        buf.tagHead_ = buf.head_;
        const size_t len = Endianness::hton(keyLen_);

        try {
            BufferMemory::checkResize(buf.bkv_, buf.head_ + 2 + keyLen_, buf.head_, buf.capacity_);
        } catch (std::exception &e) { throw e; }
        std::memcpy(buf.bkv_ + buf.head_ + 1, &len, sizeof(uint8_t));
        buf.head_ += 2; // Add 1 for tag (added later) and 1 for key length
        std::memcpy(buf.bkv_ + buf.head_, key_, keyLen_);
        buf.head_ += keyLen_;
        buf.valHead_ = buf.head_;
        buf.stateTree_.push(&buf.findTagState_);

        char key[256];
        std::memcpy(key, key_, keyLen_);
        key[keyLen_] = '\0';
        std::stringstream m;
        m << "Key state finished parsing: " << key;
        Logger::log(LOG_INFO, m.str());
        reset();
    }

    void BKV_State_Key::continueKey(BKV_Buffer& buf, const char c) {
        // Build string
        if (keyLen_ >= UINT8_MAX) {
            reset();
            std::stringstream msg;
            msg << "Too many characters in BKV key: " << keyLen_ << "/255 characters.";
            throw std::length_error(msg.str());
        }

        // Toggle break character after it breaks once
        if (breakChar_) {
            breakChar_ = false;

            char b = BKV_State_String::getBreakChar(c);
            if (b < 0) {
                reset();
                std::stringstream msg;
                msg << "Invalid break character in BKV key at index " << keyLen_ << ": 0x" << ((c & 0xf0) >> 4) << (c & 0xf);
                throw std::invalid_argument(msg.str());
            }
            
            key_[keyLen_] = b;
            keyLen_++;
        } else if (c == '\\') {
            breakChar_ = true;
        } else {
            key_[keyLen_] = c;
            keyLen_++;
        }
    }

    void BKV_State_Key::parse(BKV_Buffer& buf, const char c) {
        std::stringstream m;
        m << "Key state parsing character: " << c;
        Logger::log(LOG_INFO, m.str());
        if (!buf.head_) {
            // Must open with a compound
            if (c == '{') {
                buf.openCompound();
            } else {
                std::stringstream msg;
                msg << "Opening compound not found in BKV, first character was: 0x" << ((c & 0xf0) >> 4) << (c & 0xf);
                throw std::invalid_argument(msg.str());
            }
        } else if (strChar_ != DEFAULT_CHAR) { // Any UTF-8 string allowed
            if (c == strChar_ && !breakChar_) {
                try { completeKey(buf, c); } catch (std::exception &e) { throw e; }
            } else {
                try { continueKey(buf, c); } catch (std::exception &e) { throw e; }
            }
        } else if ((c == '}') && !keyLen_) {
            // Compound ended or is empty, and another one is ending. Ex: {ex1:{ex2:{id:1}},xe:5}
            try { buf.closeCompound(); } catch (std::exception &e) { throw e; }
        } else if (std::isspace(c) && !keyLen_) {
            // Whitespace, igore
            return;
        } else if (((c == '\'') || (c == '"')) && !keyLen_) {
            strChar_ = c;
        } else if (std::isalpha(c) || (keyLen_ && (std::isdigit(c) || c == '_' || c == '.' || c == '+' || c == '-'))) {
            try { continueKey(buf, c); } catch (std::exception &e) { throw e; }
        } else if (c == ':') {
            try { completeKey(buf, c); } catch (std::exception &e) { throw e; }
        } else if (!((c == ',') && !keyLen_)) {
            // If a compound hasn't just ended, this is an unexpected input
            reset();
            std::stringstream msg;
            msg << "Invalid character in BKV key at index " << keyLen_ << ": 0x" << ((c & 0xf0) >> 4) << (c & 0xf);
            throw std::invalid_argument(msg.str());
        } else return;
        // NOTE: In case a compound just ended and we are at the comma following it, just continue. Ex: {ex1:{ex2:{id:1}},xe:5}
    }
}
