#include "gm_bkv_state_number.hpp"

namespace game {
    void BKV_State_Number::parse(BKV_Buffer& buf, const char c) {
        if (std::isdigit(c) || c == '.' || c == '-') {
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
                        bkv.tag = (bkv.tag & BKV_ARRAY_FLAG) ? BKV::BKV_UINT8_ARRAY : BKV::BKV_UINT8;
                    } else {
                        bkv.tag = (bkv.tag & BKV_ARRAY_FLAG) ? BKV::BKV_INT8_ARRAY : BKV::BKV_INT8;
                    }
                    // TODO Input validation
                    int8_t val = Endianness::hton(atoi(numBuf_));
                    std::memcpy(bkv.bkv + bkv.head, val, sizeof(int8_t));
                    bkv.head++;
                    reset();
                    // TODO Same for other cases
                } break;
                case 'd': {
                    bkv.tag = (bkv.tag & BKV_ARRAY_FLAG) ? BKV::BKV_DOUBLE_ARRAY : BKV::BKV_DOUBLE;
                } break;
                case 'f': {
                    bkv.tag = (bkv.tag & BKV_ARRAY_FLAG) ? BKV::BKV_FLOAT_ARRAY : BKV::BKV_FLOAT;
                } break;
                case 'i': {
                    if (unsigned_) {
                        bkv.tag = (bkv.tag & BKV_ARRAY_FLAG) ? BKV::BKV_UINT32_ARRAY : BKV::BKV_UINT32;
                    } else {
                        bkv.tag = (bkv.tag & BKV_ARRAY_FLAG) ? BKV::BKV_INT32_ARRAY : BKV::BKV_INT32;
                    }
                } break;
                case 'l': {
                    if (unsigned_) {
                        bkv.tag = (bkv.tag & BKV_ARRAY_FLAG) ? BKV::BKV_UINT64_ARRAY : BKV::BKV_UINT64;
                    } else {
                        bkv.tag = (bkv.tag & BKV_ARRAY_FLAG) ? BKV::BKV_INT64_ARRAY : BKV::BKV_INT64;
                    }
                } break;
                case 's': {
                    if (unsigned_) {
                        bkv.tag = (bkv.tag & BKV_ARRAY_FLAG) ? BKV::BKV_UINT16_ARRAY : BKV::BKV_UINT16;
                    } else {
                        bkv.tag = (bkv.tag & BKV_ARRAY_FLAG) ? BKV::BKV_INT16_ARRAY : BKV::BKV_INT16;
                    }
                } break;
                case 'u': {
                    unsigned_ = true;
                } break;
                default: {
                    std::stringstream msg;
                    msg << "Invalid character in BKV number at index: " << strLen_ << ": 0x" << ((c & 0xf0) >> 4) << (c & 0xf);
                    throw std::invalid_argument(msg.str());
                }
            }
        }
    }
}
