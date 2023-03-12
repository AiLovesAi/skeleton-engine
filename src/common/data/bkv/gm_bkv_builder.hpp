#pragma once

#include "gm_bkv.hpp"
#include "gm_bkv_buffer.hpp"

namespace game {
    class BKV_Builder {
        public:
            // Functions
            BKV_t build() {
                std::shared_ptr<const uint8_t> data(static_cast<uint8_t*>(std::malloc(buffer_.head_)), std::free);
                std::memcpy(data.get(), buffer_.bkv_, buffer_.head_);
                return BKV_t{buffer_.head_, data};
            }

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
