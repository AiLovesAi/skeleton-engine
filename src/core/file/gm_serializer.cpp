#include "gm_serializer.hpp"

#include <algorithm>

namespace game {
    void Serializer::writeBits(const uint8_t data, const size_t count) {
        size_t resultHead = head_ + count;

        if (resultHead > capacity_) {
            resizeBuffer(std::max((capacity_ >> 3) * 2, (resultHead >> 3) + !!(resultHead & 0x7)));
        }

        size_t byteOffset = head_ >> 3; // Number of fully written bytes
        uint8_t bitOffset = head_ & 0x7; // Bit remainder

        // Preserve remaining bits
        uint8_t mask = ~(0xff << bitOffset);
        buffer_[byteOffset] = (buffer_[byteOffset] & mask) | (data << bitOffset);

        uint8_t freeBits = 0x8 - bitOffset; // Bits remaining in current byte
        // Use another byte if needed
        if (freeBits < count) {
            buffer_[byteOffset + 1] = data >> freeBits;
        }

        head_ = resultHead;
    }

    void Serializer::writeBits(const void* data, size_t count) {
        const char* srcByte = static_cast<const char*>(data);

        // Write full bytes
        while (count > 8) {
            writeBits(*srcByte, 8);
            ++srcByte;
            count -= 8;
        }

        // Write remaining bits
        if (count > 0) {
            writeBits(*srcByte, count);
        }
    }
}
