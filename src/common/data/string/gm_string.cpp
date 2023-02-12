#include "gm_string.hpp"

#include <stdexcept>

namespace game {

    void String::reverse(char*& str, const int64_t len) {
        char temp;
        for (int64_t i = 0; i < len / 2; i++) {
            temp = str[i];
            str[i] = str[len - i - 1];
            str[len - i - 1] = temp;
        }
    }

    bool String::isAscii(const char* str, const int64_t len) {
        char c;
        for (int64_t i = 0; i < len; i++) {
            c = str[i];
            if (c < ' ' || c > '~') return false;
        }

        return true;
    }

    UTF8Str String::asAscii(const char* str, const int64_t len) {
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
