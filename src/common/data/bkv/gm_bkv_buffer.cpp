#include "gm_bkv_buffer.hpp"

#include "gm_bkv.hpp"
#include "../gm_buffer_memory.hpp"
#include "../gm_endianness.hpp"
#include "states/gm_bkv_state_complete.hpp"

#include <sstream>
#include <stdexcept>

namespace game {
    void BKV_Buffer::openCompound() {
        if (tag_ & BKV::BKV_ARRAY) {
            std::stringstream msg;
            msg << "Compound in unclosed BKV array at index " << head_ + 1 << ".";
            throw std::invalid_argument(msg.str());
        }
        
        // Increase compound depth and return to name state for next input
        try {
            BufferMemory::checkResize(bkv_, head_ + static_cast<int64_t>(sizeof(uint32_t)) + 1, head_, capacity_);
        } catch (std::exception &e) { throw e; }
        bkv_[tagHead_] = BKV::BKV_COMPOUND;
        if (tagHead_ == head_) head_++; // This will be true for the opening compound
        head_ += sizeof(uint32_t);
        depth_.push(head_);
        valHead_ = head_;
        tagHead_ = head_;
        
        std::stringstream m;
        m << "Opening compound with new depth: " << depth_.size();
        Logger::log(LOG_INFO, m.str());
        if (depth_.size() > UINT8_MAX) {
            std::stringstream msg;
            msg << "Reached maximum compound depth in BKV at index " << head_ + 1 << ": " << depth_.size() << "/255.";
            throw std::overflow_error(msg.str());
        }
    }
    void BKV_Buffer::closeCompound() {
        try {
            BufferMemory::checkResize(bkv_, head_ + 1, head_, capacity_);
        } catch (std::exception &e) { throw e; }
        bkv_[head_] = BKV::BKV_END;
        head_++;

        uint32_t len = Endianness::hton(head_ - depth_.top());
        std::memcpy(bkv_ + depth_.top() - sizeof(uint32_t), &len, sizeof(uint32_t));
        depth_.pop();
        valHead_ = head_;
        tagHead_ = head_;
        tag_ = 0;
        std::stringstream m;
        m << "Closing compound with new depth: " << depth_.size();
        Logger::log(LOG_INFO, m.str());
        
        if (!depth_.size()) {
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

            // Make sure tag has not changed
            if (!arrayState_.arrayTag_) {
                arrayState_.arrayTag_ = tag_;
            } else if (tag_ != arrayState_.arrayTag_) {
                reset();
                std::stringstream msg;
                msg << "Array value changed data type at index: " << head_ << ".";
                throw std::invalid_argument(msg.str());
            }

            tag_ &= BKV::BKV_FLAGS_ALL; // Clear the tag so it can be found again
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
            if (depth_.size()) stateTree_.pop();
            tagHead_ = head_;
            tag_ = 0;
        }
    }
};
