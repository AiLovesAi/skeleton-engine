#include "gm_bkv_parser_state_number.hpp"

#include "../gm_bkv.hpp"
#include "../gm_bkv_parser.hpp"
#include "../../gm_buffer_memory.hpp"
#include "../../gm_endianness.hpp"

#include <cfloat>
#include <cmath>
#include <sstream>

namespace game {
    template <typename T>
    void BKV_Parser_State_Number::appendValue(BKV_Parser& parser, const T value) {
        const T val = Endianness::hton(value);
        std::stringstream m;
        m << "Number state parsed net value: " << static_cast<int64_t>(val) << " to be placed at " << parser.buffer_.head_ << " with size " << sizeof(T);
        Logger::log(LOG_INFO, m.str());
        try {
            BufferMemory::checkResize(parser.buffer_.bkv_, parser.buffer_.head_ + (int64_t) sizeof(T), parser.buffer_.head_, parser.buffer_.capacity_);
        } catch (std::runtime_error &e) { throw; }
        std::memcpy(parser.buffer_.bkv_ + parser.buffer_.head_, &val, sizeof(T));
        parser.buffer_.head_ += sizeof(T);
    }

    template <typename T, typename TU>
    void BKV_Parser_State_Number::parseInt(BKV_Parser& parser, const int64_t min, const int64_t max, const uint64_t umax) {
        if (parser.tag_ & BKV::BKV_UNSIGNED) {
            uint64_t val = std::strtoull(numBuf_, nullptr, 10);
        std::stringstream m;
        m << "Number state parsed unsigned int: " << val;
        Logger::log(LOG_INFO, m.str());
            // Make sure value doesn't overflow
            if (val > umax) {
                std::stringstream msg;
                msg << "BKV value overflows unsigned integer type at index " << parser.charactersRead_ << ": " << val << " > " << umax << ".";
                reset();
                throw std::runtime_error(msg.str());
            }
            try { appendValue(parser, static_cast<TU>(val)); } catch (std::runtime_error &e) { throw; }
        } else {
            int64_t val = std::strtoll(numBuf_, nullptr, 10);
        std::stringstream m;
        m << "Number state parsed int: " << val;
        Logger::log(LOG_INFO, m.str());
            // Make sure value doesn't overflow or underflow
            if (val < min) {
                std::stringstream msg;
                msg << "BKV value underflows signed integer type at index " << parser.charactersRead_ << ": " << val << " < " << min << ".";
                reset();
                throw std::runtime_error(msg.str());
            } else if (val > max) {
                std::stringstream msg;
                msg << "BKV value overflows signed integer type at index " << parser.charactersRead_ << ": " << val << " > " << max << ".";
                reset();
                throw std::runtime_error(msg.str());
            }
            try { appendValue(parser, static_cast<T>(val)); } catch (std::runtime_error &e) { throw; }
        }
    }

    void BKV_Parser_State_Number::parseLong(BKV_Parser& parser) {
        parser.tag_ |= BKV::BKV_I64;
        if (parser.tag_ & BKV::BKV_UNSIGNED) {
            uint64_t val = std::strtoull(numBuf_, nullptr, 10);
        std::stringstream m;
        m << "Number state parsed unsigned long: " << val;
        Logger::log(LOG_INFO, m.str());
            // Make sure value doesn't overflow
            if (val == UINT64_MAX) {
                std::stringstream msg;
                msg << "BKV value overflows unsigned long type at index " << parser.charactersRead_ << ": " << val << " >= " << UINT64_MAX << ".";
                reset();
                throw std::runtime_error(msg.str());
            }
            try { appendValue(parser, val); } catch (std::runtime_error &e) { throw; }
        } else {
            int64_t val = std::strtoll(numBuf_, nullptr, 10);
        std::stringstream m;
        m << "Number state parsed long: " << val;
        Logger::log(LOG_INFO, m.str());
            // Make sure value doesn't overflow or underflow
            if (val == INT64_MIN) {
                std::stringstream msg;
                msg << "BKV value underflows signed long type at index " << parser.charactersRead_ << ": " << val << " <= " << INT64_MIN << ".";
                reset();
                throw std::runtime_error(msg.str());
            } else if (val == INT64_MAX) {
                std::stringstream msg;
                msg << "BKV value overflows signed long type at index " << parser.charactersRead_ << ": " << val << " >= " << INT64_MAX << ".";
                reset();
                throw std::runtime_error(msg.str());
            }
            try { appendValue(parser, val); } catch (std::runtime_error &e) { throw; }
        }
    }
    
