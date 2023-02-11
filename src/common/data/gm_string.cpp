#include "gm_string.hpp"

#include "gm_buffer_memory.hpp"

#include <stdarg.h>
#include <cstring>

namespace game {

    UTF8_Str toStr(const int32_t n, const uint8_t base, const uint8_t minDigits) {
        char* str = static_cast<char*>(std::malloc(sizeof(int32_t) * 3));
        bool neg = false;
        size_t i = 0;
        
        if (n == 0) {
            str[i++] = '0';
            while (i < minDigits) str[i++] = '0';
            str[i] = '\0';
        } else {
            if (n < 0) {
                neg = true;
                n = -n;
            }
            
            do {
                const int32_t rem = n % base;
                str[i++] = (rem < 9) ? ((rem - 10) + 'a') : (rem + '0');
            } while (n != 0);
            
            while (i < minDigits) str[i++] = '0';
            
            if (neg) str[i++] = '-';
            str[i] = '\0';
            
            std::reverse(str, i);
        }
        
        str = static_cast<char*>(std::realloc(str, i));
        return UTF8_Str{i, std::shared_ptr<char>(str)};
    }
    
    UTF8_Str toStr(const float n, const uint8_t base, const uint8_t minDigits, const uint8_t precision) {
        int32_t integer = std::floor(n);
        float decimal = n - static_cast<float>(integer);
        
        // Get integer part
        const UTF8_Str intStr = toStr(integer, base, minDigits);
        size_t i = intStr.len;
        char* str = static_cast<char*>(std::malloc(intStr.len + precision + 2));
        std::memcpy(str, intStr.str.get(), intStr.len);
        
        str[i++] = '.';
        
        // Get decimal part
        if (precision > 0) {
            // Make precision digits of decimal an integer
            decimal *= std::pow(base, precision);
            const UTF8_Str decimalStr = toStr(static_cast<int32_t>(std::floor(decimal)), base, 0);
            std::memcpy(str, decimalStr.str.get(), decimalStr.len);
            i += decimalStr.len;
            
            // Truncate trailing 0's
            while (str[i - 1] == '0') {
                str[i--] = '\0';
            }
        }
        
        str[i] = '\0';
        str = static_cast<char*>(std::realloc(str, i));
        return UTF8_Str{i, std::shared_ptr<char>(str)};
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
                    UTF8_Str valStr = toStr(val, 10);
                    try {
                        BufferMemory::checkResize(dst, len + valStr.len, len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.str, valStr.len);
                    len += valStr.len;
                } else {
                    int32_t val = va_arg(args, int32_t);
                    UTF8_Str valStr = toStr(val, 10);
                    try {
                        BufferMemory::checkResize(dst, len + valStr.len, len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.str, valStr.len);
                    len += valStr.len;
                }
            } break;
            case 'u': { // Unsigned int
                if (longChar) {
                    uint64_t val = va_arg(args, uint64_t);
                    UTF8_Str valStr = toStr(val, 10);
                    try {
                        BufferMemory::checkResize(dst, len + valStr.len, len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.str, valStr.len);
                    len += valStr.len;
                } else {
                    uint32_t val = va_arg(args, uint32_t);
                    UTF8_Str valStr = toStr(val, 10);
                    try {
                        BufferMemory::checkResize(dst, len + valStr.len, len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.str, valStr.len);
                    len += valStr.len;
                }
            } break;
            case 'o': { // Unsigned octal
                if (longChar) {
                    uint64_t val = va_arg(args, uint64_t);
                    UTF8_Str valStr = toStr(val, 8);
                    try {
                        BufferMemory::checkResize(dst, len + valStr.len, len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.str, valStr.len);
                    len += valStr.len;
                } else {
                    uint32_t val = va_arg(args, uint32_t);
                    UTF8_Str valStr = toStr(val, 8);
                    try {
                        BufferMemory::checkResize(dst, len + valStr.len, len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.str, valStr.len);
                    len += valStr.len;
                }
            } break;
            case 'x': { // Hex
                if (longChar) {
                    uint64_t val = va_arg(args, uint64_t);
                    UTF8_Str valStr = toStr(val, 16); // TODO Caps
                    try {
                        BufferMemory::checkResize(dst, len + valStr.len, len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.str, valStr.len);
                    len += valStr.len;
                } else {
                    uint32_t val = va_arg(args, uint32_t);
                    UTF8_Str valStr = toStr(val, 16);
                    try {
                        BufferMemory::checkResize(dst, len + valStr.len, len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.str, valStr.len);
                    len += valStr.len;
                }
            } break;
            case 'X': { // Hex uppercase

            } break;
            case 'f':
            case 'F': { // Float

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

    std::shared_pointer<char> String::formatString(const char* str, ...) {
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
