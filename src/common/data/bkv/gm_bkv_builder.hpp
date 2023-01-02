#pragma once

#include "gm_bkv.hpp"
#include "gm_bkv_buffer.hpp"

namespace game {
    class BKV_Builder {
        public:
            // Functions
            // TODO builder.addCompound("test", builder.addStr("name", "Billy").addIntArray("money", ints, 5).addCompound("boba", builder.addInt("id", 1)).addBool("alive", true)).build();
            BKV_t build();

            template<typename T>
            void setInt(BKV_t& bkv, const UTF8Str& key, const T  value);
            template<typename T>
            void setIntList(BKV_t& bkv, const UTF8Str& key, const T* value, const uint32_t size);
            void setFloat(BKV_t& bkv, const UTF8Str& key, const float value);
            void setFloatList(BKV_t& bkv, const UTF8Str& key, const float* value);
            void setDouble(BKV_t& bkv, const UTF8Str& key, const double value);
            void setDoubleList(BKV_t& bkv, const UTF8Str& key, const double* value);
            void setBool(BKV_t& bkv, const UTF8Str& key, const bool value);
            void setBoolList(BKV_t& bkv, const UTF8Str& key, const bool* value);
            void setStr(BKV_t& bkv, const UTF8Str& key, const UTF8Str& value);
            void setStrList(BKV_t& bkv, const UTF8Str& key, const UTF8Str* value, const uint16_t size);
        
        private:
            // Variables
            BKV_Buffer buffer_;
    };
}
