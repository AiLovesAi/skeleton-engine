#include "gm_bkv_state_name.hpp"

#include "../../gm_endianness.hpp"
#include "../../gm_buffer_memory.hpp"

#include <sstream>
#include <stdexcept>

namespace game {
    void BKV_State_Name::parse(BKV_Buffer& buf, const char c) {
        if ((c == '}') && !nameLen_) {
            // Compound ended or is empty, and another one is ending. Ex: {ex1:{ex2:{id:1}},xe:5}
            try {
                buf.endCompound();
            } catch (std::overflow_error e) {
                throw e;
            }
        } else if (std::isalpha(c) || (lastChar_ != DEFAULT_CHAR && std::isdigit(c))) {
            nameLen_++;
            if (nameLen_ > UINT8_MAX) {
                reset();
                std::stringstream msg;
                msg << "Too many characters in BKV name: " << nameLen_ << "/255 characters.";
                throw std::length_error(msg.str());
            }
            name_[nameLen_ - 1] = c;
            lastChar_ = c;
        } else if (c == ':') {
            buf.tagHead = buf.head;
            const size_t len = Endianness::hton(nameLen_);

            try {
                BufferMemory::checkResize(buf.bkv, buf.head + 2 + nameLen_, buf.head, buf.capacity);
            } catch (std::overflow_error e) {
                throw e;
            }
            std::memcpy(buf.bkv + buf.head + 1, &len, sizeof(uint8_t));
            buf.head += 2; // Add 1 for tag (added later) and 1 for name length
            std::memcpy(buf.bkv + buf.head, name_, nameLen_);
            buf.head += nameLen_;
            buf.valHead = buf.head;
            buf.state = BKV_State::findTagState();
            reset();
        } else if (!((c == ',') && !nameLen_)) {
            // If a compound hasn't just ended, this is an unexpected input
            reset();
            std::stringstream msg;
            msg << "Invalid character in BKV name at index " << nameLen_ << ": 0x" << ((c & 0xf0) >> 4) << (c & 0xf);
            throw std::invalid_argument(msg.str());
        }
        // NOTE: In case a compound just ended and we are at the comma following it, just continue. Ex: {ex1:{ex2:{id:1}},xe:5}
    }
}
