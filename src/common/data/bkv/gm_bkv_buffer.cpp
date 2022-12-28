#include "gm_bkv_buffer.hpp"

#include "gm_bkv.hpp"
#include "../gm_buffer_memory.hpp"

#include <sstream>
#include <stdexcept>

namespace game {
    BKV_State_Key BKV_Buffer::keyState_;
    BKV_State_String BKV_Buffer::stringState_;
    BKV_State_Find_Tag BKV_Buffer::findTagState_;
    //BKV_State_Array BKV_Buffer::arrayState_;
    BKV_State_Number BKV_Buffer::numberState_;
    BKV_State_Complete BKV_Buffer::completeState_;

    void BKV_Buffer::endCompound() {
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
            stateTree_.push(BKV_Buffer::completeState());
        }
    }

    void BKV_Buffer::endKV(const char c) {
        valHead_ = head_;
        if (tag_ & BKV::BKV_ARRAY) {
            if (c == '}') {
                std::stringstream msg;
                msg << "Closing character is '}' when in BKV array at index: " << head_ << ".";
                throw std::invalid_argument(msg.str());
            }
            stateTree_.push(BKV_Buffer::arrayState());
            try { state()->parse(*this, c); } catch (std::exception &e) { throw e; }
        } else {
            bkv_[tagHead_] = tag_;
            if (c == '}') {
                try { endCompound(); } catch (std::exception &e) { throw e; }
            }
            stateTree_.pop();
            tagHead_ = head_;
            tag_ = 0;
        }
    }
};
