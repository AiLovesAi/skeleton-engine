#include "gm_string.hpp"

#include <stdarg.h>
#include <cstring>

namespace game {
    void String::formatString(const char* str, ...) {
                va_list args;
                va_start(args, str);

                // TODO Create buffer
                size_t len = std::strlen(str);
                char c;
                bool formatChar = false;
                bool escapeChar = false;
                for (size_t i = 0; i < len; i++) {
                    c = str[i];
                    if (formatChar) { // TODO Inline function
                        switch (c) {
                            case 'd':
                            case 'i': { // Signed int
                                // TODO
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

                            } break;
                            case '%': { // '%'

                            } break;
                            default: { // Throw

                            }
                        }
                    } else if (escapeChar) {
                        escapeChar = false;
                    } else {
                        switch (c) {
                            case '\\': {
                                escapeChar = true;

                            } break;
                            case '%': {
                                formatChar = true;

                            } break;
                            default: { // Copy to str

                            } break;
                        }
                    }
                }
                // TODO

                va_end(args);
    }
}
