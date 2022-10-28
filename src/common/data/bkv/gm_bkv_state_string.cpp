#include "gm_bkv_state_string.hpp"

#include <sstream>
#include <stdexcept>

namespace game {
    inline void checkResize(char*& ptr, const size_t head, size_t& capacity) {
        if (head > capacity) {
            capacity <<= 1;
            ptr = static_cast<char*>(std::realloc(ptr, capacity));
        }
    }

    void BKV_State_String::parse(BKV::BKVbuf_t& buf, const char c) {
        // TODO Copy string until finished
        if ((c == ',' && strChar_ == DEFAULT_CHAR) || (c == strChar_ && lastChar_ != '\\')) {
            // String complete
            // TODO Copy string to buffer
            buf.state = parent_; // TODO buf.state->parse(buf, c);
            reset();
        } else {
            if (strLen_ + 1 >= UINT16_MAX) {
                std::stringstream msg;
                msg << "Too many characters in BKV string: " << strLen_ << "/255 characters.";
                throw std::length_error(msg.str());
            }
            checkResize(str_, strLen_, strCapacity_);
            str_[strLen_] = c;
        }
    }
}
