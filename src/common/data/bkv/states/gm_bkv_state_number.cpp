#include "gm_bkv_state_number.hpp"

#include "../gm_buffer_memory.hpp"
#include "../../gm_endianness.hpp"

#include <cfloat>
#include <cmath>
#include <sstream>

namespace game {
    void BKV_State_Number::parseDouble(BKV_Buffer& buf) {
        buf.tag |= BKV::BKV_DOUBLE;
        long double val = std::strtold(numBuf_, nullptr);
        // Make sure value doesn't overflow or underflow
        if ((val <= LDBL_MIN) || (val == -HUGE_VALL)) {
            reset();
            std::stringstream msg;
            msg << "BKV value underflows double: " << val << " <= " << LDBL_MIN << ".";
            throw std::underflow_error(msg.str());
        } else if ((val >= LDBL_MAX) || (val == HUGE_VALL)) {
            reset();
            std::stringstream msg;
            msg << "BKV value overflows double: " << val << " >= " << LDBL_MAX << ".";
            throw std::overflow_error(msg.str());
        }
        long double v = Endianness::htond(val);
        std::memcpy(buf.bkv + buf.head, &v, sizeof(long double));
        buf.head += sizeof(long double);
    }

    void BKV_State_Number::parse(BKV_Buffer& buf, const char c) {
        // Check if number has been completed and a tag is assigned
        if (buf.tag & ~(BKV::BKV_ARRAY | BKV::BKV_UNSIGNED)) {
            if ((c == '}') || (c == ',') || ((buf.tag & BKV::BKV_ARRAY) && (c == ']'))) {
                reset();
                try {
                    buf.endKV(buf, c);
                } catch (std::overflow_error e) {
                    throw e;
                }
            } else {
                reset();
                std::stringstream msg;
                msg << "Invalid character in BKV number at index: " << bufLen_ + 1 << ": 0x" << ((c & 0xf0) >> 4) << (c & 0xf);
                throw std::invalid_argument(msg.str());
            }
        } else if (std::isdigit(c) || (c == '.' && !hasDecimal_) || (c == '-' && bufLen_ == 0)) {
            // Build number string
            if (c == '.') {
                hasDecimal_ = true;
            }
            bufLen_++;
            if (bufLen_ > UINT8_MAX) {
                reset();
                std::stringstream msg;
                msg << "Too many digits in BKV number: " << bufLen_ << "/255 digits.";
                throw std::length_error(msg.str());
            }

            numBuf_[bufLen_ - 1] = c;
        } else {
            // Terminate number string and search for type
            numBuf_[bufLen_] = '\0';
            switch (c) {
                case ',':
                case ']':
                case '}': {
                    // Use default (integer if possible, long otherwise)
                    if (hasDecimal_) {
                        parseDouble(buf);
                    } else {
                        if (buf.tag & BKV::BKV_UNSIGNED) {
                            uint64_t val = std::strtoull(numBuf_, nullptr, 10);
                            // Make sure value doesn't overflow
                            if (val > UINT32_MAX) {
                                if (val == UINT64_MAX) {
                                    reset();
                                    std::stringstream msg;
                                    msg << "BKV value overflows long: " << val << " >= " << UINT64_MAX << ".";
                                    throw std::overflow_error(msg.str());
                                }
                                buf.tag |= BKV::BKV_I64;
                                uint64_t v = Endianness::hton(val);
                                std::memcpy(buf.bkv + buf.head, &v, sizeof(uint64_t));
                                buf.head += sizeof(uint64_t);
                            } else {
                                buf.tag |= BKV::BKV_I32;
                                uint32_t v = Endianness::hton(static_cast<uint32_t>(val));
                                std::memcpy(buf.bkv + buf.head, &v, sizeof(uint32_t));
                                buf.head += sizeof(uint32_t);
                            }
                        } else {
                            int64_t val = std::strtoll(numBuf_, nullptr, 10);
                            // Make sure value doesn't overflow or underflow
                            if ((val < INT32_MIN) || (val > INT32_MAX)) {
                                if (val == INT64_MIN) {
                                    reset();
                                    std::stringstream msg;
                                    msg << "BKV value underflows long: " << val << " <= " << INT64_MIN << ".";
                                    throw std::underflow_error(msg.str());
                                } else if (val == INT64_MAX) {
                                    reset();
                                    std::stringstream msg;
                                    msg << "BKV value overflows long: " << val << " >= " << INT64_MAX << ".";
                                    throw std::overflow_error(msg.str());
                                }
                                buf.tag |= BKV::BKV_I64;
                                int64_t v = Endianness::hton(val);
                                std::memcpy(buf.bkv + buf.head, &v, sizeof(int64_t));
                                buf.head += sizeof(int64_t);
                            } else {
                                buf.tag |= BKV::BKV_I32;
                                int32_t v = Endianness::hton(static_cast<int32_t>(val));
                                std::memcpy(buf.bkv + buf.head, &v, sizeof(int32_t));
                                buf.head += sizeof(int32_t);
                            }
                        }
                    }
                    // Start again now that we have a number
                    parse(buf, c);
                } break;
                case 'B':
                case 'b': {
                    buf.tag |= BKV::BKV_I8;
                    if (buf.tag & BKV::BKV_UNSIGNED) {
                        uint64_t val = std::strtoull(numBuf_, nullptr, 10);
                        // Make sure value doesn't overflow
                        if (val > UINT8_MAX) {
                            reset();
                            std::stringstream msg;
                            msg << "BKV value overflows short: " << val << " > " << UINT8_MAX << ".";
                            throw std::overflow_error(msg.str());
                        }
                        uint8_t v = Endianness::hton(static_cast<uint8_t>(val));
                        std::memcpy(buf.bkv + buf.head, &v, sizeof(uint8_t));
                    } else {
                        int64_t val = std::strtoll(numBuf_, nullptr, 10);
                        // Make sure value doesn't overflow or underflow
                        if (val < INT8_MIN) {
                            reset();
                            std::stringstream msg;
                            msg << "BKV value underflows short: " << val << " < " << INT8_MIN << ".";
                            throw std::underflow_error(msg.str());
                        } else if (val > INT8_MAX) {
                            reset();
                            std::stringstream msg;
                            msg << "BKV value overflows short: " << val << " > " << INT8_MAX << ".";
                            throw std::overflow_error(msg.str());
                        }
                        int8_t v = Endianness::hton(static_cast<int8_t>(val));
                        std::memcpy(buf.bkv + buf.head, &v, sizeof(int8_t));
                    }
                    buf.head++;
                } break;
                case 'D':
                case 'd': {
                    parseDouble(buf);
                } break;
                case 'F':
                case 'f': {
                    buf.tag |= BKV::BKV_FLOAT;
                    long double val = std::strtold(numBuf_, nullptr);
                    // Make sure value doesn't overflow or underflow
                    if ((val < FLT_MIN) || (val == -HUGE_VALL)) {
                        reset();
                        std::stringstream msg;
                        msg << "BKV value underflows double: " << val << " <= " << FLT_MIN << ".";
                        throw std::underflow_error(msg.str());
                    } else if ((val > FLT_MAX) || (val == HUGE_VALL)) {
                        reset();
                        std::stringstream msg;
                        msg << "BKV value overflows double: " << val << " >= " << FLT_MAX << ".";
                        throw std::overflow_error(msg.str());
                    }
                    float v = Endianness::htonf(static_cast<float>(val));
                    std::memcpy(buf.bkv + buf.head, &v, sizeof(float));
                    buf.head += sizeof(float);
                } break;
                case 'I':
                case 'i': {
                    buf.tag |= BKV::BKV_I32;
                    if (buf.tag & BKV::BKV_UNSIGNED) {
                        uint64_t val = std::strtoull(numBuf_, nullptr, 10);
                        // Make sure value doesn't overflow
                        if (val > UINT32_MAX) {
                            reset();
                            std::stringstream msg;
                            msg << "BKV value overflows integer: " << val << " > " << UINT32_MAX << ".";
                            throw std::overflow_error(msg.str());
                        }
                        uint32_t v = Endianness::hton(static_cast<uint32_t>(val));
                        std::memcpy(buf.bkv + buf.head, &v, sizeof(uint32_t));
                    } else {
                        int64_t val = std::strtoll(numBuf_, nullptr, 10);
                        // Make sure value doesn't overflow or underflow
                        if (val < INT32_MIN) {
                            reset();
                            std::stringstream msg;
                            msg << "BKV value underflows integer: " << val << " < " << INT32_MIN << ".";
                            throw std::underflow_error(msg.str());
                        } else if (val > INT32_MAX) {
                            reset();
                            std::stringstream msg;
                            msg << "BKV value overflows integer: " << val << " > " << INT32_MAX << ".";
                            throw std::overflow_error(msg.str());
                        }
                        int32_t v = Endianness::hton(static_cast<int32_t>(val));
                        std::memcpy(buf.bkv + buf.head, &v, sizeof(int32_t));
                    }
                    buf.head += sizeof(uint32_t);
                } break;
                case 'L':
                case 'l': {
                    buf.tag |= BKV::BKV_I64;
                    if (buf.tag & BKV::BKV_UNSIGNED) {
                        uint64_t val = std::strtoull(numBuf_, nullptr, 10);
                        // Make sure value doesn't overflow
                        if (val == UINT64_MAX) {
                            reset();
                            std::stringstream msg;
                            msg << "BKV value overflows long: " << val << " >= " << UINT64_MAX << ".";
                            throw std::overflow_error(msg.str());
                        }
                        uint64_t v = Endianness::hton(val);
                        std::memcpy(buf.bkv + buf.head, &v, sizeof(uint64_t));
                    } else {
                        int64_t val = std::strtoll(numBuf_, nullptr, 10);
                        // Make sure value doesn't overflow or underflow
                        if (val == INT64_MIN) {
                            reset();
                            std::stringstream msg;
                            msg << "BKV value underflows long: " << val << " <= " << INT64_MIN << ".";
                            throw std::underflow_error(msg.str());
                        } else if (val == INT64_MAX) {
                            reset();
                            std::stringstream msg;
                            msg << "BKV value overflows long: " << val << " >= " << INT64_MAX << ".";
                            throw std::overflow_error(msg.str());
                        }
                        int64_t v = Endianness::hton(val);
                        std::memcpy(buf.bkv + buf.head, &v, sizeof(int64_t));
                    }
                    buf.head += sizeof(uint64_t);
                } break;
                case 'S':
                case 's': {
                    buf.tag |= BKV::BKV_I16;
                    if (buf.tag & BKV::BKV_UNSIGNED) {
                        uint64_t val = std::strtoull(numBuf_, nullptr, 10);
                        // Make sure value doesn't overflow
                        if (val > UINT16_MAX) {
                            reset();
                            std::stringstream msg;
                            msg << "BKV value overflows short: " << val << " > " << UINT16_MAX << ".";
                            throw std::overflow_error(msg.str());
                        }
                        uint16_t v = Endianness::hton(static_cast<int16_t>(val));
                        std::memcpy(buf.bkv + buf.head, &v, sizeof(uint16_t));
                    } else {
                        int64_t val = std::strtoll(numBuf_, nullptr, 10);
                        // Make sure value doesn't overflow or underflow
                        if (val < INT16_MIN) {
                            reset();
                            std::stringstream msg;
                            msg << "BKV value underflows short: " << val << " < " << INT16_MIN << ".";
                            throw std::underflow_error(msg.str());
                        } else if (val > INT16_MAX) {
                            reset();
                            std::stringstream msg;
                            msg << "BKV value overflows short: " << val << " > " << INT16_MAX << ".";
                            throw std::overflow_error(msg.str());
                        }
                        int16_t v = Endianness::hton(static_cast<int16_t>(val));
                        std::memcpy(buf.bkv + buf.head, &v, sizeof(int16_t));
                    }
                    buf.head += sizeof(uint16_t);
                } break;
                case 'U':
                case 'u': {
                    buf.tag |= BKV::BKV_UNSIGNED;
                } break;
                default: {
                    reset();
                    std::stringstream msg;
                    msg << "Invalid character in BKV number at index: " << bufLen_ << ": 0x" << ((c & 0xf0) >> 4) << (c & 0xf);
                    throw std::invalid_argument(msg.str());
                }
            }
        }
    }
}
