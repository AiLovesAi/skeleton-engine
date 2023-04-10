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

            inline BKV_Builder reset() {
                _depth = std::stack<int32_t>();
                _buffer.reset();
                _buffer._head = BKV::BKV_COMPOUND_SIZE + 2;
                return *this;
            }

            inline BKV_Builder openCompound(const char*__restrict__ key) {
                return openCompound(UTF8Str{key});
            }
            template <typename T>
            inline BKV_Builder setValue(const char*__restrict__ key, const T value) {
                return setValue(UTF8Str{key}, value);
            }
            template <typename T>
            inline BKV_Builder setValueArray(const char*__restrict__ key, const T* values, const uint16_t arraySize) {
                return setValueArray(UTF8Str{key}, values, arraySize);
            }
            inline BKV_Builder setBool(const char*__restrict__ key, const bool value) {
                return setBool(UTF8Str{key}, value);
            }
            inline BKV_Builder setString(const char*__restrict__ key, const char*__restrict__ value) {
                return setString(UTF8Str{key}, UTF8Str{value});
            }
            inline BKV_Builder setString(const char*__restrict__ key, const UTF8Str& value) {
                return setString(UTF8Str{key}, value);
            }
            inline BKV_Builder setStringArray(const char*__restrict__ key, const UTF8Str* value, const uint16_t arraySize) {
                return setStringArray(UTF8Str{key}, value, arraySize);
            }

            BKV_Builder openCompound(const UTF8Str& key);
            BKV_Builder closeCompound();
            template <typename T>
            BKV_Builder setValue(const UTF8Str& key, const T value);
            template <typename T>
            BKV_Builder setValueArray(const UTF8Str& key, const T* values, const uint16_t arraySize);
            BKV_Builder setBool(const UTF8Str& key, const bool value);
            BKV_Builder setString(const UTF8Str& key, const UTF8Str& value);
            BKV_Builder setStringArray(const UTF8Str& key, const UTF8Str* value, const uint16_t arraySize);

        private:
            // Functions
            void _setKeyTag(const UTF8Str& key, const uint8_t tag, const uint64_t size);
            template <typename T>
            BKV_Builder _setInt(const UTF8Str& key, const T value, const uint8_t tag);
            template <typename T>
            BKV_Builder _setIntArray(const UTF8Str& key, const T* values, const uint16_t arraySize, const uint8_t tag);
            template <typename T>
            BKV_Builder _setFloat(const UTF8Str& key, const T value, const uint8_t tag);
            template <typename T>
            BKV_Builder _setFloatArray(const UTF8Str& key, const T* values, const uint16_t arraySize, const uint8_t tag);

            // Variables
            BKV_Buffer _buffer;
            std::stack<int32_t> _depth; // Current compound depth, with head at compound opening as value
    };
    
    // Specializations
    template<> BKV_Builder BKV_Builder::setValue<int8_t>(const UTF8Str& key, const int8_t value);
    template<> BKV_Builder BKV_Builder::setValue<uint8_t>(const UTF8Str& key, const uint8_t value);
    template<> BKV_Builder BKV_Builder::setValue<int16_t>(const UTF8Str& key, const int16_t value);
    template<> BKV_Builder BKV_Builder::setValue<uint16_t>(const UTF8Str& key, const uint16_t value);
    template<> BKV_Builder BKV_Builder::setValue<int32_t>(const UTF8Str& key, const int32_t value);
    template<> BKV_Builder BKV_Builder::setValue<uint32_t>(const UTF8Str& key, const uint32_t value);
    template<> BKV_Builder BKV_Builder::setValue<int64_t>(const UTF8Str& key, const int64_t value);
    template<> BKV_Builder BKV_Builder::setValue<uint64_t>(const UTF8Str& key, const uint64_t value);
    template<> BKV_Builder BKV_Builder::setValue<float32_t>(const UTF8Str& key, const float32_t value);
    template<> BKV_Builder BKV_Builder::setValue<float128_t>(const UTF8Str& key, const float128_t value);
    
    template<> BKV_Builder BKV_Builder::setValueArray<int8_t>(const UTF8Str& key, const int8_t* values, const uint16_t arraySize);
    template<> BKV_Builder BKV_Builder::setValueArray<uint8_t>(const UTF8Str& key, const uint8_t* values, const uint16_t arraySize);
    template<> BKV_Builder BKV_Builder::setValueArray<int16_t>(const UTF8Str& key, const int16_t* values, const uint16_t arraySize);
    template<> BKV_Builder BKV_Builder::setValueArray<uint16_t>(const UTF8Str& key, const uint16_t* values, const uint16_t arraySize);
    template<> BKV_Builder BKV_Builder::setValueArray<int32_t>(const UTF8Str& key, const int32_t* values, const uint16_t arraySize);
    template<> BKV_Builder BKV_Builder::setValueArray<uint32_t>(const UTF8Str& key, const uint32_t* values, const uint16_t arraySize);
    template<> BKV_Builder BKV_Builder::setValueArray<int64_t>(const UTF8Str& key, const int64_t* values, const uint16_t arraySize);
    template<> BKV_Builder BKV_Builder::setValueArray<uint64_t>(const UTF8Str& key, const uint64_t* values, const uint16_t arraySize);
    template<> BKV_Builder BKV_Builder::setValueArray<float32_t>(const UTF8Str& key, const float32_t* values, const uint16_t arraySize);
    template<> BKV_Builder BKV_Builder::setValueArray<float128_t>(const UTF8Str& key, const float128_t* values, const uint16_t arraySize);
}
