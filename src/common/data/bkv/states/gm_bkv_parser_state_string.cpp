#include "gm_bkv_parser_state_string.hpp"

#include "../gm_bkv.hpp"
#include "../gm_bkv_parser.hpp"
#include "../../gm_endianness.hpp"
#include "../../../headers/string.hpp"

#include <stdexcept>

namespace game {
    char BKV_Parser_State_String::getEscapeChar(const char c) {
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
            case 'a': return '\a';
            case 'b': return '\b';
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
        if (_strLen >= BKV::BKV_STR_MAX) {
            UTF8Str msg = FormatString::formatString("Too many characters in SBKV string at index %ld: %ld/%ld characters.",
                parser._charactersRead, (_strLen + 1), BKV::BKV_STR_MAX);
            reset();
            throw std::runtime_error(msg.get());
        }

        try {
            StringBuffer::checkResize(_str, _strLen + 1, _strLen, _strCapacity);
        } catch (std::runtime_error &e) {
            reset();
            throw;
        }

        // Toggle break character after it breaks once
        if (_escapeChar) {
            _escapeChar = false;

            char b = BKV_Parser_State_String::getEscapeChar(c);
            if (b < 0) {
                UTF8Str msg = FormatString::formatString("Invalid break character in SBKV string at index %ld: %02x",
                    parser._charactersRead, c
                );
                reset();
                throw std::runtime_error(msg.get());
            }
            
            _str[_strLen] = c;
            _strLen++;
        } else if (c == '\\') {
            _escapeChar = true;
        } else {
            _str[_strLen] = c;
            _strLen++;
        }
    }

    void BKV_Parser_State_String::checkForBool(BKV_Parser& parser) {
        // Check if this is a boolean "true" or "false"
        if (FormatString::strToBool(_str, _strLen)) {
            parser._tag &= ~BKV::BKV_STR;
            parser._tag |= BKV::BKV_BOOL;
            try {
                StringBuffer::checkResize(parser._buffer.bkv_, parser._buffer._head + 1, parser._buffer._head, parser._buffer._capacity);
            } catch (std::runtime_error &e) {
                reset();
                throw;
            }

            // 0x01 for true
            parser._buffer.bkv_[parser._buffer._head] = 0x01;
            parser._buffer._head++;
        } else if ((_strLen == 5) && (
            ((_str[0] == 'f') || (_str[0] == 'F')) &&
            ((_str[1] == 'a') || (_str[1] == 'A')) &&
            ((_str[2] == 'l') || (_str[2] == 'L')) &&
            ((_str[3] == 's') || (_str[3] == 'S')) &&
            ((_str[4] == 'e') || (_str[4] == 'E'))
        )) {
            parser._tag &= ~BKV::BKV_STR;
            parser._tag |= BKV::BKV_BOOL;
            try {
                StringBuffer::checkResize(parser._buffer.bkv_, parser._buffer._head + 1, parser._buffer._head, parser._buffer._capacity);
            } catch (std::runtime_error &e) {
                reset();
                throw;
            }

            // 0x01 for false
            parser._buffer.bkv_[parser._buffer._head] = 0x00;
            parser._buffer._head++;
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
            const uint16_t len = Endianness::hton(static_cast<uint16_t>(_strLen));
            try {
                StringBuffer::checkResize(parser._buffer.bkv_, parser._buffer._head + BKV::BKV_STR_SIZE + _strLen, parser._buffer._head, parser._buffer._capacity);
            } catch (std::runtime_error &e) {
                reset();
                throw;
            }

            std::memcpy(parser._buffer.bkv_ + parser._buffer._head, &len, BKV::BKV_STR_SIZE);
            parser._buffer._head += BKV::BKV_STR_SIZE;
            std::memcpy(parser._buffer.bkv_ + parser._buffer._head, _str, _strLen);
            parser._buffer._head += _strLen;
        }

        reset();
        try { parser.endKV(c); } catch (std::runtime_error &e) { throw; }
    }
    
    void BKV_Parser_State_String::parse(BKV_Parser& parser, const char c) {
        parser._charactersRead++;
        
        if (!_strLen && ((c == '\'') || (c == '"'))) {
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
                if (!_strLen) {
                    UTF8Str msg = FormatString::formatString("Empty BKV string at index: %ld.");
                    reset();
                    throw std::runtime_error(msg.get());
                }
                completeStr(parser, c);
            } else if (std::isspace(c)) {
                // Whitespace, ignore
                return;
            } else {
                UTF8Str msg = FormatString::formatString("Invalid character in SBKV string at index %ld: %02x",
                    parser._charactersRead, c
                );
                reset();
                throw std::runtime_error(msg.get());
            }
        }
    }
}
