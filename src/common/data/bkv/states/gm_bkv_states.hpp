#pragma once

#include "gm_bkv_state.hpp"
#include "gm_bkv_state_array.hpp"
#include "gm_bkv_state_find_tag.hpp"
#include "gm_bkv_state_key.hpp"
#include "gm_bkv_state_number.hpp"
#include "gm_bkv_state_string.hpp"
#include "gm_bkv_state_complete.hpp"

namespace game {
    class BKV_States {
        public:
            // Functions
            static BKV_State* keyState() { return &keyState_; }
            static BKV_State* stringState() { return &stringState_; }
            static BKV_State* findTagState() { return &findTagState_; }
            static BKV_State* arrayState() { return &arrayState_; }
            static BKV_State* numberState() { return &numberState_; }
            static BKV_State* completeState() { return &completeState_; }
        
        private:
            // Variables
            static BKV_State_Key keyState_;
            static BKV_State_String stringState_;
            static BKV_State_Find_Tag findTagState_;
            static BKV_State_Array arrayState_;
            static BKV_State_Number numberState_;
            static BKV_State_Complete completeState_;
    };
}