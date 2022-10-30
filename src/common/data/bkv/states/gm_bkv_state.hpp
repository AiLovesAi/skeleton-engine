#pragma once

#include "gm_bkv_state_find_tag.hpp"
#include "gm_bkv_state_name.hpp"
#include "gm_bkv_state_string.hpp"
#include "../gm_bkv_buffer.hpp"

namespace game {
    class BKV_State {
        public:
            // Functions
            virtual void parse(BKV_Buffer& buf, const char c) = 0;

            static BKV_State* nameState() { return &nameState_; }
            static BKV_State* stringState() { return &stringState_; }
            static BKV_State* findTagState() { return &findTagState_; }
            
        protected:
            // Variables
            BKV_State* parent_ = nullptr;
        
        private:
            // Variables
            static BKV_State_Name nameState_;
            static BKV_State_String stringState_;
            static BKV_State_Find_Tag findTagState_;
    };
}
