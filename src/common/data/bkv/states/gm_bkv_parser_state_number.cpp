#include "gm_bkv_parser_state_number.hpp"

#include "../gm_bkv.hpp"
#include "../gm_bkv_parser.hpp"
#include "../../../headers/string.hpp"
#include "../../gm_endianness.hpp"

#include <cfloat>
#include <cmath>
#include <sstream>

namespace game {
    template <typename T>
    void BKV_Parser_State_Number::appendValue(BKV_Parser& parser, const T value) {
        const T val = Endianness::hton(value);
        std::stringstream m;
        m << "Number state parsed net value: " << static_cast<int64_t>(val) << " to be placed at " << parser._buffer._head << " with size " << sizeof(T);
        Logger::log(LOG_INFO, m.str());
        try {
            StringBuffer::checkResize(parser._buffer.bkv_, parser._buffer._head + (int64_t) sizeof(T), parser._buffer._head, parser._buffer._capacity);
        } catch (std::runtime_error &e) { throw; }
        std::memcpy(parser._buffer.bkv_ + parser._buffer._head, &val, sizeof(T));
        parser._buffer._head += sizeof(T);
    }

    template <typename T, typename TU>
    void BKV_Parser_State_Number::parseInt(BKV_Parser& parser, const int64_t min, const int64_t max, const uint64_t umax) {
        if (parser._tag & BKV::BKV_UNSIGNED) {
            uint64_t val = std::strtoull(_numBuffer, nullptr, 10);
        std::stringstream m;
        m << "Number state parsed unsigned int: " << val;
        Logger::log(LOG_INFO, m.str());
            // Make sure value doesn't overflow
            if (val > umax) {
                std::stringstream msg;
                msg << "BKV value overflows unsigned integer type at index " << parser._charactersRead << ": " << val << " > " << umax << ".";
                reset();
                throw std::runtime_error(msg.str());
            }
            try { appendValue(parser, static_cast<TU>(val)); } catch (std::runtime_error &e) { throw; }
        } else {
            int64_t val = std::strtoll(_numBuffer, nullptr, 10);
        std::stringstream m;
        m << "Number state parsed int: " << val;
        Logger::log(LOG_INFO, m.str());
            // Make sure value doesn't overflow or underflow
            if (val < min) {
                std::stringstream msg;
                msg << "BKV value underflows signed integer type at index " << parser._charactersRead << ": " << val << " < " << min << ".";
                reset();
                throw std::runtime_error(msg.str());
            } else if (val > max) {
                std::stringstream msg;
                msg << "BKV value overflows signed integer type at index " << parser._charactersRead << ": " << val << " > " << max << ".";
                reset();
                throw std::runtime_error(msg.str());
            }
            try { appendValue(parser, static_cast<T>(val)); } catch (std::runtime_error &e) { throw; }
        }
    }

    void BKV_Parser_State_Number::parseLong(BKV_Parser& parser) {
        parser._tag |= BKV::BKV_I64;
        if (parser._tag & BKV::BKV_UNSIGNED) {
            uint64_t val = std::strtoull(_numBuffer, nullptr, 10);
        std::stringstream m;
        m << "Number state parsed unsigned long: " << val;
        Logger::log(LOG_INFO, m.str());
            // Make sure value doesn't overflow
            if (val == UINT64_MAX) {
                std::stringstream msg;
                msg << "BKV value overflows unsigned long type at index " << parser._charactersRead << ": " << val << " >= " << UINT64_MAX << ".";
                reset();
                throw std::runtime_error(msg.str());
            }
            try { appendValue(parser, val); } catch (std::runtime_error &e) { throw; }
        } else {
            int64_t val = std::strtoll(_numBuffer, nullptr, 10);
        std::stringstream m;
        m << "Number state parsed long: " << val;
        Logger::log(LOG_INFO, m.str());
            // Make sure value doesn't overflow or underflow
            if (val == INT64_MIN) {
                std::stringstream msg;
                msg << "BKV value underflows signed long type at index " << parser._charactersRead << ": " << val << " <= " << INT64_MIN << ".";
                reset();
                throw std::runtime_error(msg.str());
            } else if (val == INT64_MAX) {
                std::stringstream msg;
                msg << "BKV value overflows signed long type at index " << parser._charactersRead << ": " << val << " >= " << INT64_MAX << ".";
                reset();
                throw std::runtime_error(msg.str());
            }
            try { appendValue(parser, val); } catch (std::runtime_error &e) { throw; }
        }
    }
    
