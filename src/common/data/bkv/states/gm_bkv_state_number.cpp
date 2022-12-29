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
        } catch (std::exception &e) { throw e; }
        std::memcpy(buf.bkv_ + buf.head_, &val, sizeof(T));
        buf.head_ += sizeof(T);
    }

    template <typename T, typename TU>
    void BKV_State_Number::parseInt(BKV_Buffer& buf, const int64_t min, const int64_t max, const uint64_t umax) {
        if (buf.tag_ & BKV::BKV_UNSIGNED) {
            uint64_t val = std::strtoull(numBuf_, nullptr, 10);
            // Make sure value doesn't overflow
            if (val > umax) {
                reset();
                std::stringstream msg;
                msg << "BKV value overflows unsigned integer type: " << val << " > " << umax << ".";
                throw std::overflow_error(msg.str());
            }
            try { appendValue(buf, static_cast<TU>(val)); } catch (std::exception &e) { throw e; }
        } else {
            int64_t val = std::strtoll(numBuf_, nullptr, 10);
            // Make sure value doesn't overflow or underflow
            if (val < min) {
                reset();
                std::stringstream msg;
                msg << "BKV value underflows signed integer type: " << val << " < " << min << ".";
                throw std::underflow_error(msg.str());
            } else if (val > max) {
                reset();
                std::stringstream msg;
                msg << "BKV value overflows signed integer type: " << val << " > " << max << ".";
                throw std::overflow_error(msg.str());
            }
            try { appendValue(buf, static_cast<T>(val)); } catch (std::exception &e) { throw e; }
        }
    }

    void BKV_State_Number::parseLong(BKV_Buffer& buf) {
        buf.tag_ |= BKV::BKV_I64;
        if (buf.tag_ & BKV::BKV_UNSIGNED) {
            uint64_t val = std::strtoull(numBuf_, nullptr, 10);
            // Make sure value doesn't overflow
            if (val == UINT64_MAX) {
                reset();
                std::stringstream msg;
                msg << "BKV value overflows unsigned long type: " << val << " >= " << UINT64_MAX << ".";
                throw std::overflow_error(msg.str());
            }
            try { appendValue(buf, val); } catch (std::exception &e) { throw e; }
        } else {
            int64_t val = std::strtoll(numBuf_, nullptr, 10);
            // Make sure value doesn't overflow or underflow
            if (val == INT64_MIN) {
                reset();
                std::stringstream msg;
                msg << "BKV value underflows signed long type: " << val << " <= " << INT64_MIN << ".";
                throw std::underflow_error(msg.str());
            } else if (val == INT64_MAX) {
                reset();
                std::stringstream msg;
                msg << "BKV value overflows signed long type: " << val << " >= " << INT64_MAX << ".";
                throw std::overflow_error(msg.str());
            }
            try { appendValue(buf, val); } catch (std::exception &e) { throw e; }
        }
    }
    
    void BKV_State_Number::parseFloat(BKV_Buffer& buf) {
        buf.tag_ |= BKV::BKV_FLOAT;
        long double val = std::strtold(numBuf_, nullptr);
        // Make sure value doesn't overflow or underflow
        if ((val < FLT_MIN) || (val == -HUGE_VALL)) {
            reset();
            std::stringstream msg;
            msg << "BKV value underflows float: " << val << " <= " << FLT_MIN << ".";
            throw std::underflow_error(msg.str());
        } else if ((val > FLT_MAX) || (val == HUGE_VALL)) {
            reset();
            std::stringstream msg;
            msg << "BKV value overflows float: " << val << " >= " << FLT_MAX << ".";
            throw std::overflow_error(msg.str());
        }
        float v = Endianness::htonf(static_cast<float>(val));
        std::memcpy(buf.bkv_ + buf.head_, &v, sizeof(float));
        buf.head_ += sizeof(float);
    }

    void BKV_State_Number::parseDouble(BKV_Buffer& buf) {
        buf.tag_ |= BKV::BKV_DOUBLE;
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
        std::memcpy(buf.bkv_ + buf.head_, &v, sizeof(long double));
        buf.head_ += sizeof(long double);
    }
    
    void BKV_State_Number::endNumber(BKV_Buffer& buf, const char c) {
        reset();

        if ((c == '}') || (c == ',') || (c == ']')) {
            try { buf.endKV(c); } catch (std::exception &e) { throw e; }
        } else {
            std::stringstream msg;
            msg << "Invalid character in BKV number at index: " << bufLen_ + 1 << ": 0x" << ((c & 0xf0) >> 4) << (c & 0xf);
            throw std::invalid_argument(msg.str());
        }
    }

    void BKV_State_Number::parse(BKV_Buffer& buf, const char c) {
        std::stringstream m;
        m << "Number state parsing character: " << c;
        Logger::log(LOG_INFO, m.str());
        if (buf.tag_ & ~BKV::BKV_FLAGS_ALL) {
            // Check if number has been completed and a tag is assigned
            try { endNumber(buf, c); } catch (std::exception& e) {
                reset();
                throw e;
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
        } else if (std::isspace(c) && !bufLen_) {
            // Whitespace, ignore
            return;
        } else {
            // Terminate number string and search for type
            numBuf_[bufLen_] = '\0';
            switch (c) {
                case ',':
                case ']':
                case '}': {
                    // Use default (integer if possible, long otherwise)
                    if (hasDecimal_) {
                        try { parseDouble(buf); } catch (std::exception &e) {
                            reset();
                            throw e;
                        }
                    } else {
                        if (buf.tag_ & BKV::BKV_UNSIGNED) {
                            uint64_t val = std::strtoull(numBuf_, nullptr, 10);
                            // Make sure value doesn't overflow
                            if (val > UINT32_MAX) {
                                if (val == UINT64_MAX) {
                                    reset();
                                    std::stringstream msg;
                                    msg << "BKV value overflows long: " << val << " >= " << UINT64_MAX << ".";
                                    throw std::overflow_error(msg.str());
                                }
                                buf.tag_ |= BKV::BKV_I64;
                                try { appendValue(buf, val); } catch (std::exception &e) { throw e; }
                            } else {
                                buf.tag_ |= BKV::BKV_I32;
                                try { appendValue(buf, val); } catch (std::exception &e) { throw e; }
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
                                buf.tag_ |= BKV::BKV_I64;
                                try { appendValue(buf, val); } catch (std::exception &e) { throw e; }
                            } else {
                                buf.tag_ |= BKV::BKV_I32;
                                try { appendValue(buf, val); } catch (std::exception &e) { throw e; }
                            }
                        }
                    }
                    // Start again now that we have a number
                    try { endNumber(buf, c); } catch (std::exception &e) {
                        reset();
                        throw e;
                    }
                } break;
                case 'B':
                case 'b': {
                    buf.tag_ |= BKV::BKV_I8;
                    try { parseInt<int8_t, uint8_t>(buf, INT8_MIN, INT8_MAX, UINT8_MAX); } catch (std::exception &e) {
                        reset();
                        throw e;
                    }
                } break;
                case 'D':
                case 'd': {
                    try { parseDouble(buf); } catch (std::exception &e) {
                        reset();
                        throw e;
                    }
                } break;
                case 'F':
                case 'f': {
                    try { parseFloat(buf); } catch (std::exception &e) {
                        reset();
                        throw e;
                    }
                } break;
                case 'I':
                case 'i': {
                    buf.tag_ |= BKV::BKV_I32;
                    try { parseInt<int32_t, uint32_t>(buf, INT32_MIN, INT32_MAX, UINT32_MAX); } catch (std::exception &e) {
                        reset();
                        throw e;
                    }
                } break;
                case 'L':
                case 'l': {
                    try { parseLong(buf); } catch (std::exception &e) {
                        reset();
                        throw e;
                    }
                } break;
                case 'S':
                case 's': {
                    buf.tag_ |= BKV::BKV_I16;
                    try { parseInt<int16_t, uint16_t>(buf, INT16_MIN, INT16_MAX, UINT16_MAX); } catch (std::exception &e) {
                        reset();
                        throw e;
                    }
                } break;
                case 'U':
                case 'u': {
                    buf.tag_ |= BKV::BKV_UNSIGNED;
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
