#include "gm_bkv_state_number.hpp"

#include "../gm_buffer_memory.hpp"
#include "../../gm_endianness.hpp"

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
        } else if ((c == 'x') && (numBuf_[bufLen_ - 1] == '0')) {
            // TODO Hexidecimal (stores in as little data as possible up to 64 bits)
        } else {
            numBuf_[bufLen_] = '\0';
            switch (c) {
                case 'b': {
                    buf.tag |= BKV::BKV_I8;
                    int8_t val = Endianness::hton(std::atoi(numBuf_));
                    std::memcpy(buf.bkv + buf.head, &val, sizeof(int8_t));
                    buf.head++;
                    reset();
                    // TODO Same for other cases
                } break;
                case 'd': {
                    buf.tag |= BKV::BKV_DOUBLE;
                } break;
                case 'f': {
                    buf.tag |= BKV::BKV_FLOAT;
                } break;
                case 'i': {
                    buf.tag |= BKV::BKV_I32;
                } break;
                case 'l': {
                    buf.tag |= BKV::BKV_I64;
                } break;
                case 's': {
                    buf.tag |= BKV::BKV_I16;
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