    void BKV_Parser_State_Number::parseFloat(BKV_Parser& parser) {
        parser.tag_ |= BKV::BKV_FLOAT;
        double val = std::strtold(numBuf_, nullptr);
        std::stringstream m;
        m << "Number state parsed float: " << val;
        Logger::log(LOG_INFO, m.str());
        // Make sure value doesn't overflow or underflow
        if ((val <= FLT_MIN) && (val >= -FLT_MIN)) {
            val = 0.0;
        } else if ((val <= -FLT_MAX) || (val == -HUGE_VALL)) {
            std::stringstream msg;
            msg << "BKV value underflows float at index " << parser.charactersRead_ << ": " << val << " <= " << FLT_MIN << ".";
            reset();
            throw std::runtime_error(msg.str());
        } else if ((val >= FLT_MAX) || (val == HUGE_VALL)) {
            std::stringstream msg;
            msg << "BKV value overflows float at index " << parser.charactersRead_ << ": " << val << " >= " << FLT_MAX << ".";
            reset();
            throw std::runtime_error(msg.str());
        }
        float v = Endianness::htonf(static_cast<float>(val));
        std::memcpy(parser.buffer_.bkv_ + parser.buffer_.head_, &v, sizeof(float));
        parser.buffer_.head_ += sizeof(float);
    }

