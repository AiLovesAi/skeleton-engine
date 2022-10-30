#include "gm_bkv_state_string.hpp"

#include "../../gm_buffer_memory.hpp"

#include <sstream>
#include <stdexcept>

namespace game {
    void BKV_State_String::parse(BKV_Buffer& buf, const char c) {
        // TODO Copy string until finished
        if ((c == ',' && strChar_ == DEFAULT_CHAR) || (c == strChar_ && lastChar_ != '\\')) {
            // String complete
            // TODO Copy string to buffer
            buf.state = parent_;
            reset();
        } else {
            strLen_++;
            if (strLen_ >= UINT16_MAX) {
                std::stringstream msg;
                msg << "Too many characters in BKV string: " << strLen_ << "/255 characters.";
                throw std::length_error(msg.str());
            }
            BufferMemory::checkResize(str_, strLen_, strCapacity_);
            str_[strLen_ - 1] = c;
            lastChar_ = c;
        }
    }
}
