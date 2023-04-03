#pragma once

#include <cstdint>
#include <memory>

namespace game {
    typedef struct UTF8Str_ {
        private:
            int64_t _len;
            std::shared_ptr<const char> _str;
    
        public:
            UTF8Str_() : _len{0}, _str{nullptr} {}
            UTF8Str_(const int64_t len, const std::shared_ptr<const char> str) :
                _len{len}, _str{str} {}

            inline const char* get() const {
                return _str.get();
            }

            inline int64_t length() const {
                return _len;
            }
    } UTF8Str;
}