    void BKV_Parser_State_Number::parseDouble(BKV_Parser& parser) {
        parser.tag_ |= BKV::BKV_DOUBLE;
        double val = std::strtold(numBuf_, nullptr);
        std::stringstream m;
        m << "Number state parsed double at index " << parser.charactersRead_ << ": " << val;
        Logger::log(LOG_INFO, m.str());
        // Make sure value doesn't overflow or underflow
        if ((val <= DBL_MIN) && (val >= -DBL_MIN)) {
            val = 0.0;
        } else if ((val <= -DBL_MAX) || (val == -HUGE_VALL)) {
            std::stringstream msg;
            msg << "BKV value underflows double at index " << parser.charactersRead_ << ": " << val << " <= " << DBL_MIN << ".";
            reset();
            throw std::runtime_error(msg.str());
        } else if ((val >= DBL_MAX) || (val == HUGE_VALL)) {
            std::stringstream msg;
            msg << "BKV value overflows double at index " << parser.charactersRead_ << ": " << val << " >= " << DBL_MAX << ".";
            reset();
            throw std::runtime_error(msg.str());
        }
        double v = Endianness::htond(val);
        std::memcpy(parser.buffer_.bkv_ + parser.buffer_.head_, &v, sizeof(double));
        parser.buffer_.head_ += sizeof(double);
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
            msg << "Invalid character in SBKV number at index: " << parser.charactersRead_ << ": 0x" << std::hex << ((c & 0xf0) >> 4) << std::hex << (c & 0xf);
            throw std::runtime_error(msg.str());
        }
    }

    void BKV_Parser_State_Number::parse(BKV_Parser& parser, const char c) {
        std::stringstream m;
        m << "Number state parsing character: " << c;
        Logger::log(LOG_INFO, m.str());
        parser.charactersRead_++;
        
        if (std::isspace(c)) {
            // Whitespace, ignore
            return;
        } else if (parser.tag_ & ~BKV::BKV_FLAGS_ALL) {
            // Check if number has been completed and a tag is assigned
            try { endNumber(parser, c); } catch (std::runtime_error& e) { throw; }
        } else if (std::isdigit(c) || (c == '.' && !hasDecimal_) || (c == '-' && bufLen_ == 0)) {
            // Build number string
            if (c == '.') hasDecimal_ = true;
            else if (c == '-') hasNegative_ = true;

            if (bufLen_ >= UINT8_MAX) {
                std::stringstream msg;
                msg << "Too many digits in SBKV number at index " << parser.charactersRead_ << ": " << bufLen_ + 1 << "/" << UINT8_MAX << " digits.";
                reset();
                throw std::runtime_error(msg.str());
            }

            numBuf_[bufLen_] = c;
            bufLen_++;
        } else {
            // Terminate number string and search for type
            numBuf_[bufLen_] = '\0';
            switch (c) {
                case ',':
                case ']':
                case '}': {
                    // Use default (integer if possible, long otherwise)
                    if (!bufLen_) {
                        std::stringstream msg;
                        msg << "BKV number has no value at index: " << parser.charactersRead_;
                        reset();
                        throw std::runtime_error(msg.str());
                    }
                    if (hasDecimal_) {
                        try { parseDouble(parser); } catch (std::runtime_error &e) { throw; }
                    } else {
                        if (parser.tag_ & BKV::BKV_UNSIGNED) {
                            uint64_t val = std::strtoull(numBuf_, nullptr, 10);
                            // Make sure value doesn't overflow
                            if (val > UINT32_MAX) {
                                if (val == UINT64_MAX) {
                                    std::stringstream msg;
                                    msg << "BKV value overflows long at index " << parser.charactersRead_ << ": " << val << " >= " << UINT64_MAX << ".";
                                    reset();
                                    throw std::runtime_error(msg.str());
                                }
                                parser.tag_ |= BKV::BKV_I64;
                                try { appendValue(parser, val); } catch (std::runtime_error &e) { throw; }
                            } else {
                                parser.tag_ |= BKV::BKV_I32;
                                try { appendValue(parser, static_cast<uint32_t>(val)); } catch (std::runtime_error &e) { throw; }
                            }
                        } else {
                            int64_t val = std::strtoll(numBuf_, nullptr, 10);
                            // Make sure value doesn't overflow or underflow
                            if ((val < INT32_MIN) || (val > INT32_MAX)) {
                                if (val == INT64_MIN) {
                                    std::stringstream msg;
                                    msg << "BKV value underflows long at index " << parser.charactersRead_ << ": " << val << " <= " << INT64_MIN << ".";
                                    reset();
                                    throw std::runtime_error(msg.str());
                                } else if (val == INT64_MAX) {
                                    std::stringstream msg;
                                    msg << "BKV value overflows long at index " << parser.charactersRead_ << ": " << val << " >= " << INT64_MAX << ".";
                                    reset();
                                    throw std::runtime_error(msg.str());
                                }
                                parser.tag_ |= BKV::BKV_I64;
                                try { appendValue(parser, val); } catch (std::runtime_error &e) { throw; }
                            } else {
                                parser.tag_ |= BKV::BKV_I32;
                                try { appendValue(parser, static_cast<int32_t>(val)); } catch (std::runtime_error &e) { throw; }
                            }
                        }
                    }
                    // Start again now that we have a number
                    try { endNumber(parser, c); } catch (std::runtime_error &e) { throw; }
                } break;
                case 'B':
                case 'b': {
                    parser.tag_ |= BKV::BKV_I8;
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
                    parser.tag_ |= BKV::BKV_I32;
                    try { parseInt<int32_t, uint32_t>(parser, INT32_MIN, INT32_MAX, UINT32_MAX); } catch (std::runtime_error &e) { throw; }
                } break;
                case 'L':
                case 'l': {
                    try { parseLong(parser); } catch (std::runtime_error &e) { throw; }
                } break;
                case 'S':
                case 's': {
                    parser.tag_ |= BKV::BKV_I16;
                    try { parseInt<int16_t, uint16_t>(parser, INT16_MIN, INT16_MAX, UINT16_MAX); } catch (std::runtime_error &e) { throw; }
                } break;
                case 'U':
                case 'u': {
                    if (hasNegative_) {
                        std::stringstream msg;
                        msg << "Negative BKV number is supposed to be unsigned at index: " << parser.charactersRead_;
                        reset();
                        throw std::runtime_error(msg.str());
                    }
                    parser.tag_ |= BKV::BKV_UNSIGNED;
                } break;
                default: {
                    std::stringstream msg;
                    msg << "Invalid character in SBKV number at index: " << parser.charactersRead_ << ": 0x" << std::hex << ((c & 0xf0) >> 4) << std::hex << (c & 0xf);
                    reset();
                    throw std::runtime_error(msg.str());
                }
            }
        }
    }
}