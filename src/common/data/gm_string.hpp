#pragma once

#include "gm_utf8.hpp"

#include <memory>

namespace game {
    class String {
        public:
            // Functions
            static std::shared_pointer<char> formatString(const char* str, ...);
            // TODO General string functions that are currently implemented elsewhere (check File and BKV classes)
            
            static UTF8_Str toStr(const int32_t n, const uint8_t base);
            static UTF8_Str toStr(const uint32_t n, const uint8_t base);
            static UTF8_Str toStr(const int64_t n, const uint8_t base);
            static UTF8_Str toStr(const uint64_t n, const uint8_t base);
            static UTF8_Str toStr(const float n, const uint8_t base);
            static UTF8_Str toStr(const double n, const uint8_t base);
            static UTF8_Str toStr(const bool n, const uint8_t base);
            
            static int32_t strToInt(const char* str);
            static uint32_t strToUInt(const char* str);
            static int64_t strToLong(const char* str);
            static uint64_t strToULong(const char* str);
            static float strToFloat(const char* str);
            static double strToDouble(const char* str);
            static bool strToBool(const char* str); 
    };
}
