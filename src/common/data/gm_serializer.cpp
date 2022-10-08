#include "gm_serializer.hpp"

#include <algorithm>
#include <cstring>

namespace game {
    void Serializer::resizeBuffer(const size_t size) {
        buffer_ = static_cast<uint8_t*>(std::realloc(buffer_, size));
        capacity_ = size;
    }

    void Serializer::write(const void* data, const size_t size) {
        size_t resultHead = head_ + size;

        if (resultHead > capacity_) {
            resizeBuffer(std::max(capacity_ * 2, resultHead));
        }
        
        std::memcpy(buffer_ + head_, data, size);
        head_ = resultHead;
    }

    template<typename T>
    std::shared_ptr<uint8_t> Serializer::generate(const wchar_t* name, const uint8_t nameLen, T data) {
        static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value,
            "Generic generate() only supports primitive data types.");

        size_t c = 2 + (sizeof(wchar_t) * nameLen);
        uint8_t *buffer = new uint8_t[c + sizeof(T)];
        data = Endianness::hton(data);

        buffer[0] = BKV::BKV_I64;
        buffer[1] = nameLen;
        std::memcpy(buffer + 2, name, sizeof(wchar_t) * nameLen); // Copy name
        std::memcpy(buffer + c, &data, sizeof(T));

        return std::shared_ptr<uint8_t>(buffer, std::free);
    }
    std::shared_ptr<uint8_t> Serializer::generateSTR(const wchar_t* name, const uint8_t nameLen, const wchar_t* data, uint16_t len) {
        size_t c = 2 + (sizeof(wchar_t) * nameLen);
        uint8_t *buffer = new uint8_t[c + sizeof(uint16_t) + (sizeof(wchar_t) * len)];

        buffer[0] = BKV::BKV_STR;
        buffer[1] = nameLen;
        std::memcpy(buffer + 2, name, sizeof(wchar_t) * nameLen); // Copy name
        std::memcpy(buffer + c + sizeof(uint16_t), data, sizeof(wchar_t) * len); // Copy string after string length
        len = Endianness::hton(len);
        std::memcpy(buffer + c, &len, sizeof(uint16_t)); // Copy string length before string

        return std::shared_ptr<uint8_t>(buffer, std::free);
    }
}
