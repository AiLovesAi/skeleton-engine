#include "gm_bkv_state_number.hpp"

#include "../gm_bkv.hpp"
#include "../gm_bkv_buffer.hpp"
#include "../../gm_buffer_memory.hpp"
#include "../../gm_endianness.hpp"

#include <cfloat>
#include <cmath>
#include <sstream>

namespace game {
    template <typename T>
    void BKV_State_Number::appendValue(BKV_Buffer& buf, const T value) {
        const T val = Endianness::hton(value);
        try {
            BufferMemory::checkResize(buf.bkv_, buf.head_ + (int64_t) sizeof(T), buf.head_, buf.capacity_);
        } catch (std::runtime_error &e) { throw; }
        std::memcpy(buf.bkv_ + buf.head_, &val, sizeof(T));
        buf.head_ += sizeof(T);
    }

    template <typename T, typename TU>
    void BKV_State_Number::parseInt(BKV_Buffer& buf, const int64_t min, const int64_t max, const uint64_t umax) {
        if (buf.tag_ & BKV::BKV_UNSIGNED) {
            uint64_t val = std::strtoull(numBuf_, nullptr, 10);

            // Make sure value doesn't overflow
            if (val > umax) {
                std::stringstream msg;
                msg << "BKV value overflows unsigned integer type at index " << buf.charactersRead_ << ": " << val << " > " << umax << ".";
                reset();
                throw std::runtime_error(msg.str());
            }
            try { appendValue(buf, static_cast<TU>(val)); } catch (std::runtime_error &e) { throw; }
        } else {
            int64_t val = std::strtoll(numBuf_, nullptr, 10);

            // Make sure value doesn't overflow or underflow
            if (val < min) {
                std::stringstream msg;
                msg << "BKV value underflows signed integer type at index " << buf.charactersRead_ << ": " << val << " < " << min << ".";
                reset();
                throw std::runtime_error(msg.str());
            } else if (val > max) {
                std::stringstream msg;
                msg << "BKV value overflows signed integer type at index " << buf.charactersRead_ << ": " << val << " > " << max << ".";
                reset();
                throw std::runtime_error(msg.str());
            }
            try { appendValue(buf, static_cast<T>(val)); } catch (std::runtime_error &e) { throw; }
        }
    }

    void BKV_State_Number::parseLong(BKV_Buffer& buf) {
        buf.tag_ |= BKV::BKV_I64;

        if (buf.tag_ & BKV::BKV_UNSIGNED) {
            uint64_t val = std::strtoull(numBuf_, nullptr, 10);
            // Make sure value doesn't overflow
            if (val == UINT64_MAX) {
                std::stringstream msg;
                msg << "BKV value overflows unsigned long type at index " << buf.charactersRead_ << ": " << val << " >= " << UINT64_MAX << ".";
                reset();
                throw std::runtime_error(msg.str());
            }
            try { appendValue(buf, val); } catch (std::runtime_error &e) { throw; }
        } else {
            int64_t val = std::strtoll(numBuf_, nullptr, 10);
            // Make sure value doesn't overflow or underflow
            if (val == INT64_MIN) {
                std::stringstream msg;
                msg << "BKV value underflows signed long type at index " << buf.charactersRead_ << ": " << val << " <= " << INT64_MIN << ".";
                reset();
                throw std::runtime_error(msg.str());
            } else if (val == INT64_MAX) {
                std::stringstream msg;
                msg << "BKV value overflows signed long type at index " << buf.charactersRead_ << ": " << val << " >= " << INT64_MAX << ".";
                reset();
                throw std::runtime_error(msg.str());
            }
            try { appendValue(buf, val); } catch (std::runtime_error &e) { throw; }
        }
    }
    
