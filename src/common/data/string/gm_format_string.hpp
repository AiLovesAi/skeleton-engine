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
            static UTF8Str formatString(const char *__restrict__ str, ...) noexcept;
            
            static inline int32_t strToInt(const char *__restrict__ str) { return strToInt(str, std::strlen(str)); }
            static inline uint32_t strToUInt(const char *__restrict__ str) { return strToUInt(str, std::strlen(str)); }
            static inline int64_t strToLong(const char *__restrict__ str) { return strToLong(str, std::strlen(str)); }
            static inline uint64_t strToULong(const char *__restrict__ str) { return strToULong(str, std::strlen(str)); }
            static inline float strToFloat(const char *__restrict__ str) { return strToFloat(str, std::strlen(str)); }
            static inline double strToDouble(const char *__restrict__ str) { return strToDouble(str, std::strlen(str)); }
            static inline bool strToBool(const char *__restrict__ str) { return strToBool(str, std::strlen(str)); }

            static inline int32_t strToInt(const char *__restrict__ str, const int64_t len) {
                return strToInt(str, len, 10);
            }
            static inline uint32_t strToUInt(const char *__restrict__ str, const int64_t len) {
                return strToUInt(str, len, 10);
            }
            static inline int64_t strToLong(const char *__restrict__ str, const int64_t len) {
                return strToLong(str, len, 10);
            }
            static inline uint64_t strToULong(const char *__restrict__ str, const int64_t len) {
                return strToULong(str, len, 10);
            }
            static inline float strToFloat(const char *__restrict__ str, const int64_t len) {
                return strToFloat(str, len, 10);
            }
            static inline double strToDouble(const char *__restrict__ str, const int64_t len) {
                return strToDouble(str, len, 10);
            }

            static inline int32_t strToInt(const char *__restrict__ str, const int64_t len, const uint8_t base) {
                int32_t result;
                try { _strToInt(str, len, base, result); } catch (std::runtime_error& e) { throw; }

                // Check for negative values
                if (str[0] == '-') result = -result;

                return result;
            }
            static inline uint32_t strToUInt(const char *__restrict__ str, const int64_t len, const uint8_t base) {
                uint32_t result;
                try { _strToInt(str, len, base, result); } catch (std::runtime_error& e) { throw; }
                return result;
            }
            static inline int64_t strToLong(const char *__restrict__ str, const int64_t len, const uint8_t base) {
                int64_t result;
                try { _strToInt(str, len, base, result); } catch (std::runtime_error& e) { throw; }

                // Check for negative values
                if (str[0] == '-') result = -result;

                return result;
            }
            static inline uint64_t strToULong(const char *__restrict__ str, const int64_t len, const uint8_t base) {
                uint64_t result;
                try { _strToInt(str, len, base, result); } catch (std::runtime_error& e) { throw; }
                return result;
            }
            static inline float strToFloat(const char *__restrict__ str, const int64_t len, const uint8_t base) {
                float result;
                try { _strToFloat(str, len, base, result); } catch (std::runtime_error& e) { throw; }
                return result;
            }
            static inline double strToDouble(const char *__restrict__ str, const int64_t len, const uint8_t base) {
                double result;
                try { _strToFloat(str, len, base, result); } catch (std::runtime_error& e) { throw; }
                return result;
            }
            static bool strToBool(const char *__restrict__ str, const int64_t len);
            
            template <typename T>
            static inline UTF8Str intToStr(T val) { return intToStr(val, 10); }
            template <typename T>
            static inline UTF8Str floatToStr(T val) { return floatToStr(val, 10); }
            static inline UTF8Str boolToStr(const bool boolean) { return boolToStr(boolean, UPPERCASE_FIRST); }
            static UTF8Str boolToStr(const bool boolean, const StringCases caseType) noexcept;
            static UTF8Str ptrToStr(const void* ptr) { return ptrToStr(ptr, 0); }
            static UTF8Str ptrToStr(const void* ptr, const int32_t flags) noexcept;
            
            template <typename T>
            static inline UTF8Str intToStr(T val, uint8_t base) { return intToStr(val, base, 0); }
            template <typename T>
            static inline UTF8Str floatToStr(T val, uint8_t base) { return floatToStr(val, base, 0); }

            template <typename T>
            static inline UTF8Str intToStr(T val, uint8_t base, int64_t minDigits) {
                return intToStr(val, base, minDigits, FORMAT_DIGITAL);
            }
            template <typename T>
            static inline UTF8Str floatToStr(T val, uint8_t base, int64_t precision) {
                return floatToStr(val, base, precision, 0);
            }
            template <typename T>
            static inline UTF8Str floatToStr(T val, uint8_t base, int64_t precision, int64_t minDigits) {
                return floatToStr(val, base, precision, minDigits, FORMAT_DIGITAL);
            }

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
            static void _strToInt(const char*__restrict__ str, const int64_t len, const uint8_t base, T& result);
            template <typename T>
            static void _strToFloat(const char*__restrict__ str, const int64_t len, const uint8_t base, T& result);

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
}
