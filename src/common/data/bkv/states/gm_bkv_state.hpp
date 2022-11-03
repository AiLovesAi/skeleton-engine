#pragma once

#include "gm_bkv_state_array.hpp"
#include "gm_bkv_state_find_tag.hpp"
#include "gm_bkv_state_name.hpp"
#include "gm_bkv_state_number.hpp"
#include "gm_bkv_state_string.hpp"
#include "../gm_bkv_buffer.hpp"

#include <cctype>

namespace game {
    class BKV_State {
        public:
            // Types
            static constexpr uint8_t BKV_ARRAY_FLAG = 1 << 7;
            
            // Functions
            virtual void parse(BKV_Buffer& buf, const char c) = 0;

            static BKV_State* nameState() { return &nameState_; }
            static BKV_State* stringState() { return &stringState_; }
            static BKV_State* findTagState() { return &findTagState_; }
            static BKV_State* arrayState() { return &arrayState_; }
            static BKV_State* numberState() { return &numberState_; }
        
        private:
            // Variables
            static BKV_State_Name nameState_;
            static BKV_State_String stringState_;
            static BKV_State_Find_Tag findTagState_;
            static BKV_State_Array arrayState_;
            static BKV_State_Number numberState_;
    };
}
