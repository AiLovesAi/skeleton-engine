#include "gm_string.hpp"

#include "gm_buffer_memory.hpp"

#include <stdarg.h>
#include <cstring>

namespace game {
    inline static void formatStringFormat(const char c, va_list& args,
        char*& dst, size_t& capacity, size_t& len, bool& longChar)
    {
        switch (c) {
            case 'l':
            case 'L': {
                longChar = true;
            } break;
            case 'd':
            case 'i': { // Signed int
                if (longChar) {
                    long val = va_arg(args, long);
                    char* valStr = std::ltoa(val);
                    // TODO
                } else {
                    int val = va_arg(args, int);
                    char* valStr = std::itoa(val);
                    // TODO
                }
            } break;
            case 'u': { // Unsigned int

            } break;
            case 'o': { // Unsigned octal

            } break;
            case 'x': { // Hex

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
