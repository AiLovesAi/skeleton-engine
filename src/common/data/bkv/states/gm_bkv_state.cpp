#include "gm_bkv_state.hpp"

namespace game {
    BKV_State_Key BKV_State::keyState_;
    BKV_State_String BKV_State::stringState_;
    BKV_State_Find_Tag BKV_State::findTagState_;
    BKV_State_Array BKV_State::arrayState_;
    BKV_State_Number BKV_State::numberState_;
    BKV_State_Complete BKV_State::completeState_;
}