    void BKV_State_Number::parseFloat(BKV_Buffer& buf) {
        buf.tag_ |= BKV::BKV_FLOAT;
        double val = std::strtold(numBuf_, nullptr);

        // Make sure value doesn't overflow or underflow
        if ((val <= FLT_MIN) && (val >= -FLT_MIN)) {
            val = 0.0;
        } else if ((val <= -FLT_MAX) || (val == -HUGE_VALL)) {
            std::stringstream msg;
            msg << "BKV value underflows float at index " << buf.charactersRead_ << ": " << val << " <= " << FLT_MIN << ".";
            reset();
            throw std::runtime_error(msg.str());
        } else if ((val >= FLT_MAX) || (val == HUGE_VALL)) {
            std::stringstream msg;
            msg << "BKV value overflows float at index " << buf.charactersRead_ << ": " << val << " >= " << FLT_MAX << ".";
            reset();
            throw std::runtime_error(msg.str());
        }
        float v = Endianness::htonf(static_cast<float>(val));
        std::memcpy(buf.bkv_ + buf.head_, &v, sizeof(float));
        buf.head_ += sizeof(float);
    }

    void BKV_State_Number::parseDouble(BKV_Buffer& buf) {
        buf.tag_ |= BKV::BKV_DOUBLE;
        double val = std::strtold(numBuf_, nullptr);
        
        // Make sure value doesn't overflow or underflow
        if ((val <= DBL_MIN) && (val >= -DBL_MIN)) {
            val = 0.0;
        } else if ((val <= -DBL_MAX) || (val == -HUGE_VALL)) {
            std::stringstream msg;
            msg << "BKV value underflows double at index " << buf.charactersRead_ << ": " << val << " <= " << DBL_MIN << ".";
            reset();
            throw std::runtime_error(msg.str());
        } else if ((val >= DBL_MAX) || (val == HUGE_VALL)) {
            std::stringstream msg;
            msg << "BKV value overflows double at index " << buf.charactersRead_ << ": " << val << " >= " << DBL_MAX << ".";
            reset();
            throw std::runtime_error(msg.str());
        }
        double v = Endianness::htond(val);
        std::memcpy(buf.bkv_ + buf.head_, &v, sizeof(double));
        buf.head_ += sizeof(double);
    }
    
    void BKV_State_Number::endNumber(BKV_Buffer& buf, const char c) {
        reset();
        
        if ((c == '}') || (c == ',') || (c == ']')) {
            try { buf.endKV(c); } catch (std::runtime_error &e) { throw; }
        } else {
            std::stringstream msg;
            msg << "Invalid character in BKV number at index: " << buf.charactersRead_ << ": 0x" << std::hex << ((c & 0xf0) >> 4) << std::hex << (c & 0xf);
            throw std::runtime_error(msg.str());
        }
    }

