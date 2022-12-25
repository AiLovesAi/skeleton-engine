#include "gm_bkv_state_array.hpp"

namespace game {
    void BKV_State_Array::parse(BKV_Buffer& buf, const char c) {
        if (!size_) {
            arrayStart_ = buf.head;
            buf.head += sizeof(uint32_t);
        }
        // TODO
    }
}
