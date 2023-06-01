#include "gm_utf8.hpp"

namespace game {
    UTF8Str_::UTF8Str_(const char* str) {
        _len = std::strlen(str);
        char* copy = static_cast<char*>(std::malloc(_len + 1));
        std::memcpy(copy, str, _len);
        copy[_len] = '\0';
        _str = copy;
    }
    UTF8Str_::UTF8Str_(const char* const str, const int64_t len) : _len{len} {
        char* copy = static_cast<char*>(std::malloc(_len + 1));
        std::memcpy(copy, str, _len);
        copy[_len] = '\0';
        _str = copy;
    }
    UTF8Str_::UTF8Str_(const char* const str, const size_t len) {
        _len = static_cast<int64_t>(len);
        char* copy = static_cast<char*>(std::malloc(_len + 1));
        std::memcpy(copy, str, _len);
        copy[_len] = '\0';
        _str = copy;
    }
    UTF8Str_::UTF8Str_(const UTF8Str_& str) {
        _len = str.length();
        char* copy = static_cast<char*>(std::malloc(_len + 1));
        std::memcpy(copy, str.get(), _len);
        copy[_len] = '\0';
        _str = copy;
    }
    UTF8Str_::~UTF8Str_() {
        if (_str) std::free(_str);
    }
}
