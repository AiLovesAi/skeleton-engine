#include "gm_bkv_state_array.hpp"

#include "../gm_bkv.hpp"
#include "../gm_bkv_buffer.hpp"
#include "../../gm_buffer_memory.hpp"
#include "../../gm_endianness.hpp"

#include <sstream>
#include <stdexcept>

namespace game {
    void BKV_State_Array::parse(BKV_Buffer& buf, const char c) {
        if (!size_) {
            arrayStart_ = buf.head_;
            arrayTagHead_ = buf.tagHead_;
            try {
                BufferMemory::checkResize(buf.bkv_, buf.head_ + (int64_t) sizeof(uint32_t), buf.head_, buf.capacity_);
            } catch (std::exception &e) {
                reset();
                throw e;
            }
            buf.head_ += sizeof(uint32_t);
        } else if (buf.tag_ & ~BKV::BKV_FLAGS_ALL) {
            // Tag is found, make sure it has not changed
            if (!arrayTag_) {
                arrayTag_ = buf.tag_;
            } else if (buf.tag_ != arrayTag_) {
                reset();
                std::stringstream msg;
                msg << "Array value changed data type at index: " << buf.head_ << ".";
                throw std::invalid_argument(msg.str());
            }

            size_++;
            if (c == ',') {
                // Continue array
                buf.stateTree_.pop(); // Back to specific tag state
            } else if (c == ']') {
                // End array
                buf.bkv_[arrayTagHead_] = buf.tag_;
                uint32_t val = Endianness::hton(static_cast<uint32_t>(size_));
                std::memcpy(buf.bkv_ + arrayStart_, &val, sizeof(uint32_t));
                
                reset();
                buf.valHead_ = buf.head_;
                buf.stateTree_.pop(); // Back to specific tag state
                buf.stateTree_.pop(); // Back to array state
                buf.stateTree_.pop(); // Back to key state
                buf.tagHead_ = buf.head_;
                buf.tag_ = 0;
            } else {
                reset();
                std::stringstream msg;
                msg << "Invalid character in BKV string at index: " << buf.head_ << ".";
                throw std::invalid_argument(msg.str());
            }
        } else {
            buf.stateTree_.push(BKV_Buffer::findTagState());
            try {
                buf.state()->parse(buf, c);
            } catch (std::exception &e) {
                reset();
                throw e;
            }
        }
    }
}
