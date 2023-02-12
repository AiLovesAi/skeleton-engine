#include "gm_bkv_parser_state_key.hpp"

#include "gm_bkv_parser_state_string.hpp"
#include "../gm_bkv.hpp"
#include "../gm_bkv_parser.hpp"
#include "../../gm_endianness.hpp"
#include "../../string/gm_string.hpp"

#include <sstream>
#include <stdexcept>

namespace game {
    void BKV_Parser_State_Key::completeKey(BKV_Parser& parser, const char c) {
        parser.buffer_.tagHead_ = parser.buffer_.head_;
        uint8_t len = static_cast<uint8_t>(keyLen_);

        try {
            String::checkResize(parser.buffer_.bkv_, parser.buffer_.head_ + 2 + keyLen_, parser.buffer_.head_, parser.buffer_.capacity_);
        } catch (std::runtime_error &e) { throw; }
        std::memcpy(parser.buffer_.bkv_ + parser.buffer_.head_ + 1, &len, BKV::BKV_KEY_SIZE);
        parser.buffer_.head_ += 1 + BKV::BKV_KEY_SIZE; // Add 1 for tag (added later)
    std::stringstream m;
    m << "Key putting data at: " << parser.buffer_.head_;
    Logger::log(LOG_INFO, m.str());
        std::memcpy(parser.buffer_.bkv_ + parser.buffer_.head_, key_, keyLen_);
        parser.buffer_.head_ += keyLen_;
        parser.buffer_.valHead_ = parser.buffer_.head_;
        parser.stateTree_.push(&parser.findTagState_);

    char key[256];
    std::memcpy(key, key_, keyLen_);
    key[keyLen_] = '\0';
    m.str("");
    m << "Key state finished parsing: " << key;
    Logger::log(LOG_INFO, m.str());
        reset();
    }

    void BKV_Parser_State_Key::continueKey(BKV_Parser& parser, const char c) {
        // Build string
        if (keyLen_ >= static_cast<int16_t>(BKV::BKV_KEY_MAX)) {
            std::stringstream msg;
            msg << "Too many characters in SBKV key at index " << parser.charactersRead_ << ": " << keyLen_ + 1 << "/" << BKV::BKV_KEY_MAX << " characters.";
            reset();
            throw std::runtime_error(msg.str());
        }

        // Toggle break character after it breaks once
        if (escapeChar_) {
            escapeChar_ = false;

            char b = BKV_Parser_State_String::getBreakChar(c);
            if (b < 0) {
                std::stringstream msg;
                msg << "Invalid break character in SBKV key at index " << parser.charactersRead_ << ": 0x" << std::hex << ((c & 0xf0) >> 4) << std::hex << (c & 0xf);
                reset();
                throw std::runtime_error(msg.str());
            }
            
            key_[keyLen_] = b;
            keyLen_++;
        } else if (c == '\\') {
            escapeChar_ = true;
        } else {
            key_[keyLen_] = c;
            keyLen_++;
        }
    }

    void BKV_Parser_State_Key::parse(BKV_Parser& parser, const char c) {
        parser.charactersRead_++;
        std::stringstream m;
        m << "Key state parsing character: " << c;
        Logger::log(LOG_INFO, m.str());
        if (!parser.buffer_.head_) {
            // Must open with a compound
            if (c == '{') {
                parser.openCompound();
            } else {
                std::stringstream msg;
                msg << "Opening compound not found in SBKV, first character was: 0x" << std::hex << ((c & 0xf0) >> 4) << std::hex << (c & 0xf);
                throw std::runtime_error(msg.str());
            }
        } else if (strChar_) { // Any UTF-8 string allowed
            // NOTE: Checking to see if a UTF-8 character piece matches strChar is unnecessary because all UTF-8 characters
            // start with the first bit set, which is the signed bit. All ASCII characters have the signed bit cleared.
            if (c == strChar_ && !escapeChar_) {
                try { completeKey(parser, c); } catch (std::runtime_error &e) { throw; }
            } else {
                try { continueKey(parser, c); } catch (std::runtime_error &e) { throw; }
            }
        } else if ((c == '}') && !keyLen_) {
            // Compound ended or is empty, and another one is ending. Ex: {ex1:{ex2:{id:1}},xe:5}
            try { parser.closeCompound(); } catch (std::runtime_error &e) { throw; }
        } else if ((std::isspace(c) || (c == ',')) && !keyLen_) {
            // Whitespace, igore OR
            // In case a compound just ended and we are at the comma following it, just continue. Ex: {ex1:{ex2:{id:1}},xe:5}
            return;
        } else if (((c == '\'') || (c == '"')) && !keyLen_) {
            strChar_ = c;
        } else if (std::isalpha(c) || (keyLen_ && (std::isdigit(c) || c == '_' || c == '.' || c == '+' || c == '-'))) {
            try { continueKey(parser, c); } catch (std::runtime_error &e) { throw; }
        } else if (c == ':') {
            try { completeKey(parser, c); } catch (std::runtime_error &e) { throw; }
        } else {
            // If a compound hasn't just ended, this is an unexpected input
            std::stringstream msg;
            msg << "Invalid character in SBKV key at index: " << parser.charactersRead_ << ": 0x" << std::hex << ((c & 0xf0) >> 4) << std::hex << (c & 0xf);
            reset();
            throw std::runtime_error(msg.str());
        }
    }
}
