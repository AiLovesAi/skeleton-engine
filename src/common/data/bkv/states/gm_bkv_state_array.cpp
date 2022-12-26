#include "gm_bkv_state_array.hpp"

#include "../gm_buffer_memory.hpp"
#include "../../gm_endianness.hpp"

#include <sstream>
#include <stdexcept>

namespace game {
    void BKV_State_Array::parse(BKV_Buffer& buf, const char c) {
        if (!size_) {
            arrayStart_ = buf.head;
            arrayTagHead_ = buf.tagHead;
            try {
                BufferMemory::checkResize(buf.bkv, buf.head + sizeof(uint32_t), buf.head, buf.capacity);
            } catch (std::exception e) {
                reset();
                throw e;
            }
            buf.head += sizeof(uint32_t);
        } else if (buf.tag & ~(BKV::BKV_ARRAY | BKV::BKV_UNSIGNED)) {
            // Tag is found, make sure it has not changed
            if (!arrayTag_) {
                arrayTag_ = buf.tag;
            } else if (buf.tag != arrayTag_) {
                reset();
                std::stringstream msg;
                msg << "Array value changed data type at index: " << buf.head << ".";
                throw std::invalid_argument(msg.str());
            }

            size_++;
            if (c == ',') {
                // Continue array
                buf.state = buf.parentState;
            } else if (c == ']') {
                // End array
                buf.bkv[arrayTagHead_] = buf.tag;
                uint32_t val = Endianness::hton(static_cast<uint32_t>(size_));
                std::memcpy(buf.bkv + arrayStart_, &val, sizeof(uint32_t));
                
                reset();
                buf.valHead = buf.head;
                buf.state = BKV_State::keyState();
                buf.tagHead = buf.head;
                buf.tag = 0;
            } else {
                reset();
                std::stringstream msg;
                msg << "Invalid character in BKV string at index: " << buf.head << ".";
                throw std::invalid_argument(msg.str());
            }
        } else {
            buf.state = BKV_State::findTagState();
            try {
                buf.state->parse(buf, c);
            } catch (std::exception e) {
                reset();
                throw e;
            }
        }
    }
}
