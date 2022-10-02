#include "gm_serializer.hpp"

#include <algorithm>
#include <cstring>

namespace game {
    void Serializer::resizeBuffer(const size_t size) {
        buffer_ = static_cast<char*>(std::realloc(buffer_, size));
        capacity_ = size;
    }

    void Serializer::write(const void* data, const size_t size) {
        // TODO Translate to network endianness
        size_t resultHead = head_ + size;

        if (resultHead > capacity_) {
            resizeBuffer(std::max(capacity_ * 2, resultHead));
        }
        
        // TODO Write bits
        std::memcpy(buffer_ + head_, data, size);
        head_ = resultHead;
    }
}
