#pragma once

#include "gm_utf8.hpp"

#include <cstring>
#include <memory>

namespace game {
    class FormatString {
        public:
            // Types
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
            static inline UTF8Str intToStr(T val) { return intToStr(n, 10); }
            template <typename T>
            static inline UTF8Str floatToStr(T val) { return floatToStr(n, 10); }
            static inline UTF8Str boolToStr(const bool b) { return boolToStr(b, UPPERCASE_FIRST); }
            static UTF8Str boolToStr(const bool b, const StringCases caseType) noexcept;
            static UTF8Str ptrToStr(const void* ptr) { return ptrToStr(ptr, 0); }
            static UTF8Str ptrToStr(const void* ptr, const int32_t flags) noexcept;
            
            template <typename T>
            static inline UTF8Str intToStr(T val, uint8_t base) { return intToStr(n, base, 0); }
            template <typename T>
            static inline UTF8Str floatToStr(T val, uint8_t base) { return floatToStr(n, base, 0); }

            template <typename T>
            static inline UTF8Str intToStr(T val, uint8_t base, size_t minDigits) { return intToStr(n, base, minDigits, FORMAT_DIGITAL); }
            template <typename T>
            static inline UTF8Str floatToStr(T val, uint8_t base, size_t precision) { return floatToStr(n, base, precision, 0); }
            template <typename T>
            static inline UTF8Str floatToStr(T val, uint8_t base, size_t precision, size_t minDigits) { return floatToStr(n, base, precision, minDigits, FORMAT_DIGITAL); }

            // Variables
            static constexpr int MAX_DIGITS = 18; // 2^63 - 1 = 9.223372036854775807E18, so use 18 since we can hold 19 digits max

        private:
            // Types
            enum FormatFlags{
                // Numbers
                FORMAT_DIGITAL,
                FORMAT_SCIENTIFIC,

                // Flags
                FORMAT_UPPERCASE = 1 << 7, // Hex/scientific notation is uppercase

                // Combinations
                FORMAT_SCIENTIFIC_UPPERCASE = FORMAT_SCIENTIFIC | FORMAT_UPPERCASE,
                FORMAT_SCIENTIFIC_LOWERCASE = FORMAT_SCIENTIFIC,

                // Flags
                // Hex/scientific notation is uppercase
                FORMAT_UPPERCASE = 1 << 31, 
                // If no sign is going to be written, a blank space is inserted before the value
                FORMAT_SIGNED = 1 << 30,
                // Forces sign (even on positive)
                FORMAT_SIGN_PADDING = 1 << 29,
                // When padded with spaces, they will be appended to the value instead of prepended
                FORMAT_LEFT_JUSTIFIED = 1 << 28,
                // Places a '0' before octal and '0x' before hex/pointers, and a decimal point following integral floating types
                FORMAT_TAGGED = 1 << 27,
                // Left-pads the number with zeroes (0) instead of spaces when padding is specified
                FORMAT_ZERO_PADDED = 1 << 26,
                // Size is 64-bit long
                FORMAT_LONG = 1 << 25,
                // The following width is now for precision as opposed to minimum integer digits
                FORMAT_PRECISION = 1 << 24,
                // Truncates trailing zeroes in a float
                FORMAT_TRUNCATE_ZEROES = 1 << 23,
            };

            // Functions
            template <typename T>
            static UTF8Str _intToStr(T val, uint8_t base, size_t minDigits, const int32_t flags) noexcept;
            template <typename T>
            static UTF8Str _floatToStrScientific(const T absVal, const bool isNeg, const uint8_t base,
                const size_t precision, const size_t minDigits, const int32_t flags) noexcept;
            template <typename T>
            static UTF8Str _floatToStrDecimal(const T absVal, const bool isNeg, const uint8_t base,
                const size_t precision, const size_t minDigits, const int32_t flags) noexcept;
            template <typename T>
            static UTF8Str _floatToStr(T val, uint8_t base, size_t precision, size_t minDigits, const int32_t flags) noexcept;

            static bool _formatStringFormat(const char c, va_list& args,
                char*& dst, int64_t& capacity, int64_t& len, int& flags,
                size_t& minDigits, size_t& precision); // Returns false when format is complete
    };
}