    void BKV_Parser_State_Number::parseFloat(BKV_Parser& parser) {
        parser._tag |= BKV::BKV_FLOAT;
        double val = std::strtold(_numBuffer, nullptr);
        std::stringstream m;
        m << "Number state parsed float: " << val;
        Logger::log(LOG_INFO, m.str());
        // Make sure value doesn't overflow or underflow
        if ((val <= FLT_MIN) && (val >= -FLT_MIN)) {
            val = 0.0;
        } else if ((val <= -FLT_MAX) || (val == -HUGE_VALL)) {
            std::stringstream msg;
            msg << "BKV value underflows float at index " << parser._charactersRead << ": " << val << " <= " << FLT_MIN << ".";
            reset();
            throw std::runtime_error(msg.str());
        } else if ((val >= FLT_MAX) || (val == HUGE_VALL)) {
            std::stringstream msg;
            msg << "BKV value overflows float at index " << parser._charactersRead << ": " << val << " >= " << FLT_MAX << ".";
            reset();
            throw std::runtime_error(msg.str());
        }
        float v = Endianness::htonf(static_cast<float>(val));
        std::memcpy(parser._buffer.bkv_ + parser._buffer._head, &v, sizeof(float));
        parser._buffer._head += sizeof(float);
    }

    void BKV_Parser_State_Number::parseDouble(BKV_Parser& parser) {
        parser._tag |= BKV::BKV_DOUBLE;
        double val = std::strtold(_numBuffer, nullptr);
        std::stringstream m;
        m << "Number state parsed double at index " << parser._charactersRead << ": " << val;
        Logger::log(LOG_INFO, m.str());
        // Make sure value doesn't overflow or underflow
        if ((val <= DBL_MIN) && (val >= -DBL_MIN)) {
            val = 0.0;
        } else if ((val <= -DBL_MAX) || (val == -HUGE_VALL)) {
            std::stringstream msg;
            msg << "BKV value underflows double at index " << parser._charactersRead << ": " << val << " <= " << DBL_MIN << ".";
            reset();
            throw std::runtime_error(msg.str());
        } else if ((val >= DBL_MAX) || (val == HUGE_VALL)) {
            std::stringstream msg;
            msg << "BKV value overflows double at index " << parser._charactersRead << ": " << val << " >= " << DBL_MAX << ".";
            reset();
            throw std::runtime_error(msg.str());
        }
        double v = Endianness::htond(val);
        std::memcpy(parser._buffer.bkv_ + parser._buffer._head, &v, sizeof(double));
        parser._buffer._head += sizeof(double);
    }
    
    void BKV_Parser_State_Number::endNumber(BKV_Parser& parser, const char c) {
        std::stringstream m;
        m << "Number state ending number with character: " << c;
        Logger::log(LOG_INFO, m.str());
        reset();

        if ((c == '}') || (c == ',') || (c == ']')) {
            try { parser.endKV(c); } catch (std::runtime_error &e) { throw; }
        } else {
            std::stringstream msg;
            msg << "Invalid character in SBKV number at index: " << parser._charactersRead << ": 0x" << std::hex << ((c & 0xf0) >> 4) << std::hex << (c & 0xf);
            throw std::runtime_error(msg.str());
        }
    }

