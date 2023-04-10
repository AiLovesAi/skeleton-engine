#include "gm_bkv_parser_state_string.hpp"

#include "../gm_bkv.hpp"
#include "../gm_bkv_parser.hpp"
#include "../../gm_endianness.hpp"

#include <stdexcept>

namespace game {
    void BKV_Parser_State_String::continueStr(BKV_Parser& parser, const char c) {
        // Build string
        if (_str.len() >= BKV::BKV_STR_MAX) {
            reset();
            throw std::runtime_error(FormatString::formatString(
                "Too many characters in SBKV string at index %ld: %ld/%u characters.",
                parser._charactersRead, (_str.len() + 1), BKV::BKV_STR_MAX
            ).get());
        }

        // Toggle break character after it breaks once
        if (_escapeChar) {
            _escapeChar = false;

            char b = String::escapeChar(c);
            if (b < 0) {
                reset();
                throw std::runtime_error(FormatString::formatString(
                    "Invalid break character in SBKV string at index %ld: %02x",
                    parser._charactersRead, c
                ).get());
            }
            
            try { _str.append(b); } catch (std::runtime_error& e) {
                reset();
                throw;
            }
        } else if (c == '\\') {
            _escapeChar = true;
        } else {
            try { _str.append(c); } catch (std::runtime_error& e) {
                reset();
                throw;
            }
        }
    }

    void BKV_Parser_State_String::checkForBool(BKV_Parser& parser) {
        // Check if this is a boolean "true" or "false"
        if (FormatString::strToBool(_str.get(), _str.len())) {
            parser._tag &= ~BKV::BKV_STR;
            parser._tag |= BKV::BKV_BOOL;

            try {
                StringBuffer::checkResize(parser._buffer._bkv, parser._buffer._head + 1, parser._buffer._head, parser._buffer._capacity);
            } catch (std::runtime_error &e) {
                reset();
                throw;
            }

            // 0x01 for true
            parser._buffer._bkv[parser._buffer._head++] = 0x01;
        } else if ((_str.len() == 5) && (
            (_str.cmp('f', 0) || _str.cmp('F', 0)) &&
            (_str.cmp('a', 1) || _str.cmp('A', 1)) &&
            (_str.cmp('l', 2) || _str.cmp('L', 2)) &&
            (_str.cmp('s', 3) || _str.cmp('S', 3)) &&
            (_str.cmp('e', 4) || _str.cmp('E', 4))
        )) {
            parser._tag &= ~BKV::BKV_STR;
            parser._tag |= BKV::BKV_BOOL;
            try {
                StringBuffer::checkResize(parser._buffer._bkv, parser._buffer._head + 1,
                    parser._buffer._head, parser._buffer._capacity
                );
            } catch (std::runtime_error &e) {
                reset();
                throw;
            }

            // 0x00 for false
            parser._buffer._bkv[parser._buffer._head++] = 0x00;
        }
    }

    void BKV_Parser_State_String::completeStr(BKV_Parser& parser, const char c) {
        if (!_strChar) {
            // Only check for bool if it is not a string. "True" is a string and True is a boolean.
            try { checkForBool(parser); } catch (std::runtime_error &e) { throw; }
        }

        if (!(parser._tag & BKV::BKV_BOOL)) {
            // Must be string, copy to buffer
            parser._tag |= BKV::BKV_STR;
            const uint16_t len = Endianness::hton(static_cast<uint16_t>(_str.len()));
            try {
                StringBuffer::checkResize(parser._buffer._bkv,
                    parser._buffer._head + BKV::BKV_STR_SIZE + static_cast<int64_t>(_str.len()),
                    parser._buffer._head,
                    parser._buffer._capacity
                );
            } catch (std::runtime_error &e) {
                reset();
                throw;
            }

            std::memcpy(parser._buffer._bkv + parser._buffer._head, &len, BKV::BKV_STR_SIZE);
            parser._buffer._head += BKV::BKV_STR_SIZE;
            std::memcpy(parser._buffer._bkv + parser._buffer._head, _str.get(), _str.len());
            parser._buffer._head += _str.len();
        }

        reset();
        try { parser.endKV(c); } catch (std::runtime_error &e) { throw; }
    }
    
    void BKV_Parser_State_String::parse(BKV_Parser& parser, const char c) {
        parser._charactersRead++;
        
        if (!_str.len() && ((c == '\'') || (c == '"'))) {
            _strChar = c;
        } else if (_strChar > 0) { // Any UTF-8 string allowed
            // NOTE: Checking to see if a UTF-8 character piece matches strChar is unnecessary because all UTF-8 characters
            // start with the first bit set, which is the signed bit. All ASCII characters have the signed bit cleared.
            if (c == _strChar && !_escapeChar) {
                _strChar = -1; // Wait for the following KV end character to call complete string
            } else {
                continueStr(parser, c);
            }
        } else { // Only alphanumeric ASCII allowed
            if (!_strChar && (std::isalnum(c) || std::isdigit(c) || c == '_' || c == '.' || c == '+' || c == '-')) {
                continueStr(parser, c);
            } else if ((c == '}') || (c == ',') || (c == ']')) {
                if (!_str.len()) {
                    reset();
                    throw std::runtime_error(FormatString::formatString(
                        "Empty BKV string at index: %ld.", parser._charactersRead
                    ).get());
                }
                completeStr(parser, c);
            } else if (std::isspace(c)) {
                // Whitespace, ignore
                return;
            } else {
                reset();
                throw std::runtime_error(FormatString::formatString(
                    "Invalid character in SBKV string at index %ld: %02x",
                    parser._charactersRead, c
                ).get());
            }
        }
    }
}
