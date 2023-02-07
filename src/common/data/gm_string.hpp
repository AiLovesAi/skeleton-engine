#pragma once

#include <memory>

namespace game {
    class String {
        public:
            // Functions
            static std::shared_pointer<char> formatString(const char* str, ...);
            // TODO General string functions that are currently implemented elsewhere (check File and BKV classes)
            
            static std::shared_pointer<char> toStr(const int32_t n);
            static std::shared_pointer<char> toStr(const uint32_t n);
            static std::shared_pointer<char> toStr(const int64_t n);
            static std::shared_pointer<char> toStr(const uint64_t n);
            static std::shared_pointer<char> toStr(const float n);
            static std::shared_pointer<char> toStr(const double n);
            static std::shared_pointer<char> toStr(const bool n);
            
            static int32_t strToInt(const char* str);
            static uint32_t strToUInt(const char* str);
            static int64_t strToLong(const char* str);
            static uint64_t strToULong(const char* str);
            static float strToFloat(const char* str);
            static double strToDouble(const char* str);
            static bool strToBool(const char* str); 
    };
}
