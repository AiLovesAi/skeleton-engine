#include "gm_bkv_state_find_tag.hpp"

namespace game {
    static void parseStr(BKV_Buffer& buf) {
        buf.tag = BKV::BKV_STRING;
        buf.state = BKV_State::stringState();
        buf.state->parse(buf, c);
    }
    
    void BKV_State_Find_Tag::parse(BKV_Buffer& buf, const char c) {
        if (c == '\'' || c == '"') {
            parseStr(buf);
        } else if (c >= '0' && c <= '9') {
            buf.state = BKV_State::numberState();
            buf.state->parse(buf, c);
        } else if (c == '[') {
            buf.type |= BKV_ARRAY_FLAG;
            buf.state = BKV_State::arrayState();
        } else {
            parseStr(buf);
        }
    }
}
