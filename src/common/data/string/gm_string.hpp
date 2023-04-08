#pragma once

#include "gm_format_string.hpp"
#include "gm_utf8.hpp"

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
            static inline int64_t insert(char*& str, const char*__restrict__ insertStr, const int64_t pos) {
                return insert(
                    str, static_cast<int64_t>(std::strlen(str)),
                    insertStr, static_cast<int64_t>(std::strlen(insertStr)),
                    pos
                );
            }
            static int64_t insert(char*& str, const int64_t strLen, 
                const char*__restrict__ insertStr, const int64_t insertStrLen, const int64_t pos
            ) noexcept;

            // NOTE: Must have enough memory
            // @return Length of the new string
            static inline int64_t prepend(char*& str, const char*__restrict__ prependStr) { return insert(str, prependStr, 0); }
            static inline int64_t prepend(char*& str, const int64_t strLen, const char*__restrict__ prependStr, const int64_t prependStrLen) {
                return insert(str, strLen, prependStr, prependStrLen, 0);
            }

            static inline bool isAscii(const char*__restrict__ str) { return isAscii(str, std::strlen(str)); }
            static bool isAscii(const char*__restrict__ str, const int64_t len) noexcept;
            static inline UTF8Str asAscii(const char*__restrict__ str) { return asAscii(str, std::strlen(str)); }
            static UTF8Str asAscii(const char*__restrict__ str, const int64_t len) noexcept;

            // Translates escape character into its ASCII counterpart
            static char escapeChar(const char c) noexcept;
    };
}
