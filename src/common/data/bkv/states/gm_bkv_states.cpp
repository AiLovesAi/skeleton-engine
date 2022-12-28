#include "gm_bkv_states.hpp"

namespace game {
    BKV_State_Key BKV_States::keyState_;
    BKV_State_String BKV_States::stringState_;
    BKV_State_Find_Tag BKV_States::findTagState_;
    BKV_State_Array BKV_States::arrayState_;
    BKV_State_Number BKV_States::numberState_;
    BKV_State_Complete BKV_States::completeState_;
}
