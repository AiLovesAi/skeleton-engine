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
            static inline T strToUInt(const char *__restrict__ str) { return strToUInt<T>(str, 10); }
            template <typename T>
            static inline T strToFloat(const char *__restrict__ str) { return strToFloat<T>(str, 10); }
            static inline bool strToBool(const char *__restrict__ str) { return strToBool(str, std::strlen(str)); }

            template <typename T>
            static inline T strToInt(const char *__restrict__ str, const uint8_t base) {
                return strToInt<T>(str, base, std::strlen(str));
            }
            template <typename T>
            static inline T strToUInt(const char *__restrict__ str, const uint8_t base) {
                return strToUInt<T>(str, base, std::strlen(str));
            }
            template <typename T>
            static inline T strToFloat(const char *__restrict__ str, const uint8_t base) {
                return strToFloat<T>(str, base, std::strlen(str));
            }
            
            template <typename T>
            static T strToInt(const char *__restrict__ str, const uint8_t base, const int64_t len) {
                return Stringifier<T, sizeof(T)>().strToInt(str, base, len);
            }
            template <typename T>
            static T strToUInt(const char *__restrict__ str, const uint8_t base, const int64_t len) {
                return Stringifier<T, sizeof(T)>().strToUInt(str, base, len);
            }
            template <typename T>
            static T strToFloat(const char *__restrict__ str, const uint8_t base, const int64_t len) {
                return Stringifier<T, sizeof(T)>().strToFloat(str, base, len);
            }
            static bool strToBool(const char *__restrict__ str, const int64_t len);
            
            template <typename T>
            static inline UTF8Str intToStr(T val) { return intToStr(val, 10); }
            template <typename T>
            static inline UTF8Str uintToStr(T val) { return uintToStr(val, 10); }
            template <typename T>
            static inline UTF8Str floatToStr(T val) { return floatToStr(val, 10); }
            static inline UTF8Str boolToStr(const bool boolean) { return boolToStr(boolean, UPPERCASE_FIRST); }
            static UTF8Str boolToStr(const bool boolean, const StringCases caseType) noexcept;
            static inline UTF8Str ptrToStr(const void* ptr) { return ptrToStr(ptr, 0); }
            static UTF8Str ptrToStr(const void* ptr, const int32_t flags) noexcept;
            
            template <typename T>
            static inline UTF8Str intToStr(T val, uint8_t base) { return intToStr(val, base, 0); }
            template <typename T>
            static inline UTF8Str uintToStr(T val, uint8_t base) { return uintToStr(val, base, 0); }
            template <typename T>
            static inline UTF8Str floatToStr(T val, uint8_t base) { return floatToStr(val, base, 0); }
            template <typename T>
            static inline UTF8Str floatToStr(T val, uint8_t base, int64_t precision) {
                return floatToStr(val, base, precision, 0);
            }

            template <typename T>
            static inline UTF8Str intToStr(T val, uint8_t base, int64_t minDigits) {
                return Stringifier<T, sizeof(T)>().intToStr(val, base, minDigits);
            }
            template <typename T>
            static inline UTF8Str uintToStr(T val, uint8_t base, int64_t minDigits) {
                return Stringifier<T, sizeof(T)>().uintToStr(val, base, minDigits);
            }
            template <typename T>
            static inline UTF8Str floatToStr(T val, uint8_t base, int64_t precision, int64_t minDigits) {
                return Stringifier<T, sizeof(T)>().floatToStr(val, base, precision, minDigits);
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
            static T _strToInt(const char*__restrict__ str, const uint8_t base, const int64_t len);
            static int8_t _strToInt8(const char *__restrict__ str, const uint8_t base, const int64_t len);
            static uint8_t _strToIntU8(const char *__restrict__ str, const uint8_t base, const int64_t len);
            static int16_t _strToInt16(const char *__restrict__ str, const uint8_t base, const int64_t len);
            static uint16_t _strToIntU16(const char *__restrict__ str, const uint8_t base, const int64_t len);
            static int32_t _strToInt32(const char *__restrict__ str, const uint8_t base, const int64_t len);
            static uint32_t _strToIntU32(const char *__restrict__ str, const uint8_t base, const int64_t len);
            static int64_t _strToInt64(const char *__restrict__ str, const uint8_t base, const int64_t len);
            static uint64_t _strToIntU64(const char *__restrict__ str, const uint8_t base, const int64_t len);

            template <typename T>
            static T _strToFloat(const char*__restrict__ str, const uint8_t base, const int64_t len);
            static float32_t _strToFloat32(const char *__restrict__ str, const uint8_t base, const int64_t len);
            static float64_t _strToFloat64(const char *__restrict__ str, const uint8_t base, const int64_t len);
            static float128_t _strToFloat128(const char *__restrict__ str, const uint8_t base, const int64_t len);

            template <typename T>
            static UTF8Str _intToStr(T val, uint8_t base, int64_t minDigits, const int32_t flags) noexcept;
            static UTF8Str _intToStr8(int8_t val, uint8_t base, int64_t minDigits);
            static UTF8Str _intToStrU8(uint8_t val, uint8_t base, int64_t minDigits);
            static UTF8Str _intToStr16(int16_t val, uint8_t base, int64_t minDigits);
            static UTF8Str _intToStrU16(uint16_t val, uint8_t base, int64_t minDigits);
            static UTF8Str _intToStr32(int32_t val, uint8_t base, int64_t minDigits);
            static UTF8Str _intToStrU32(uint32_t val, uint8_t base, int64_t minDigits);
            static UTF8Str _intToStr64(int64_t val, uint8_t base, int64_t minDigits);
            static UTF8Str _intToStrU64(uint64_t val, uint8_t base, int64_t minDigits);

            template <typename T>
            static UTF8Str _floatToStrScientific(const T absVal, const bool isNeg, const uint8_t base,
                const int64_t precision, const int64_t minDigits, const int32_t flags) noexcept;
            template <typename T>
            static UTF8Str _floatToStrDecimal(const T absVal, const bool isNeg, const uint8_t base,
                const int64_t precision, const int64_t minDigits, const int32_t flags) noexcept;
            template <typename T>
            static UTF8Str _floatToStr(T val, uint8_t base, int64_t precision, int64_t minDigits, const int32_t flags) noexcept;
            static UTF8Str _floatToStr32(float32_t val, uint8_t base, int64_t precision, int64_t minDigits);
            static UTF8Str _floatToStr64(float64_t val, uint8_t base, int64_t precision, int64_t minDigits);
            static UTF8Str _floatToStr128(float128_t val, uint8_t base, int64_t precision, int64_t minDigits);

            static bool _formatStringFormat(const char c, va_list& args,
                char*& dst, int64_t& capacity, int64_t& leval, int& flags,
                int64_t& minDigits, int64_t& precision); // Returns false when format is complete
            
            // Classes
            template <typename T, size_t tSize> class Stringifier;
            template <typename T> class Stringifier<T,1> {
                public:
                    static T strToInt(const char*__restrict__ str, const uint8_t base, const int64_t len) {
                        return _strToInt8(str, base, len);
                    }
                    static T strToUInt(const char*__restrict__ str, const uint8_t base, const int64_t len) {
                        return _strToIntU8(str, base, len);
                    }
                    static UTF8Str intToStr(T val, uint8_t base, int64_t minDigits) {
                        return _intToStr8(val, base, minDigits);
                    }
                    static UTF8Str uintToStr(T val, uint8_t base, int64_t minDigits) {
                        return _intToStrU8(val, base, minDigits);
                    }
            };
            template <typename T> class Stringifier<T,2> {
                public:
                    static T strToInt(const char*__restrict__ str, const uint8_t base, const int64_t len) {
                        return _strToInt16(str, base, len);
                    }
                    static T strToUInt(const char*__restrict__ str, const uint8_t base, const int64_t len) {
                        return _strToIntU16(str, base, len);
                    }
                    static UTF8Str intToStr(T val, uint8_t base, int64_t minDigits) {
                        return _intToStr16(val, base, minDigits);
                    }
                    static UTF8Str uintToStr(T val, uint8_t base, int64_t minDigits) {
                        return _intToStrU16(val, base, minDigits);
                    }
            };
            template <typename T> class Stringifier<T,4> {
                public:
                    static T strToInt(const char*__restrict__ str, const uint8_t base, const int64_t len) {
                        return _strToInt32(str, base, len);
                    }
                    static T strToUInt(const char*__restrict__ str, const uint8_t base, const int64_t len) {
                        return _strToIntU32(str, base, len);
                    }
                    static UTF8Str intToStr(T val, uint8_t base, int64_t minDigits) {
                        return _intToStr32(val, base, minDigits);
                    }
                    static UTF8Str uintToStr(T val, uint8_t base, int64_t minDigits) {
                        return _intToStrU32(val, base, minDigits);
                    }
                    static T strToFloat(const char*__restrict__ str, const uint8_t base, const int64_t len) {
                        return _strToFloat32(str, base, len);
                    }
                    static UTF8Str floatToStr(T val, uint8_t base, int64_t precision, int64_t minDigits) {
                        return _floatToStr32(val, base, precision, minDigits);
                    }
            };
            template <typename T> class Stringifier<T,8> {
                public:
                    static T strToInt(const char*__restrict__ str, const uint8_t base, const int64_t len) {
                        return _strToInt64(str, base, len);
                    }
                    static T strToUInt(const char*__restrict__ str, const uint8_t base, const int64_t len) {
                        return _strToIntU64(str, base, len);
                    }
                    static UTF8Str intToStr(T val, uint8_t base, int64_t minDigits) {
                        return _intToStr64(val, base, minDigits);
                    }
                    static UTF8Str uintToStr(T val, uint8_t base, int64_t minDigits) {
                        return _intToStrU64(val, base, minDigits);
                    }
                    static T strToFloat(const char*__restrict__ str, const uint8_t base, const int64_t len) {
                        return _strToFloat64(str, base, len);
                    }
                    static UTF8Str floatToStr(T val, uint8_t base, int64_t precision, int64_t minDigits) {
                        return _floatToStr64(val, base, precision, minDigits);
                    }
            };
            template <typename T> class Stringifier<T,16> {
                public:
                    static T strToFloat(const char*__restrict__ str, const uint8_t base, const int64_t len) {
                        return _strToFloat128(str, base, len);
                    }
                    static UTF8Str floatToStr(T val, uint8_t base, int64_t precision, int64_t minDigits) {
                        return _floatToStr128(val, base, precision, minDigits);
                    }
            };
    };
}
