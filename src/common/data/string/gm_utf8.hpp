#pragma once

#include <cstdint>
#include <memory>
#include <cstring>

namespace game {
    typedef struct UTF8Str_ {
        private:
            int64_t _len;
            char* _str;
    
        public:
            UTF8Str_() : _len{0}, _str{nullptr} {}
            UTF8Str_(const char* str);
            UTF8Str_(const char* const str, const int64_t len);
            UTF8Str_(const char* const str, const size_t len);
            UTF8Str_(const UTF8Str_& str);
            ~UTF8Str_();
            
            UTF8Str_& operator=(const UTF8Str_& str) {
                _len = str.length();
                char* copy = static_cast<char*>(std::malloc(_len + 1));
                std::memcpy(copy, str.get(), _len);
                copy[_len] = '\0';
                _str = copy;
                return *this;
            }

            inline const char* get() const { return _str; }

            inline int64_t length() const { return _len; }
    } UTF8Str;
    #define EMPTY_STR UTF8Str{"NULL", sizeof("NULL") - 1}
}
