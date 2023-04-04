#include "gm_bkv_parser_state_key.hpp"

#include "gm_bkv_parser_state_string.hpp"
#include "../gm_bkv.hpp"
#include "../gm_bkv_parser.hpp"
#include "../../gm_endianness.hpp"
#include "../../../headers/string.hpp"

#include <stdexcept>

namespace game {
    void BKV_Parser_State_Key::completeKey(BKV_Parser& parser, const char c) {
        parser._buffer._tagHead = parser._buffer._head;
        uint8_t len = static_cast<uint8_t>(_keyLen);

        try {
            StringBuffer::checkResize(parser._buffer.bkv_, parser._buffer._head + 2 + _keyLen,
                parser._buffer._head, parser._buffer._capacity
            );
        } catch (std::runtime_error &e) { throw; }
        std::memcpy(parser._buffer.bkv_ + parser._buffer._head + 1, &len, BKV::BKV_KEY_SIZE);
        parser._buffer._head += 1 + BKV::BKV_KEY_SIZE; // Add 1 for tag (added later)
        std::memcpy(parser._buffer.bkv_ + parser._buffer._head, _key, _keyLen);
        parser._buffer._head += _keyLen;
        parser._buffer._valHead = parser._buffer._head;
        parser._stateTree.push(&parser._findTagState);

        reset();
    }

    void BKV_Parser_State_Key::continueKey(BKV_Parser& parser, const char c) {
        // Build string
        if (_keyLen >= static_cast<int16_t>(BKV::BKV_KEY_MAX)) {
            UTF8Str msg = FormatString::formatString("Too many characters in SBKV key at %ld: %ld/%ld characters.",
                parser._charactersRead, (_keyLen + 1), BKV::BKV_KEY_MAX
            );
            reset();
            throw std::runtime_error(msg.get());
        }

        // Toggle break character after it breaks once
        if (_escapeChar) {
            _escapeChar = false;

            char b = BKV_Parser_State_String::getEscapeChar(c);
            if (b < 0) {
                UTF8Str msg = FormatString::formatString("Invalid break character in SBKV key at %ld: %02x",
                    parser._charactersRead, c
                );
                reset();
                throw std::runtime_error(msg.get());
            }
            
            _key[_keyLen] = b;
            _keyLen++;
        } else if (c == '\\') {
            _escapeChar = true;
        } else {
            _key[_keyLen] = c;
            _keyLen++;
        }
    }

    void BKV_Parser_State_Key::parse(BKV_Parser& parser, const char c) {
        parser._charactersRead++;
        if (!parser._buffer._head) {
            // Must open with a compound
            if (c == '{') {
                parser.openCompound();
            } else {
                UTF8Str msg = FormatString::formatString("Opening compound not found in SBKV, first character was: %02x", c);
                throw std::runtime_error(msg.get());
            }
        } else if (_strChar) { // Any UTF-8 string allowed
            // NOTE: Checking to see if a UTF-8 character piece matches strChar is unnecessary because all UTF-8 characters
            // start with the first bit set, which is the signed bit. All ASCII characters have the signed bit cleared.
            if (c == _strChar && !_escapeChar) {
                try { completeKey(parser, c); } catch (std::runtime_error &e) { throw; }
            } else {
                try { continueKey(parser, c); } catch (std::runtime_error &e) { throw; }
            }
        } else if ((c == '}') && !_keyLen) {
            // Compound ended or is empty, and another one is ending. Ex: {ex1:{ex2:{id:1}},xe:5}
            try { parser.closeCompound(); } catch (std::runtime_error &e) { throw; }
        } else if ((std::isspace(c) || (c == ',')) && !_keyLen) {
            // Whitespace, igore OR
            // In case a compound just ended and we are at the comma following it, just continue. Ex: {ex1:{ex2:{id:1}},xe:5}
            return;
        } else if (((c == '\'') || (c == '"')) && !_keyLen) {
            _strChar = c;
        } else if (std::isalpha(c) || (_keyLen && (std::isdigit(c) || c == '_' || c == '.' || c == '+' || c == '-'))) {
            try { continueKey(parser, c); } catch (std::runtime_error &e) { throw; }
        } else if (c == ':') {
            try { completeKey(parser, c); } catch (std::runtime_error &e) { throw; }
        } else {
            // If a compound hasn't just ended, this is an unexpected input
            UTF8Str msg = FormatString::formatString("Invalid character in SBKV key at %ld: %02x", parser._charactersRead, c);
            reset();
            throw std::runtime_error(msg.get());
        }
    }
}
