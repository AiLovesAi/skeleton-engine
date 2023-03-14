#include "gm_string.hpp"

#include <stdexcept>

namespace game {

    void String::reverse(char*& str, const int64_t len) noexcept {
        char temp;
        for (int64_t i = 0; i < len / 2; i++) {
            temp = str[i];
            str[i] = str[len - i - 1];
            str[len - i - 1] = temp;
        }
    }

    void String::insert(char*& str, const int64_t strLen, const char c, const int64_t pos) noexcept {
        // Copy character to be replaced
        char temp = str[pos];

        // Insert at pos
        str[pos] = c;

        // Copy remaining characters
        char temp2;
        for (int64_t i = pos + 1; i < strLen + 1; i++) {
            temp2 = str[i];
            str[i] = temp;
            temp = temp2;
        }

        str[strLen + 2] = '\0';
    }

    int64_t String::insert(char*& str, const int64_t strLen, const char*& insertStr, const int64_t insertStrLen, const int64_t pos) noexcept {
        // Copy str to buffer after pos
        const int64_t rem = strLen - pos;
        char* buffer = static_cast<char*>(std::malloc(rem));
        std::memcpy(buffer, str + pos, rem);

        // Insert at pos
        int64_t len = pos;
        std::memcpy(str + len, insertStr, insertStrLen);
        len += insertStrLen;

        // Copy remaining characters
        std::memcpy(str + len, buffer, rem);
        len += rem;

        std::free(buffer);
        str[strLen + 2] = '\0';
        return len;
    }

    bool String::isAscii(const char* str, const int64_t len) noexcept {
        char c;
        for (int64_t i = 0; i < len; i++) {
            c = str[i];
            if (c < ' ' || c > '~') return false;
        }

        return true;
    }

    UTF8Str String::asAscii(const char* str, const int64_t len) noexcept {
        char* dst = static_cast<char*>(std::malloc(len + 1));

        char c;
        int64_t i = 0;
        for (; i < len; i++) {
            c = str[i];
            if (c < ' ' || c > '~') break;
            dst[i] = c;
        }

        return UTF8Str{len, std::shared_ptr<const char>(dst, std::free)};
    }
}