    void BKV_State_Number::parse(BKV_Buffer& buf, const char c) {
        buf.charactersRead_++;
        
        if (std::isspace(c)) {
            // Whitespace, ignore
            return;
        } else if (buf.tag_ & ~BKV::BKV_FLAGS_ALL) {
            // Check if number has been completed and a tag is assigned
            try { endNumber(buf, c); } catch (std::runtime_error& e) { throw; }
        } else if (std::isdigit(c) || (c == '.' && !hasDecimal_) || (c == '-' && bufLen_ == 0)) {
            // Build number string
            if (c == '.') hasDecimal_ = true;
            else if (c == '-') hasNegative_ = true;

            if (bufLen_ >= UINT8_MAX) {
                std::stringstream msg;
                msg << "Too many digits in BKV number at index " << buf.charactersRead_ << ": " << bufLen_ + 1 << "/" << UINT8_MAX << " digits.";
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
                        msg << "BKV number has no value at index: " << buf.charactersRead_;
                        reset();
                        throw std::runtime_error(msg.str());
                    }
                    if (hasDecimal_) {
                        try { parseDouble(buf); } catch (std::runtime_error &e) { throw; }
                    } else {
                        if (buf.tag_ & BKV::BKV_UNSIGNED) {
                            uint64_t val = std::strtoull(numBuf_, nullptr, 10);
                            // Make sure value doesn't overflow
                            if (val > UINT32_MAX) {
                                if (val == UINT64_MAX) {
                                    std::stringstream msg;
                                    msg << "BKV value overflows long at index " << buf.charactersRead_ << ": " << val << " >= " << UINT64_MAX << ".";
                                    reset();
                                    throw std::runtime_error(msg.str());
                                }
                                buf.tag_ |= BKV::BKV_I64;
                                try { appendValue(buf, val); } catch (std::runtime_error &e) { throw; }
                            } else {
                                buf.tag_ |= BKV::BKV_I32;
                                try { appendValue(buf, static_cast<uint32_t>(val)); } catch (std::runtime_error &e) { throw; }
                            }
                        } else {
                            int64_t val = std::strtoll(numBuf_, nullptr, 10);
                            // Make sure value doesn't overflow or underflow
                            if ((val < INT32_MIN) || (val > INT32_MAX)) {
                                if (val == INT64_MIN) {
                                    std::stringstream msg;
                                    msg << "BKV value underflows long at index " << buf.charactersRead_ << ": " << val << " <= " << INT64_MIN << ".";
                                    reset();
                                    throw std::runtime_error(msg.str());
                                } else if (val == INT64_MAX) {
                                    std::stringstream msg;
                                    msg << "BKV value overflows long at index " << buf.charactersRead_ << ": " << val << " >= " << INT64_MAX << ".";
                                    reset();
                                    throw std::runtime_error(msg.str());
                                }
                                buf.tag_ |= BKV::BKV_I64;
                                try { appendValue(buf, val); } catch (std::runtime_error &e) { throw; }
                            } else {
                                buf.tag_ |= BKV::BKV_I32;
                                try { appendValue(buf, static_cast<int32_t>(val)); } catch (std::runtime_error &e) { throw; }
                            }
                        }
                    }
                    // Start again now that we have a number
                    try { endNumber(buf, c); } catch (std::runtime_error &e) { throw; }
                } break;
                case 'B':
                case 'b': {
                    buf.tag_ |= BKV::BKV_I8;
                    try { parseInt<int8_t, uint8_t>(buf, INT8_MIN, INT8_MAX, UINT8_MAX); } catch (std::runtime_error &e) { throw; }
                } break;
                case 'D':
                case 'd': {
                    try { parseDouble(buf); } catch (std::runtime_error &e) { throw; }
                } break;
                case 'F':
                case 'f': {
                    try { parseFloat(buf); } catch (std::runtime_error &e) { throw; }
                } break;
                case 'I':
                case 'i': {
                    buf.tag_ |= BKV::BKV_I32;
                    try { parseInt<int32_t, uint32_t>(buf, INT32_MIN, INT32_MAX, UINT32_MAX); } catch (std::runtime_error &e) { throw; }
                } break;
                case 'L':
                case 'l': {
                    try { parseLong(buf); } catch (std::runtime_error &e) { throw; }
                } break;
                case 'S':
                case 's': {
                    buf.tag_ |= BKV::BKV_I16;
                    try { parseInt<int16_t, uint16_t>(buf, INT16_MIN, INT16_MAX, UINT16_MAX); } catch (std::runtime_error &e) { throw; }
                } break;
                case 'U':
                case 'u': {
                    if (hasNegative_) {
                        std::stringstream msg;
                        msg << "Negative BKV number is supposed to be unsigned at index: " << buf.charactersRead_;
                        reset();
                        throw std::runtime_error(msg.str());
                    }
                    buf.tag_ |= BKV::BKV_UNSIGNED;
                } break;
                default: {
                    std::stringstream msg;
                    msg << "Invalid character in BKV number at index: " << buf.charactersRead_ << ": 0x" << std::hex << ((c & 0xf0) >> 4) << std::hex << (c & 0xf);
                    reset();
                    throw std::runtime_error(msg.str());
                }
            }
        }
    }
}
