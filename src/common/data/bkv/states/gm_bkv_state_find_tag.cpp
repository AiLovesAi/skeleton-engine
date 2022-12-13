#include "gm_bkv_state_find_tag.hpp"

#include <sstream>

namespace game {
    static void parseStr(BKV_Buffer& buf, const char c) {
        buf.tag = BKV::BKV_STR;
        buf.state = BKV_State::stringState();
        buf.state->parse(buf, c);
    }
    
    void BKV_State_Find_Tag::parse(BKV_Buffer& buf, const char c) {
        len_++;
        if (c == '\'' || c == '"') {
            parseStr(buf, c);
        } else if (std::isdigit(c) || c == '-' || c == '.') {
            buf.state = BKV_State::numberState();
            buf.state->parse(buf, c);
        } else if (c == '[') {
            buf.tag |= BKV_ARRAY_FLAG;
            buf.state = BKV_State::arrayState();
        } else if (c == '{') {
            buf.tag |= BKV::BKV_COMPOUND;
            buf.state = BKV_State::nameState(); // TODO Allow compound close, use compound depth
        } else if (std::isalnum(c)) {
            parseStr(buf, c);
        } else {
            std::stringstream msg;
            msg << "Invalid character in BKV string at index " << len_ << ": 0x" << ((c & 0xf0) >> 4) << (c & 0xf);
            throw std::invalid_argument(msg.str());
        }
    }
}
