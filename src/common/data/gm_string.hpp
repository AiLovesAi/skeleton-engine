#pragma once

#include "gm_utf8.hpp"

#include <memory>

namespace game {
    class String {
        public:
            // Types
            enum StringCases {
                LOWERCASE,
                UPPERCASE_FIRST,
                UPPERCASE_ALL,
            };

            // Functions
            static void reverse(char*& str, const int64_t len);

            static std::shared_ptr<char> formatString(const char* str, ...);
            // TODO General string functions that are currently implemented elsewhere (check File and BKV classes)
            
            static int32_t strToInt(const char* str);
            static uint32_t strToUInt(const char* str);
            static int64_t strToLong(const char* str);
            static uint64_t strToULong(const char* str);
            static float strToFloat(const char* str);
            static double strToDouble(const char* str);
            static bool strToBool(const char* str); 
            
            static inline UTF8Str toStr(int32_t n) { return toStr(n, 10); }
            static inline UTF8Str toStr(uint32_t n) { return toStr(n, 10); }
            static inline UTF8Str toStr(int64_t n) { return toStr(n, 10); }
            static inline UTF8Str toStr(uint64_t n) { return toStr(n, 10); }
            static inline UTF8Str toStr(float n) { return toStr(n, 10); }
            static inline UTF8Str toStr(double n) { return toStr(n, 10); }
            static inline UTF8Str toStr(const bool b) { return toStr(b, UPPERCASE_FIRST); }
            static UTF8Str toStr(const bool b, const StringCases caseType);
            
            static inline UTF8Str toStr(int32_t n, const uint8_t base) { return toStr(n, base, 0); }
            static inline UTF8Str toStr(uint32_t n, const uint8_t base) { return toStr(n, base, 0); }
            static inline UTF8Str toStr(int64_t n, const uint8_t base) { return toStr(n, base, 0); }
            static inline UTF8Str toStr(uint64_t n, const uint8_t base) { return toStr(n, base, 0); }
            static inline UTF8Str toStr(float n, const uint8_t base) { return toStr(n, base, 0); }
            static inline UTF8Str toStr(double n, const uint8_t base) { return toStr(n, base, 0); }
            
            static inline UTF8Str toStr(int32_t n, const uint8_t base, const uint8_t minDigits) {
                return toStr(n, base, minDigits, false);
            }
            static inline UTF8Str toStr(uint32_t n, const uint8_t base, const uint8_t minDigits) {
                return toStr(n, base, minDigits, false);
            }
            static inline UTF8Str toStr(int64_t n, const uint8_t base, const uint8_t minDigits) {
                return toStr(n, base, minDigits, false);
            }
            static inline UTF8Str toStr(uint64_t n, const uint8_t base, const uint8_t minDigits) {
                return toStr(n, base, minDigits, false);
            }
            static inline UTF8Str toStr(float n, const uint8_t base, uint8_t precision) {
                return toStr(n, base, precision, 0);
            }
            static inline UTF8Str toStr(double n, const uint8_t base, uint8_t precision) {
                return toStr(n, base, precision, 0);
            }
            static inline UTF8Str toStr(float n, const uint8_t base, uint8_t precision, const uint8_t minDigits) {
                return toStr(n, base, precision, minDigits, false);
            }
            static UTF8Str toStr(double n, uint8_t base, const uint8_t precision, const uint8_t minDigits) {
                return toStr(n, base, precision, minDigits, false);
            }
            
            static UTF8Str toStr(int32_t n, const uint8_t base, const uint8_t minDigits, const bool uppercase);
            static UTF8Str toStr(uint32_t n, const uint8_t base, const uint8_t minDigits, const bool uppercase);
            static UTF8Str toStr(int64_t n, const uint8_t base, const uint8_t minDigits, const bool uppercase);
            static UTF8Str toStr(uint64_t n, const uint8_t base, const uint8_t minDigits, const bool uppercase);
            static inline UTF8Str toStr(float n, const uint8_t base, uint8_t precision, const uint8_t minDigits, const bool uppercase) {
                return toStr(static_cast<double>(n), base, precision, minDigits, uppercase);
            } 
            static UTF8Str toStr(double n, const uint8_t base, uint8_t precision, const uint8_t minDigits, const bool uppercase);
    };
}
