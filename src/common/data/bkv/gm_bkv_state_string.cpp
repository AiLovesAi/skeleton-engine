#include "gm_bkv_state_string.hpp"

namespace game {
    inline void checkResize(char*& ptr, const size_t head, size_t& capacity) {
        if (head > capacity) {
            capacity *= 2;
            ptr = static_cast<char*>(std::realloc(ptr, capacity));
        }
    }

    void BKV_State_String::parse(BKV::BKVbuf_t& buf, const char c) {
        // TODO Copy string until finished
        if ((c == ',' && strChar_ == DEFAULT_CHAR) || (c == strChar_ && lastChar_ != '\\')) {
            // String complete
        }
    }
}
