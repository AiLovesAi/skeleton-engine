#include "gm_bkv_buffer.hpp"

#include "gm_bkv.hpp"
#include "../gm_buffer_memory.hpp"
#include "states/gm_bkv_state_complete.hpp"

#include <sstream>
#include <stdexcept>

namespace game {
    void BKV_Buffer::openCompound() {
        Logger::log(LOG_INFO, "Opening compound.");
        if (tag_ & BKV::BKV_ARRAY) {
            std::stringstream msg;
            msg << "Compound in unclosed BKV array at index " << head_ + 1 << ".";
            throw std::invalid_argument(msg.str());
        }
        
        // Increase compound depth and return to name state for next input
        try {
            BufferMemory::checkResize(bkv_, head_ + 1, head_, capacity_);
        } catch (std::exception &e) { throw e; }
        bkv_[tagHead_] = BKV::BKV_COMPOUND;
        head_++;
        valHead_ = head_;
        tagHead_ = head_;
        
        depth_++;
        if (depth_ > UINT8_MAX) {
            std::stringstream msg;
            msg << "Reached maximum compound depth in BKV at index " << head_ + 1 << ": " << depth_ << "/255.";
            throw std::overflow_error(msg.str());
        }
    }
    void BKV_Buffer::closeCompound() {
        Logger::log(LOG_INFO, "Closing compound.");
        depth_--;

        try {
            BufferMemory::checkResize(bkv_, head_ + 1, head_, capacity_);
        } catch (std::exception &e) { throw e; }
        bkv_[head_] = BKV::BKV_END;
        head_++;
        valHead_ = head_;
        tagHead_ = head_;
        tag_ = 0;
        
        if (depth_ < 0) {
            std::stringstream msg;
            msg << "BKV escaped compound at index " << head_ << ".";
            throw std::out_of_range(msg.str());
        } else if (!depth_) {
            // Depth has returned to zero, meaning the enclosing compound is closed; BKV is now finished.
            stateTree_.push(&completeState_);
        }
    }

    void BKV_Buffer::endKV(const char c) {
        std::stringstream m;
        m << "Ending key value with character: " << c;
        Logger::log(LOG_INFO, m.str());

        valHead_ = head_;
        if (tag_ & BKV::BKV_ARRAY) {
            if (c == '}') {
                std::stringstream msg;
                msg << "Closing character is '}' when in BKV array at index: " << head_ << ".";
                throw std::invalid_argument(msg.str());
            }
            stateTree_.push(&arrayState_);
            try { state()->parse(*this, c); } catch (std::exception &e) { throw e; }
        } else {
            if (c == ']') {
                std::stringstream msg;
                msg << "Closing character is ']' when not in BKV array at index: " << head_ << ".";
                throw std::invalid_argument(msg.str());
            }
            
            bkv_[tagHead_] = tag_;
            if (c == '}') {
                try { closeCompound(); } catch (std::exception &e) { throw e; }
            }
            stateTree_.pop();
            tagHead_ = head_;
            tag_ = 0;
        }
    }
};
