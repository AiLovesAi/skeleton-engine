#include "gm_bkv_state_number.hpp"

#include "../gm_buffer_memory.hpp"
#include "../../gm_endianness.hpp"

#include <sstream>

namespace game {
    void BKV_State_Number::parse(BKV_Buffer& buf, const char c) {
        if (std::isdigit(c) || c == '.' || (c == '-' && bufLen_ == 0)) {
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
                    if (unsigned_) {
                        buf.tag = (buf.tag & BKV_ARRAY_FLAG) ? BKV::BKV_UI8_ARRAY : BKV::BKV_UI8;
                    } else {
                        buf.tag = (buf.tag & BKV_ARRAY_FLAG) ? BKV::BKV_I8_ARRAY : BKV::BKV_I8;
                    }
                    // TODO Input validation
                    int8_t val = Endianness::hton(atoi(numBuf_));
                    std::memcpy(buf.bkv + buf.head, &val, sizeof(int8_t));
                    buf.head++;
                    reset();
                    // TODO Same for other cases
                } break;
                case 'd': {
                    buf.tag = (buf.tag & BKV_ARRAY_FLAG) ? BKV::BKV_DOUBLE_ARRAY : BKV::BKV_DOUBLE;
                } break;
                case 'f': {
                    buf.tag = (buf.tag & BKV_ARRAY_FLAG) ? BKV::BKV_FLOAT_ARRAY : BKV::BKV_FLOAT;
                } break;
                case 'i': {
                    if (unsigned_) {
                        buf.tag = (buf.tag & BKV_ARRAY_FLAG) ? BKV::BKV_UI32_ARRAY : BKV::BKV_UI32;
                    } else {
                        buf.tag = (buf.tag & BKV_ARRAY_FLAG) ? BKV::BKV_I32_ARRAY : BKV::BKV_I32;
                    }
                } break;
                case 'l': {
                    if (unsigned_) {
                        buf.tag = (buf.tag & BKV_ARRAY_FLAG) ? BKV::BKV_UI64_ARRAY : BKV::BKV_UI64;
                    } else {
                        buf.tag = (buf.tag & BKV_ARRAY_FLAG) ? BKV::BKV_I64_ARRAY : BKV::BKV_I64;
                    }
                } break;
                case 's': {
                    if (unsigned_) {
                        buf.tag = (buf.tag & BKV_ARRAY_FLAG) ? BKV::BKV_UI16_ARRAY : BKV::BKV_UI16;
                    } else {
                        buf.tag = (buf.tag & BKV_ARRAY_FLAG) ? BKV::BKV_I16_ARRAY : BKV::BKV_I16;
                    }
                } break;
                case 'u': {
                    unsigned_ = true;
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
