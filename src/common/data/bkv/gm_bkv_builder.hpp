#pragma once

#include "gm_bkv.hpp"
#include "gm_bkv_buffer.hpp"

#include "../../headers/float.hpp"

#include <stack>

namespace game {
    class BKV_Builder {
        public:
            // Constructors
            BKV_Builder() {
                _buffer._bkv[1] = 0;
                _buffer._head = BKV::BKV_COMPOUND_SIZE + 2;
            }
            ~BKV_Builder() {}

            // Functions
            [[nodiscard]] BKV build();

            inline void reset() {
                _depth = std::stack<int32_t>();
                _buffer.reset();
                _buffer._head = BKV::BKV_COMPOUND_SIZE + 2;
            }

            void openCompound(const UTF8Str& key);
            void closeCompound();

            template<typename T>
            void setInt(const UTF8Str& key, const T  value);
            template<typename T>
            void setIntArray(const UTF8Str& key, const T* value, const uint32_t size);
            template<typename T>
            void setFloat(const UTF8Str& key, const T value);
            template<typename T>
            void setFloatArray(const UTF8Str& key, const T* value);
            void setBool(const UTF8Str& key, const bool value);
            void setBoolArray(const UTF8Str& key, const bool* value);
            void setStr(const UTF8Str& key, const UTF8Str& value);
            void setStrArray(const UTF8Str& key, const UTF8Str* value, const uint16_t size);
        
        private:
            // Variables
            BKV_Buffer _buffer;
            std::stack<int32_t> _depth; // Current compound depth containing the start of the compound that flushes with head when it completes
    };
}
