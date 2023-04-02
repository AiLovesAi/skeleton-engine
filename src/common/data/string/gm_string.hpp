#pragma once

#include "gm_format_string.hpp"
#include "gm_utf8.hpp"

#include <cstring>
#include <memory>

namespace game {
    class String {
        public:
            // Functions
            static inline void reverse(char*& str) { reverse(str, static_cast<int64_t>(std::strlen(str))); }
            static void reverse(char*& str, const int64_t len) noexcept;

            // NOTE: Must have enough memory
            static inline void insert(char*& str, const char c, const int64_t pos) {
                insert(str, static_cast<int64_t>(std::strlen(str)), c, pos);
            }
            static void insert(char*& str, const int64_t strLen, const char c, const int64_t pos) noexcept;
            // NOTE: Must have enough memory
            // @return Length of the new string
            static inline int64_t insert(char*& str, const char* insertStr, const int64_t pos) {
                return insert(
                    str, static_cast<int64_t>(std::strlen(str)),
                    insertStr, static_cast<int64_t>(std::strlen(insertStr)),
                    pos
                );
            }
            static int64_t insert(char*& str, const int64_t strLen, 
                const char* insertStr, const int64_t insertStrLen, const int64_t pos
            ) noexcept;

            // NOTE: Must have enough memory
            // @return Length of the new string
            static inline int64_t prepend(char*& str, const char* prependStr) { return insert(str, prependStr, 0); }
            static inline int64_t prepend(char*& str, const int64_t strLen, const char* prependStr, const int64_t prependStrLen) {
                return insert(str, strLen, prependStr, prependStrLen, 0);
            }

            static inline bool isAscii(const char* str) { return isAscii(str, std::strlen(str)); }
            static bool isAscii(const char* str, const int64_t len) noexcept;
            static inline UTF8Str asAscii(const char* str) { return asAscii(str, std::strlen(str)); }
            static UTF8Str asAscii(const char* str, const int64_t len) noexcept;

            /// @brief Checks if @p size fits in @p ptr, given its @p capacity.
            /// If there is not room in @p ptr, it is reallocated with double the capacity.
            /// @param ptr Pointer to check
            /// @param size Size that may overflow capacity
            /// @param prevSize Last size before current check for overflow checking
            /// @param capacity The current capacity of the buffer
            template <typename T1, typename T2>
            static void checkResize(T1*& ptr, const T2 size, const T2 prevSize, T2& capacity) {
                if (size < prevSize) {
                    throw std::runtime_error(
                        FormatString::formatString(
                            "New buffer size overflows to a number less than the previous: %ld < %d.", size, prevSize
                        ).get()
                    );
                }

                if (size > capacity) {
                    capacity <<= 1; // Capacity doubles
                    ptr = static_cast<T1*>(std::realloc(ptr, capacity));
                }
            }
        
        protected:
            friend class FormatString;

            // Functions
            template <typename T1, typename T2>
            static void checkResize_(T1*& ptr, const T2 size, const T2 prevSize, T2& capacity) {
                if (size < prevSize) {
                    throw std::runtime_error("New buffer size overflows to a number less than the previous in string format function.");
                }
                
                if (size > capacity) {
                    capacity <<= 1; // Capacity doubles
                    ptr = static_cast<T1*>(std::realloc(ptr, capacity));
                }
            }
    };
}