    void BKV_Parser_State_Number::parse(BKV_Parser& parser, const char c) {
        std::stringstream m;
        m << "Number state parsing character: " << c;
        Logger::log(LOG_INFO, m.str());
        parser._charactersRead++;
        
        if (std::isspace(c)) {
            // Whitespace, ignore
            return;
        } else if (parser._tag & ~BKV::BKV_FLAGS_ALL) {
            // Check if number has been completed and a tag is assigned
            try { endNumber(parser, c); } catch (std::runtime_error& e) { throw; }
        } else if (std::isdigit(c) || (c == '.' && !_hasDecimal) || (c == '-' && _bufferLen == 0)) {
            // Build number string
            if (c == '.') _hasDecimal = true;
            else if (c == '-') _hasNegative = true;

            if (_bufferLen >= UINT8_MAX) {
                std::stringstream msg;
                msg << "Too many digits in SBKV number at index " << parser._charactersRead << ": " << _bufferLen + 1 << "/" << UINT8_MAX << " digits.";
                reset();
                throw std::runtime_error(msg.str());
            }

            _numBuffer[_bufferLen] = c;
            _bufferLen++;
        } else {
            // Terminate number string and search for type
            _numBuffer[_bufferLen] = '\0';
            switch (c) {
                case ',':
                case ']':
                case '}': {
                    // Use default (integer if possible, long otherwise)
                    if (!_bufferLen) {
                        std::stringstream msg;
                        msg << "BKV number has no value at index: " << parser._charactersRead;
                        reset();
                        throw std::runtime_error(msg.str());
                    }
                    if (_hasDecimal) {
                        try { parseDouble(parser); } catch (std::runtime_error &e) { throw; }
                    } else {
                        if (parser._tag & BKV::BKV_UNSIGNED) {
                            uint64_t val = std::strtoull(_numBuffer, nullptr, 10);
                            // Make sure value doesn't overflow
                            if (val > UINT32_MAX) {
                                if (val == UINT64_MAX) {
                                    std::stringstream msg;
                                    msg << "BKV value overflows long at index " << parser._charactersRead << ": " << val << " >= " << UINT64_MAX << ".";
                                    reset();
                                    throw std::runtime_error(msg.str());
                                }
                                parser._tag |= BKV::BKV_I64;
                                try { appendValue(parser, val); } catch (std::runtime_error &e) { throw; }
                            } else {
                                parser._tag |= BKV::BKV_I32;
                                try { appendValue(parser, static_cast<uint32_t>(val)); } catch (std::runtime_error &e) { throw; }
                            }
                        } else {
                            int64_t val = std::strtoll(_numBuffer, nullptr, 10);
                            // Make sure value doesn't overflow or underflow
                            if ((val < INT32_MIN) || (val > INT32_MAX)) {
                                if (val == INT64_MIN) {
                                    std::stringstream msg;
                                    msg << "BKV value underflows long at index " << parser._charactersRead << ": " << val << " <= " << INT64_MIN << ".";
                                    reset();
                                    throw std::runtime_error(msg.str());
                                } else if (val == INT64_MAX) {
                                    std::stringstream msg;
                                    msg << "BKV value overflows long at index " << parser._charactersRead << ": " << val << " >= " << INT64_MAX << ".";
                                    reset();
                                    throw std::runtime_error(msg.str());
                                }
                                parser._tag |= BKV::BKV_I64;
                                try { appendValue(parser, val); } catch (std::runtime_error &e) { throw; }
                            } else {
                                parser._tag |= BKV::BKV_I32;
                                try { appendValue(parser, static_cast<int32_t>(val)); } catch (std::runtime_error &e) { throw; }
                            }
                        }
                    }
                    // Start again now that we have a number
                    try { endNumber(parser, c); } catch (std::runtime_error &e) { throw; }
                } break;
                case 'B':
                case 'b': {
                    parser._tag |= BKV::BKV_I8;
                    try { parseInt<int8_t, uint8_t>(parser, INT8_MIN, INT8_MAX, UINT8_MAX); } catch (std::runtime_error &e) { throw; }
                } break;
                case 'D':
                case 'd': {
                    try { parseDouble(parser); } catch (std::runtime_error &e) { throw; }
                } break;
                case 'F':
                case 'f': {
                    try { parseFloat(parser); } catch (std::runtime_error &e) { throw; }
                } break;
                case 'I':
                case 'i': {
                    parser._tag |= BKV::BKV_I32;
                    try { parseInt<int32_t, uint32_t>(parser, INT32_MIN, INT32_MAX, UINT32_MAX); } catch (std::runtime_error &e) { throw; }
                } break;
                case 'L':
                case 'l': {
                    try { parseLong(parser); } catch (std::runtime_error &e) { throw; }
                } break;
                case 'S':
                case 's': {
                    parser._tag |= BKV::BKV_I16;
                    try { parseInt<int16_t, uint16_t>(parser, INT16_MIN, INT16_MAX, UINT16_MAX); } catch (std::runtime_error &e) { throw; }
                } break;
                case 'U':
                case 'u': {
                    if (_hasNegative) {
                        std::stringstream msg;
                        msg << "Negative BKV number is supposed to be unsigned at index: " << parser._charactersRead;
                        reset();
                        throw std::runtime_error(msg.str());
                    }
                    parser._tag |= BKV::BKV_UNSIGNED;
                } break;
                default: {
                    std::stringstream msg;
                    msg << "Invalid character in SBKV number at index: " << parser._charactersRead << ": 0x" << std::hex << ((c & 0xf0) >> 4) << std::hex << (c & 0xf);
                    reset();
                    throw std::runtime_error(msg.str());
                }
            }
        }
    }
}
