#pragma once

#include "gm_bkv.hpp"
#include "gm_bkv_buffer.hpp"

#include "../../headers/float.hpp"

namespace game {
    class BKV_Builder {
        public:
            // Functions
            BKV_t build() {
                uint8_t* buffer = static_cast<uint8_t*>(std::malloc(_buffer._head));
                std::memcpy(buffer, _buffer._bkv, _buffer._head);
                std::shared_ptr<const uint8_t> data(buffer, std::free);
                return BKV_t{_buffer._head, data};
            }

            template<typename T>
            void setInt(BKV_t& bkv, const UTF8Str& key, const T  value);
            template<typename T>
            void setIntList(BKV_t& bkv, const UTF8Str& key, const T* value, const uint32_t size);
            void setFloat(BKV_t& bkv, const UTF8Str& key, const float32_t value);
            void setFloatList(BKV_t& bkv, const UTF8Str& key, const float32_t* value);
            void setDouble(BKV_t& bkv, const UTF8Str& key, const float128_t value);
            void setDoubleList(BKV_t& bkv, const UTF8Str& key, const float128_t* value);
            void setBool(BKV_t& bkv, const UTF8Str& key, const bool value);
            void setBoolList(BKV_t& bkv, const UTF8Str& key, const bool* value);
            void setStr(BKV_t& bkv, const UTF8Str& key, const UTF8Str& value);
            void setStrList(BKV_t& bkv, const UTF8Str& key, const UTF8Str* value, const uint16_t size);
        
        private:
            // Variables
            BKV_Buffer _buffer;
    };
}
