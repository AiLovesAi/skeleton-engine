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
            msg << "Compound in unclosed BKV array at index " << charactersRead_;
            throw std::runtime_error(msg.str());
        }
        
        // Increase compound depth and return to name state for next input
        try {
            BufferMemory::checkResize(bkv_, head_ + static_cast<int64_t>(sizeof(uint32_t)) + 1, head_, capacity_);
        } catch (std::runtime_error &e) { throw; }
        bkv_[tagHead_] = BKV::BKV_COMPOUND;
        if (tagHead_ == head_) {
            head_++;
            bkv_[head_] = '\0'; // 0 for name length
            head_++; // This will be true for the opening compound
        }
        depth_.push(head_);
        head_ += sizeof(uint32_t);
        valHead_ = head_;
        tagHead_ = head_;
        
        std::stringstream m;
        m << "Opening compound with new depth: " << depth_.size();
        Logger::log(LOG_INFO, m.str());
        if (depth_.size() > UINT8_MAX) {
            std::stringstream msg;
            msg << "Reached maximum compound depth in BKV at index " << charactersRead_ << ": " << depth_.size() << "/" << UINT8_MAX;
            throw std::runtime_error(msg.str());
        }
    }

    void BKV_Buffer::closeCompound() {
        try {
            BufferMemory::checkResize(bkv_, head_ + 1, head_, capacity_);
        } catch (std::runtime_error &e) { throw; }
        bkv_[head_] = BKV::BKV_END;
        head_++;

        int64_t size = head_ - depth_.top() + sizeof(uint32_t);
        if ((size <= 0) || (size > UINT32_MAX)) {
            std::stringstream msg;
            msg << "BKV compound bigger than maximum size at index" << depth_.top() << ": " << size << "/" << UINT32_MAX;
            throw std::runtime_error(msg.str());
        }
        uint32_t len = Endianness::hton(static_cast<uint32_t>(size));
        std::memcpy(bkv_ + depth_.top(), &len, sizeof(uint32_t));
        std::stringstream m;
        m << "Closing compound with new depth: " << depth_.size() - 1 << " Length of " << size << " (net " << len << ") put at " << depth_.top();
        Logger::log(LOG_INFO, m.str());
        depth_.pop();
        valHead_ = head_;
        tagHead_ = head_;
        tag_ = 0;
        
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
                msg << "Closing character is '}' when in BKV array at index: " << charactersRead_;
                throw std::runtime_error(msg.str());
            }

            // Make sure tag has not changed
            if (!arrayState_.arrayTag_) {
                arrayState_.arrayTag_ = tag_;
            } else if (tag_ != arrayState_.arrayTag_) {
                std::stringstream msg;
                msg << "Array value changed data type at index: " << charactersRead_;
                reset();
                throw std::runtime_error(msg.str());
            }

            stateTree_.push(&arrayState_);
            try { state()->parse(*this, c); } catch (std::runtime_error &e) { throw; }
        } else {
            if (c == ']') {
                std::stringstream msg;
                msg << "Closing character is ']' when not in BKV array at index: " << charactersRead_;
                throw std::runtime_error(msg.str());
            }
            
            bkv_[tagHead_] = tag_;
            if (c == '}') {
                try { closeCompound(); } catch (std::runtime_error &e) { throw; }
            }
            if (depth_.size()) stateTree_.pop();
            tagHead_ = head_;
            tag_ = 0;
        }
    }
};
