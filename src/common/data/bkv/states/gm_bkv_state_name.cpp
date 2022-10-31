#include "gm_bkv_state_name.hpp"

#include "../../gm_endianness.hpp"
#include "../../gm_buffer_memory.hpp"

#include <sstream>
#include <stdexcept>

namespace game {
    void BKV_State_Name::parse(BKV_Buffer& buf, const char c) {
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (lastChar_ != DEFAULT_CHAR && c >= '0' && c <= '9')) {
            nameLen_++;
            if (nameLen_ >= UINT16_MAX) {
                std::stringstream msg;
                msg << "Too many characters in BKV name: " << nameLen_ << "/255 characters.";
                throw std::length_error(msg.str());
            }
            name_[nameLen_ - 1] = c;
            lastChar_ = c;
        } else if (c == ':') {
            const size_t len = Endianness::hton(nameLen_);
            std::memcpy(buf.bkv + buf.head + 1, &len, sizeof(uint8_t));
            buf.head += 2;
            std::memcpy(buf.bkv + buf.head, name_, nameLen_);
            buf.head += nameLen_;
            buf.valHead = buf.head;
            buf.state = BKV_State::findTagState();
            reset();
        } else {
            std::stringstream msg;
            msg << "Invalid character in BKV name at index " << nameLen_ << ": 0x" << ((c & 0xf0) >> 4) << (c & 0xf);
            throw std::invalid_argument(msg.str());
        }
    }
}
