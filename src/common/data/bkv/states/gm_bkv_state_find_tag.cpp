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
        } else if (std::isdigit(c) || c == '-' || c == '.') {
            buf.state = BKV_State::numberState();
            buf.state->parse(buf, c);
        } else if (c == '[') {
            buf.type |= BKV_ARRAY_FLAG;
            buf.state = BKV_State::arrayState();
        } else if (c == '{') {
            buf.type |= BKV::BKV_COMPOUND;
            buf.state = BKV_State::nameState(); // TODO Allow compound close, use compound depth
        } else if (std::isalnum(c)) {
            parseStr(buf);
        } else {
            std::stringstream msg;
            msg << "Invalid character in BKV string at index " << nameLen_ << ": 0x" << ((c & 0xf0) >> 4) << (c & 0xf);
            throw std::invalid_argument(msg.str());
        }
    }
}
