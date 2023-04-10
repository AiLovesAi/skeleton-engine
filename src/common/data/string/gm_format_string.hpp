#pragma once

#include "gm_utf8.hpp"

#include "../file/gm_logger.hpp"
#include "../../headers/float.hpp"

#include <cmath>
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
            static UTF8Str formatString(const char *__restrict__ str, ...) noexcept;
            
            template <typename T>
            static inline T strToInt(const char *__restrict__ str) { return strToInt<T>(str, 10); }
            template <typename T>
            static inline T strToFloat(const char *__restrict__ str) { return strToFloat<T>(str, 10); }
            static inline bool strToBool(const char *__restrict__ str) { return strToBool(str, std::strlen(str)); }

            template <typename T>
            static inline T strToInt(const char *__restrict__ str, const uint8_t base) {
                return strToInt<T>(str, base, std::strlen(str));
            }
            template <typename T>
            static inline T strToFloat(const char *__restrict__ str, const uint8_t base) {
                return strToFloat<T>(str, base, std::strlen(str));
            }
            
            template <typename T>
            static T strToInt(const char *__restrict__ str, const uint8_t base, const int64_t len);
            template <typename T>
            static T strToFloat(const char *__restrict__ str, const uint8_t base, const int64_t len);
            static bool strToBool(const char *__restrict__ str, const int64_t len);
            
            template <typename T>
            static inline UTF8Str intToStr(T val) { return intToStr(val, 10); }
            template <typename T>
            static inline UTF8Str floatToStr(T val) { return floatToStr(val, 10); }
            static inline UTF8Str boolToStr(const bool boolean) { return boolToStr(boolean, UPPERCASE_FIRST); }
            static UTF8Str boolToStr(const bool boolean, const StringCases caseType) noexcept;
            static inline UTF8Str ptrToStr(const void* ptr) { return ptrToStr(ptr, 0); }
            static UTF8Str ptrToStr(const void* ptr, const int32_t flags) noexcept;
            
            template <typename T>
            static inline UTF8Str intToStr(T val, uint8_t base) { return intToStr(val, base, 0); }
            template <typename T>
            static inline UTF8Str floatToStr(T val, uint8_t base) { return floatToStr(val, base, 0); }
            template <typename T>
            static inline UTF8Str floatToStr(T val, uint8_t base, int64_t precision) {
                return floatToStr(val, base, precision, 0);
            }

            template <typename T>
            static inline UTF8Str intToStr(T val, uint8_t base, int64_t minDigits);
            template <typename T>
            static inline UTF8Str floatToStr(T val, uint8_t base, int64_t precision, int64_t minDigits);

            // Variables
            static constexpr int MAX_DIGITS = 18; // 2^63 - 1 = 9.223372036854775807E18, so use 18 since we can hold 19 digits max

        private:
            // Types
            enum FormatFlags{
                // Numbers
                FORMAT_DIGITAL,
                FORMAT_SCIENTIFIC,

                // Flags
                // Hex/scientific notation is uppercase
                FORMAT_UPPERCASE = 1 << 31, 
                // If no sign is going to be writteval, a blank space is inserted before the value
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

                // Combinations
                FORMAT_SCIENTIFIC_UPPERCASE = FORMAT_SCIENTIFIC | FORMAT_UPPERCASE,
                FORMAT_SCIENTIFIC_LOWERCASE = FORMAT_SCIENTIFIC,
            };

            // Functions
            template <typename T>
            static T _strToInt(const char*__restrict__ str, const uint8_t base, const int64_t len);
            template <typename T>
            static T _strToFloat(const char*__restrict__ str, const uint8_t base, const int64_t len);

            template <typename T>
            static UTF8Str _intToStr(T val, uint8_t base, int64_t minDigits, const int32_t flags) noexcept;
            template <typename T>
            static UTF8Str _floatToStrScientific(const T absVal, const bool isNeg, const uint8_t base,
                const int64_t precision, const int64_t minDigits, const int32_t flags) noexcept;
            template <typename T>
            static UTF8Str _floatToStrDecimal(const T absVal, const bool isNeg, const uint8_t base,
                const int64_t precision, const int64_t minDigits, const int32_t flags) noexcept;
            template <typename T>
            static UTF8Str _floatToStr(T val, uint8_t base, int64_t precision, int64_t minDigits, const int32_t flags) noexcept;

            static bool _formatStringFormat(const char c, va_list& args,
                char*& dst, int64_t& capacity, int64_t& leval, int& flags,
                int64_t& minDigits, int64_t& precision); // Returns false when format is complete
    };

    // Specializations
    template<> int8_t FormatString::strToInt<int8_t>(const char *__restrict__ str, const uint8_t base, const int64_t len);
    template<> uint8_t FormatString::strToInt<uint8_t>(const char *__restrict__ str, const uint8_t base, const int64_t len);
    template<> int16_t FormatString::strToInt<int16_t>(const char *__restrict__ str, const uint8_t base, const int64_t len);
    template<> uint16_t FormatString::strToInt<uint16_t>(const char *__restrict__ str, const uint8_t base, const int64_t len);
    template<> int32_t FormatString::strToInt<int32_t>(const char *__restrict__ str, const uint8_t base, const int64_t len);
    template<> uint32_t FormatString::strToInt<uint32_t>(const char *__restrict__ str, const uint8_t base, const int64_t len);
    template<> int64_t FormatString::strToInt<int64_t>(const char *__restrict__ str, const uint8_t base, const int64_t len);
    template<> uint64_t FormatString::strToInt<uint64_t>(const char *__restrict__ str, const uint8_t base, const int64_t len);
    template<> float32_t FormatString::strToFloat<float32_t>(const char *__restrict__ str, const uint8_t base, const int64_t len);
    template<> float64_t FormatString::strToFloat<float64_t>(const char *__restrict__ str, const uint8_t base, const int64_t len);
    template<> float128_t FormatString::strToFloat<float128_t>(const char *__restrict__ str, const uint8_t base, const int64_t len);

    template<> UTF8Str FormatString::intToStr<int8_t>(int8_t val, uint8_t base, int64_t minDigits);
    template<> UTF8Str FormatString::intToStr<uint8_t>(uint8_t val, uint8_t base, int64_t minDigits);
    template<> UTF8Str FormatString::intToStr<int16_t>(int16_t val, uint8_t base, int64_t minDigits);
    template<> UTF8Str FormatString::intToStr<uint16_t>(uint16_t val, uint8_t base, int64_t minDigits);
    template<> UTF8Str FormatString::intToStr<int32_t>(int32_t val, uint8_t base, int64_t minDigits);
    template<> UTF8Str FormatString::intToStr<uint32_t>(uint32_t val, uint8_t base, int64_t minDigits);
    template<> UTF8Str FormatString::intToStr<int64_t>(int64_t val, uint8_t base, int64_t minDigits);
    template<> UTF8Str FormatString::intToStr<uint64_t>(uint64_t val, uint8_t base, int64_t minDigits);
    template<> UTF8Str FormatString::floatToStr<float32_t>(float32_t val, uint8_t base, int64_t precision, int64_t minDigits);
    template<> UTF8Str FormatString::floatToStr<float64_t>(float64_t val, uint8_t base, int64_t precision, int64_t minDigits);
    template<> UTF8Str FormatString::floatToStr<float128_t>(float128_t val, uint8_t base, int64_t precision, int64_t minDigits);
}
