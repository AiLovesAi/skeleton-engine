#pragma once

#include <cstdint>
#include <memory>

namespace game {
    typedef struct UTF8Str_ {
        private:
            int64_t len;
            std::shared_ptr<const char> str;
    
        public:
            UTF8Str_() : len{0}, str{nullptr} {}
            UTF8Str_(const int64_t len, const std::shared_ptr<const char> str) :
                len{len}, str{str} {}

            inline const char* get() const {
                return str.get();
            }

            inline int64_t length() const {
                return len;
            }
    } UTF8Str;
}
