#include "gm_bkv_state_array.hpp"

#include "../gm_bkv.hpp"
#include "../gm_bkv_buffer.hpp"
#include "../../gm_buffer_memory.hpp"
#include "../../gm_endianness.hpp"

#include <sstream>
#include <stdexcept>

namespace game {
    void BKV_State_Array::parse(BKV_Buffer& buf, const char c) {
        std::stringstream m;
        m << "Array parsing character: '" << c << "', Array size: " << size_;
        Logger::log(LOG_INFO, m.str());

        if (!arrayStart_) {
            arrayStart_ = buf.head_;
            arrayTagHead_ = buf.tagHead_;
            try {
                BufferMemory::checkResize(buf.bkv_, buf.head_ + (int64_t) sizeof(uint16_t), buf.head_, buf.capacity_);
            } catch (std::runtime_error &e) {
                reset();
                throw;
            }
            buf.head_ += sizeof(uint16_t);
            
            size_++;
            buf.stateTree_.push(&buf.findTagState_);
            try {
                buf.state()->parse(buf, c);
            } catch (std::runtime_error &e) {
                reset();
                throw;
            }
        } else {
            if (c == ',') {
                buf.tag_ &= BKV::BKV_FLAGS_ALL; // Clear the tag so it can be found again
                
                // Continue array
                size_++;
                if (size_ > UINT16_MAX) {
                    std::stringstream msg;
                    msg << "Too many indicies in BKV array at index " << buf.charactersRead_ << ": " << size_ << "/" << UINT16_MAX << " indicies.";
                    reset();
                }
                buf.stateTree_.pop(); // Back to specific tag state
            } else if (c == ']') {
                // End array
                buf.bkv_[arrayTagHead_] = buf.tag_;
                uint32_t val = Endianness::hton(static_cast<uint16_t>(size_));
                std::memcpy(buf.bkv_ + arrayStart_, &val, sizeof(uint16_t));
                
                reset();
                buf.valHead_ = buf.head_;
                buf.stateTree_.pop(); // Back to specific tag state
                buf.stateTree_.pop(); // Back to array state
                buf.stateTree_.pop(); // Back to key state
                buf.tagHead_ = buf.head_;
                buf.tag_ = 0;
            } else {
                std::stringstream msg;
                msg << "Invalid character in BKV array at index " << buf.charactersRead_ << ": 0x" << std::hex << ((c & 0xf0) >> 4) << std::hex << (c & 0xf);
                reset();
                throw std::runtime_error(msg.str());
            }
        }
    }
}
