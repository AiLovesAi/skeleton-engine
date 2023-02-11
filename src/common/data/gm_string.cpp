#include "gm_string.hpp"

#include "gm_buffer_memory.hpp"

#include <algorithm>
#include <cstring>
#include <math.h>
#include <stdarg.h>

namespace game {

    void String::reverse(char*& str, const int64_t len) {
        char temp;
        for (int64_t i = 0; i < len / 2; i++) {
            temp = str[i];
            str[i] = str[len - i - 1];
            str[len - i - 1] = temp;
        }
    }

    UTF8Str String::toStr(int32_t n, const uint8_t base, const uint8_t minDigits, const bool uppercase) {
        char* str = static_cast<char*>(std::malloc(sizeof(int32_t) * 3));
        bool neg = false;
        int64_t i = 0;
        
        if (n == 0) {
            str[i++] = '0';
            while (i < minDigits) str[i++] = '0';
            str[i] = '\0';
        } else {
            if (n < 0) {
                neg = true;
                n = -n;
            }
            
            // Write each digit
            do {
                const int32_t rem = n % base;
                if (base <= 10) {
                    str[i++] = (rem < 9) ? ((rem - 10) + 'a') : (rem + '0'); // TODO Test with bases other than 10
                } else {
                    // TODO Hex
                } 
            } while (n != 0);
            
            while (i < minDigits) str[i++] = '0';
            
            if (neg) str[i++] = '-';
            str[i] = '\0';
            
            reverse(str, i);
        }
        
        str = static_cast<char*>(std::realloc(str, i));
        return UTF8Str{i, std::shared_ptr<char>(str, std::free)};
    }

    UTF8Str String::toStr(uint32_t n, const uint8_t base, const uint8_t minDigits, const bool uppercase) { return UTF8Str{}; }
    UTF8Str String::toStr(int64_t n, const uint8_t base, const uint8_t minDigits, const bool uppercase) { return UTF8Str{}; }
    UTF8Str String::toStr(uint64_t n, const uint8_t base, const uint8_t minDigits, const bool uppercase) { return UTF8Str{}; }
    
    UTF8Str String::toStr(double n, const uint8_t base, uint8_t precision, const uint8_t minDigits, const bool uppercase) {
        int32_t integer = std::floor(n);
        double decimal = n - static_cast<double>(integer);
        
        // Get integer part
        const UTF8Str intStr = toStr(integer, base, minDigits);
        int64_t i = intStr.len;
        char* str = static_cast<char*>(std::malloc(intStr.len + precision + 2));
        std::memcpy(str, intStr.str.get(), intStr.len);
        
        str[i++] = '.';
        
        if (precision > 15) precision = 15;
        
        // Get decimal part
        if (precision > 0) {
            // Make precision digits of decimal an integer
            decimal *= std::pow(base, precision);
            const UTF8Str decimalStr = toStr(static_cast<int32_t>(std::floor(decimal)), base, 0);
            std::memcpy(str, decimalStr.str.get(), decimalStr.len);
            i += decimalStr.len;
            
            // Truncate trailing 0's
            while (str[i - 1] == '0') {
                str[i--] = '\0';
            }
        }
        
        str[i] = '\0';
        str = static_cast<char*>(std::realloc(str, i));
        return UTF8Str{i, std::shared_ptr<char>(str, std::free)};
    }

    UTF8Str String::toStr(const bool b, const StringCases caseType) {
        char* str = static_cast<char*>(std::malloc(sizeof("False")));
        int64_t len;

        switch (caseType) {
            case LOWERCASE:
                if (b) {
                    len = sizeof("true") - 1;
                    std::memcpy(str, "true", len);
                } else {
                    len = sizeof("false") - 1;
                    std::memcpy(str, "false", len);
                }
            break;
            case UPPERCASE_ALL:
                if (b) {
                    len = sizeof("TRUE") - 1;
                    std::memcpy(str, "TRUE", len);
                } else {
                    len = sizeof("FALSE") - 1;
                    std::memcpy(str, "FALSE", len);
                }
            break;
            case UPPERCASE_FIRST:
            default:
                if (b) {
                    len = sizeof("True") - 1;
                    std::memcpy(str, "True", len);
                } else {
                    len = sizeof("False") - 1;
                    std::memcpy(str, "False", len);
                }
            break;
        }

        str[len] = '\0';
        str = static_cast<char*>(std::realloc(str, len + 1));
        return UTF8Str{len, std::shared_ptr<char>(str, std::free)};
    }
    
