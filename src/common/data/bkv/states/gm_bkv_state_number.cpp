#include "gm_bkv_state_number.hpp"

#include "../gm_buffer_memory.hpp"
#include "../../gm_endianness.hpp"

#include <cfloat>
#include <cmath>
#include <sstream>

namespace game {
    void BKV_State_Number::parse(BKV_Buffer& buf, const char c) {
        if (std::isdigit(c) || (c == '.' && !hasDecimal_) || (c == '-' && bufLen_ == 0)) {
            if (c == '.') {
                hasDecimal_ = true;
            }
            bufLen_++;
            if (bufLen_ >= UINT8_MAX) {
                std::stringstream msg;
                msg << "Too many digits in BKV number: " << bufLen_ << "/255 digits.";
                throw std::length_error(msg.str());
            }
            BufferMemory::checkResize(numBuf_, bufLen_ + 1, bufCapacity_);
            numBuf_[bufLen_ - 1] = c;
        } else {
            numBuf_[bufLen_] = '\0';
            switch (c) {
                case 'b': {
                    buf.tag |= BKV::BKV_I8;
                    if (buf.tag & BKV::BKV_UNSIGNED) {
                        uint64_t val = std::strtoull(numBuf_, nullptr, 10);
                        // Make sure value doesn't overflow
                        if (val > UINT8_MAX) {
                            std::stringstream msg;
                            msg << "Value overflows short: " << val << " > " << UINT8_MAX << ".";
                            throw std::overflow_error(msg.str());
                        }
                        uint8_t v = Endianness::hton(static_cast<uint8_t>(val));
                        std::memcpy(buf.bkv + buf.head, &val, sizeof(uint8_t));
                    } else {
                        int64_t val = std::strtoll(numBuf_, nullptr, 10);
                        // Make sure value doesn't overflow or underflow
                        if (val < INT8_MIN) {
                            std::stringstream msg;
                            msg << "Value underflows short: " << val << " < " << INT8_MIN << ".";
                            throw std::underflow_error(msg.str());
                        } else if (val > INT8_MAX) {
                            std::stringstream msg;
                            msg << "Value overflows short: " << val << " > " << INT8_MAX << ".";
                            throw std::overflow_error(msg.str());
                        }
                        int8_t v = Endianness::hton(static_cast<int8_t>(val));
                        std::memcpy(buf.bkv + buf.head, &val, sizeof(int8_t));
                    }
                    buf.head++;
                    reset();
                } break;
                case 'd': {
                    buf.tag |= BKV::BKV_DOUBLE;
                    long double val = std::strtold(numBuf_, nullptr);
                    // Make sure value doesn't overflow or underflow
                    if ((val <= LDBL_MIN) || (val == -HUGE_VALL)) {
                        std::stringstream msg;
                        msg << "Value underflows double: " << val << " <= " << LDBL_MIN << ".";
                        throw std::underflow_error(msg.str());
                    } else if ((val >= LDBL_MAX) || (val == HUGE_VALL)) {
                        std::stringstream msg;
                        msg << "Value overflows double: " << val << " >= " << LDBL_MAX << ".";
                        throw std::overflow_error(msg.str());
                    }
                    long double v = Endianness::htond(val);
                    std::memcpy(buf.bkv + buf.head, &val, sizeof(long double));
                    buf.head += sizeof(long double);
                    reset();
                } break;
                case 'f': {
                    buf.tag |= BKV::BKV_FLOAT;
                    long double val = std::strtold(numBuf_, nullptr);
                    // Make sure value doesn't overflow or underflow
                    if ((val < FLT_MIN) || (val == -HUGE_VALL)) {
                        std::stringstream msg;
                        msg << "Value underflows double: " << val << " <= " << FLT_MIN << ".";
                        throw std::underflow_error(msg.str());
                    } else if ((val > FLT_MAX) || (val == HUGE_VALL)) {
                        std::stringstream msg;
                        msg << "Value overflows double: " << val << " >= " << FLT_MAX << ".";
                        throw std::overflow_error(msg.str());
                    }
                    float v = Endianness::htonf(static_cast<float>(val));
                    std::memcpy(buf.bkv + buf.head, &val, sizeof(float));
                    buf.head += sizeof(float);
                    reset();
                } break;
                case 'i': {
                    buf.tag |= BKV::BKV_I32;
                    if (buf.tag & BKV::BKV_UNSIGNED) {
                        uint64_t val = std::strtoull(numBuf_, nullptr, 10);
                        // Make sure value doesn't overflow
                        if (val > UINT32_MAX) {
                            std::stringstream msg;
                            msg << "Value overflows integer: " << val << " > " << UINT32_MAX << ".";
                            throw std::overflow_error(msg.str());
                        }
                        uint32_t v = Endianness::hton(static_cast<uint32_t>(val));
                        std::memcpy(buf.bkv + buf.head, &val, sizeof(uint32_t));
                    } else {
                        int64_t val = std::strtoll(numBuf_, nullptr, 10);
                        // Make sure value doesn't overflow or underflow
                        if (val < INT32_MIN) {
                            std::stringstream msg;
                            msg << "Value underflows integer: " << val << " < " << INT32_MIN << ".";
                            throw std::underflow_error(msg.str());
                        } else if (val > INT32_MAX) {
                            std::stringstream msg;
                            msg << "Value overflows integer: " << val << " > " << INT32_MAX << ".";
                            throw std::overflow_error(msg.str());
                        }
                        int32_t v = Endianness::hton(static_cast<int32_t>(val));
                        std::memcpy(buf.bkv + buf.head, &val, sizeof(int32_t));
                    }
                    buf.head += sizeof(uint32_t);
                    reset();
                } break;
                case 'l': {
                    buf.tag |= BKV::BKV_I64;
                    if (buf.tag & BKV::BKV_UNSIGNED) {
                        uint64_t val = std::strtoull(numBuf_, nullptr, 10);
                        // Make sure value doesn't overflow
                        if (val == UINT64_MAX) {
                            std::stringstream msg;
                            msg << "Value overflows long: " << val << " >= " << UINT64_MAX << ".";
                            throw std::overflow_error(msg.str());
                        }
                        uint64_t v = Endianness::hton(val);
                        std::memcpy(buf.bkv + buf.head, &val, sizeof(uint64_t));
                    } else {
                        int64_t val = std::strtoll(numBuf_, nullptr, 10);
                        // Make sure value doesn't overflow or underflow
                        if (val == INT64_MIN) {
                            std::stringstream msg;
                            msg << "Value underflows long: " << val << " <= " << INT64_MIN << ".";
                            throw std::underflow_error(msg.str());
                        } else if (val == INT64_MAX) {
                            std::stringstream msg;
                            msg << "Value overflows long: " << val << " >= " << INT64_MAX << ".";
                            throw std::overflow_error(msg.str());
                        }
                        int64_t v = Endianness::hton(val);
                        std::memcpy(buf.bkv + buf.head, &val, sizeof(int64_t));
                    }
                    buf.head += sizeof(uint64_t);
                    reset();
                } break;
                case 's': {
                    buf.tag |= BKV::BKV_I16;
                    if (buf.tag & BKV::BKV_UNSIGNED) {
                        uint64_t val = std::strtoull(numBuf_, nullptr, 10);
                        // Make sure value doesn't overflow
                        if (val > UINT16_MAX) {
                            std::stringstream msg;
                            msg << "Value overflows short: " << val << " > " << UINT16_MAX << ".";
                            throw std::overflow_error(msg.str());
                        }
                        uint16_t v = Endianness::hton(static_cast<int16_t>(val));
                        std::memcpy(buf.bkv + buf.head, &val, sizeof(uint16_t));
                    } else {
                        int64_t val = std::strtoll(numBuf_, nullptr, 10);
                        // Make sure value doesn't overflow or underflow
                        if (val < INT16_MIN) {
                            std::stringstream msg;
                            msg << "Value underflows short: " << val << " < " << INT16_MIN << ".";
                            throw std::underflow_error(msg.str());
                        } else if (val > INT16_MAX) {
                            std::stringstream msg;
                            msg << "Value overflows short: " << val << " > " << INT16_MAX << ".";
                            throw std::overflow_error(msg.str());
                        }
                        int16_t v = Endianness::hton(static_cast<int16_t>(val));
                        std::memcpy(buf.bkv + buf.head, &val, sizeof(int16_t));
                    }
                    buf.head += sizeof(uint16_t);
                    reset();
                } break;
                case 'u': {
                    buf.tag |= BKV::BKV_UNSIGNED;
                } break;
                default: {
                    std::stringstream msg;
                    msg << "Invalid character in BKV number at index: " << bufLen_ << ": 0x" << ((c & 0xf0) >> 4) << (c & 0xf);
                    throw std::invalid_argument(msg.str());
                }
            }
        }
    }
}
