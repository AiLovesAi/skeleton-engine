#include "gm_bkv_state_find_tag.hpp"

namespace game {
    void BKV_State_Find_Tag::parse(BKV_Buffer& buf, const char c) {
        // TODO Find tag and set corresponding parser
        // TODO Set parent of child to name since this is a one-off test
        if (c == '\'' || c == '"') {
            goto _str;
        } else if (c >= '0' && c <= '9') {
            // TODO
        } else if (c == '[') {
            // TODO
        } else {
_str:
            buf.state = BKV_State::stringState();
            buf.state->parse(buf, c);
        }
    }
}