    inline static void formatStringFormat(const char c, va_list& args,
        char*& dst, size_t& capacity, size_t& len, bool& longChar)
    {
        switch (c) { // TODO %02lu
            case 'l':
            case 'L': {
                longChar = true;
            } break;
            case 'd':
            case 'i': { // Signed int
                if (longChar) {
                    int64_t val = va_arg(args, int64_t);
                    UTF8Str valStr = String::toStr(val, 10);
                    try {
                        BufferMemory::checkResize(dst, len + valStr.len, len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.str.get(), valStr.len);
                    len += valStr.len;
                } else {
                    int32_t val = va_arg(args, int32_t);
                    UTF8Str valStr = String::toStr(val, 10);
                    try {
                        BufferMemory::checkResize(dst, len + valStr.len, len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.str.get(), valStr.len);
                    len += valStr.len;
                }
            } break;
            case 'u': { // Unsigned int
                if (longChar) {
                    uint64_t val = va_arg(args, uint64_t);
                    UTF8Str valStr = String::toStr(val, 10);
                    try {
                        BufferMemory::checkResize(dst, len + valStr.len, len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.str.get(), valStr.len);
                    len += valStr.len;
                } else {
                    uint32_t val = va_arg(args, uint32_t);
                    UTF8Str valStr = String::toStr(val, 10);
                    try {
                        BufferMemory::checkResize(dst, len + valStr.len, len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.str.get(), valStr.len);
                    len += valStr.len;
                }
            } break;
            case 'o': { // Unsigned octal
                if (longChar) {
                    uint64_t val = va_arg(args, uint64_t);
                    UTF8Str valStr = String::toStr(val, 8);
                    try {
                        BufferMemory::checkResize(dst, len + valStr.len, len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.str.get(), valStr.len);
                    len += valStr.len;
                } else {
                    uint32_t val = va_arg(args, uint32_t);
                    UTF8Str valStr = String::toStr(val, 8);
                    try {
                        BufferMemory::checkResize(dst, len + valStr.len, len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.str.get(), valStr.len);
                    len += valStr.len;
                }
            } break;
            case 'x': { // Hex
                if (longChar) {
                    uint64_t val = va_arg(args, uint64_t);
                    UTF8Str valStr = String::toStr(val, 16, 0, false);
                    try {
                        BufferMemory::checkResize(dst, len + valStr.len, len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.str.get(), valStr.len);
                    len += valStr.len;
                } else {
                    uint32_t val = va_arg(args, uint32_t);
                    UTF8Str valStr = String::toStr(val, 16, 0, false);
                    try {
                        BufferMemory::checkResize(dst, len + valStr.len, len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.str.get(), valStr.len);
                    len += valStr.len;
                }
            } break;
            case 'X': { // Hex uppercase
                if (longChar) {
                    uint64_t val = va_arg(args, uint64_t);
                    UTF8Str valStr = String::toStr(val, 16, 0, true);
                    try {
                        BufferMemory::checkResize(dst, len + valStr.len, len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.str.get(), valStr.len);
                    len += valStr.len;
                } else {
                    uint32_t val = va_arg(args, uint32_t);
                    UTF8Str valStr = String::toStr(val, 16, 0, true);
                    try {
                        BufferMemory::checkResize(dst, len + valStr.len, len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.str.get(), valStr.len);
                    len += valStr.len;
                }
            } break;
            case 'f':
            case 'F': { // Float
                double val = va_arg(args, double);
                UTF8Str valStr = String::toStr(val, 16, 0, 0, false);
                try {
                    BufferMemory::checkResize(dst, len + valStr.len, len, capacity);
                } catch (std::runtime_error& e) { throw; }
                std::memcpy(dst + len, valStr.str.get(), valStr.len);
                len += valStr.len; 
            } break;
            case 'e': { // Scientific notation

            } break;
            case 'E': { // Scientific notation uppercase

            } break;
            case 'g': { // Shortest of %f or %e

            } break;
            case 'G': { // Shortest of %F or %E

            } break;
            case 'a': { // Signed hex float

            } break;
            case 'A': { // Signed hex float uppercase

            } break;
            case 'c': { // Character

            } break;
            case 's': { // String
                const char* str = va_arg(args, char*);
                int64_t strLen = std::strlen(str);
                
                try {
                    BufferMemory::checkResize(dst, len + strLen, len, capacity);
                } catch (std::runtime_error& e) { throw; }

                std::memcpy(dst, str, strLen);
                len += strLen;
            } break;
            case 'p': { // Pointer address

            } break;
            case 'n': { // Store currently written characters in variable (signed int); print nothing
                    size_t* storage = va_arg(args, size_t*);
                    if (storage) *storage = len;
            } break;
            case '%': { // '%'
                try {
                    BufferMemory::checkResize(dst, len + 1, len, capacity);
                } catch (std::runtime_error& e) { throw; }
                dst[len++] = '%';
            } break;
            default: { // Throw
                throw std::runtime_error("Unexpected format character in formatString().");
            }
        }
    }

    std::shared_ptr<char> String::formatString(const char* str, ...) {
                va_list args;
                va_start(args, str);

                const size_t strLen = std::strlen(str);
                size_t capacity = strLen + 1;
                size_t len = 0;
                char* dst = static_cast<char*>(std::malloc(capacity));

                char c;
                bool formatChar = false;
                bool longChar = false;
                for (size_t i = 0; i < strLen; i++) {
                    c = str[i];
                    if (formatChar) {
                        formatStringFormat(c, args, dst, capacity, len, longChar);
                    } else {
                        switch (c) {
                            case '%': { // Format
                                formatChar = true;
                            } break;
                            default: { // Copy to dst
                                try {
                                    BufferMemory::checkResize(dst, len + 1, len, capacity);
                                } catch (std::runtime_error& e) { throw; }
                                dst[len++] = c;
                            } break;
                        }
                    }
                }

                // TODO

                va_end(args);
                return std::shared_ptr<char>(dst, std::free);
    }
}
