#pragma once

#include <cstdint>
#include <memory>

namespace game {
    typedef struct UTF8Str_ {
        int64_t len;
        std::shared_ptr<const char> str;
    } UTF8Str;
}
