#pragma once

#include "gm_utf8.hpp"

#include <cstring>
#include <memory>

namespace game {
    class FormatString {
        public:
            // Types
            enum NumberFormat {
                FORMAT_DIGITAL,
                FORMAT_HEXIDECIMAL,
                FORMAT_SCIENTIFIC,

                // Flags
                FORMAT_UPPERCASE = 1 << 7,

                // Combinations
                FORMAT_HEX_UPPERCASE = FORMAT_HEXIDECIMAL | FORMAT_UPPERCASE,
                FORMAT_HEX_LOWERCASE = FORMAT_HEXIDECIMAL,
                FORMAT_SCIENTIFIC_UPPERCASE = FORMAT_SCIENTIFIC | FORMAT_UPPERCASE,
                FORMAT_SCIENTIFIC_LOWERCASE = FORMAT_SCIENTIFIC,
            };

            enum StringCases {
                LOWERCASE,
                UPPERCASE_FIRST,
                UPPERCASE_ALL,
            };

            // Functions
            static UTF8Str formatString(const char* str, ...);
            // TODO General string functions that are currently implemented elsewhere (check File and BKV classes)
            
            static inline int32_t strToInt(const char* str) { return strToInt(str, std::strlen(str)); }
            static inline uint32_t strToUInt(const char* str) { return strToUInt(str, std::strlen(str)); }
            static inline int64_t strToLong(const char* str) { return strToLong(str, std::strlen(str)); }
            static inline uint64_t strToULong(const char* str) { return strToULong(str, std::strlen(str)); }
            static inline float strToFloat(const char* str) { return strToFloat(str, std::strlen(str)); }
            static inline double strToDouble(const char* str) { return strToDouble(str, std::strlen(str)); }
            static inline bool strToBool(const char* str) { return strToBool(str, std::strlen(str)); }

            static int32_t strToInt(const char* str, const int64_t len);
            static uint32_t strToUInt(const char* str, const int64_t len);
            static int64_t strToLong(const char* str, const int64_t len);
            static uint64_t strToULong(const char* str, const int64_t len);
            static float strToFloat(const char* str, const int64_t len);
            static double strToDouble(const char* str, const int64_t len);
            static bool strToBool(const char* str, const int64_t len);
            
            template <typename T>
            static inline UTF8Str toStr(T n) { return toStr(n, 10); }
            static inline UTF8Str toStr(float n) { return toStr(n, 10); }
            static inline UTF8Str toStr(double n) { return toStr(n, 10); }
            static inline UTF8Str toStr(const bool b) { return toStr(b, UPPERCASE_FIRST); }
            static UTF8Str toStr(const bool b, const StringCases caseType);
            
            template <typename T>
            static inline UTF8Str toStr(T n, uint8_t base) { return toStr(n, base, 0); }
            static inline UTF8Str toStr(float n, uint8_t base) { return toStr(n, base, 0); }
            static inline UTF8Str toStr(double n, uint8_t base) { return toStr(n, base, 0); }

            template <typename T>
            static inline UTF8Str toStr(T n, uint8_t base, const uint8_t minDigits) { return toStr(n, base, minDigits, FORMAT_DIGITAL); }
            static inline UTF8Str toStr(float n, uint8_t base, uint8_t precision) { return toStr(n, base, precision, 0); }
            static inline UTF8Str toStr(double n, uint8_t base, uint8_t precision) { return toStr(n, base, precision, 0); }
            static inline UTF8Str toStr(float n, uint8_t base, uint8_t precision, const uint8_t minDigits) { return toStr(n, base, precision, minDigits, FORMAT_DIGITAL); }
            static UTF8Str toStr(double n, uint8_t base, const uint8_t precision, const uint8_t minDigits) { return toStr(n, base, precision, minDigits, FORMAT_DIGITAL); }

            template <typename T>
            static UTF8Str toStr(T n, uint8_t base, const uint8_t minDigits, const NumberFormat numberFormat);
            static inline UTF8Str toStr(float n, uint8_t base, uint8_t precision, const uint8_t minDigits, const NumberFormat numberFormat) { return toStr(static_cast<double>(n), base, precision, minDigits, numberFormat); }
            static UTF8Str toStr(double n, uint8_t base, uint8_t precision, const uint8_t minDigits, const NumberFormat numberFormat);

        private:
            // Functions
            static void formatStringFormat(const char c, va_list& args,
                char*& dst, int64_t& capacity, int64_t& len, bool& longChar);
    };
}
