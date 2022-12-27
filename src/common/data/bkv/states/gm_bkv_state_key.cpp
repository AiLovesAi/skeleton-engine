#include "gm_bkv_state_key.hpp"

#include "../../gm_endianness.hpp"
#include "../../gm_buffer_memory.hpp"

#include <sstream>
#include <stdexcept>

namespace game {
    void BKV_State_Key::parse(BKV_Buffer& buf, const char c) {
        if ((c == '{') && !buf.head) {
            // Opening compound, ignore
            return;
        } else if ((c == '}') && !keyLen_) {
            // Compound ended or is empty, and another one is ending. Ex: {ex1:{ex2:{id:1}},xe:5}
            try { buf.endCompound(); } catch (std::exception e) { throw e; }
        } else if ((c == ' ') && !keyLen_) {
            // Whitespace, igore
            return;
        } else if (std::isalpha(c) || (lastChar_ != DEFAULT_CHAR && (std::isdigit(c) || c == '_' || c == '.' || c == '+' || c == '-'))) {
            // TODO Allow for quoted keys
            // Build key
            keyLen_++;
            if (keyLen_ > UINT8_MAX) {
                reset();
                std::stringstream msg;
                msg << "Too many characters in BKV key: " << keyLen_ << "/255 characters.";
                throw std::length_error(msg.str());
            }
            key_[keyLen_ - 1] = c;
            lastChar_ = c;
        } else if (c == ':') {
            buf.tagHead = buf.head;
            const size_t len = Endianness::hton(keyLen_);

            try {
                BufferMemory::checkResize(buf.bkv, buf.head + 2 + keyLen_, buf.head, buf.capacity);
            } catch (std::exception e) { throw e; }
            std::memcpy(buf.bkv + buf.head + 1, &len, sizeof(uint8_t));
            buf.head += 2; // Add 1 for tag (added later) and 1 for key length
            std::memcpy(buf.bkv + buf.head, key_, keyLen_);
            buf.head += keyLen_;
            buf.valHead = buf.head;
            buf.state = BKV_State::findTagState();
            reset();
        } else if (!((c == ',') && !keyLen_)) {
            // If a compound hasn't just ended, this is an unexpected input
            reset();
            std::stringstream msg;
            msg << "Invalid character in BKV key at index " << keyLen_ << ": 0x" << ((c & 0xf0) >> 4) << (c & 0xf);
            throw std::invalid_argument(msg.str());
        } else return;
        // NOTE: In case a compound just ended and we are at the comma following it, just continue. Ex: {ex1:{ex2:{id:1}},xe:5}
    }
}
