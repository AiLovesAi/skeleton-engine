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

    std::shared_ptr<uint8_t> Serializer::generate(const wchar_t* name, const uint8_t nameLen, const uint8_t data) {
        size_t c = 2 + (sizeof(wchar_t) * nameLen);
        uint8_t *buffer = new uint8_t[c + sizeof(uint8_t)];

        buffer[0] = BKV::BKV_UINT8;
        buffer[1] = nameLen;
        std::memcpy(buffer + 2, name, sizeof(wchar_t) * nameLen); // Copy name
        buffer[c - 1] = data;

        return std::shared_ptr<uint8_t>(buffer, std::free);
    }
    std::shared_ptr<uint8_t> Serializer::generate(const wchar_t* name, const uint8_t nameLen, const int8_t data) {
        size_t c = 2 + (sizeof(wchar_t) * nameLen);
        uint8_t *buffer = new uint8_t[c + sizeof(int8_t)];

        buffer[0] = BKV::BKV_INT8;
        buffer[1] = nameLen;
        std::memcpy(buffer + 2, name, sizeof(wchar_t) * nameLen); // Copy name
        buffer[c - 1] = data;

        return std::shared_ptr<uint8_t>(buffer, std::free);
    }
    std::shared_ptr<uint8_t> Serializer::generate(const wchar_t* name, const uint8_t nameLen, uint16_t data) {
        size_t c = 2 + (sizeof(wchar_t) * nameLen);
        uint8_t *buffer = new uint8_t[c + sizeof(uint16_t)];
        data = Endianness::hton(data);

        buffer[0] = BKV::BKV_UINT16;
        buffer[1] = nameLen;
        std::memcpy(buffer + 2, name, sizeof(wchar_t) * nameLen); // Copy name
        std::memcpy(buffer + c, &data, sizeof(uint16_t));

        return std::shared_ptr<uint8_t>(buffer, std::free);
    }
    std::shared_ptr<uint8_t> Serializer::generate(const wchar_t* name, const uint8_t nameLen, int16_t data) {
        size_t c = 2 + (sizeof(wchar_t) * nameLen);
        uint8_t *buffer = new uint8_t[c + sizeof(int16_t)];
        data = Endianness::hton(data);

        buffer[0] = BKV::BKV_INT16;
        buffer[1] = nameLen;
        std::memcpy(buffer + 2, name, sizeof(wchar_t) * nameLen); // Copy name
        std::memcpy(buffer + c, &data, sizeof(int16_t));

        return std::shared_ptr<uint8_t>(buffer, std::free);
    }
    std::shared_ptr<uint8_t> Serializer::generate(const wchar_t* name, const uint8_t nameLen, uint32_t data) {
        size_t c = 2 + (sizeof(wchar_t) * nameLen);
        uint8_t *buffer = new uint8_t[c + sizeof(uint32_t)];
        data = Endianness::hton(data);

        buffer[0] = BKV::BKV_UINT32;
        buffer[1] = nameLen;
        std::memcpy(buffer + 2, name, sizeof(wchar_t) * nameLen); // Copy name
        std::memcpy(buffer + c, &data, sizeof(uint32_t));

        return std::shared_ptr<uint8_t>(buffer, std::free);
    }
    std::shared_ptr<uint8_t> Serializer::generate(const wchar_t* name, const uint8_t nameLen, int32_t data) {
        size_t c = 2 + (sizeof(wchar_t) * nameLen);
        uint8_t *buffer = new uint8_t[c + sizeof(int32_t)];
        data = Endianness::hton(data);

        buffer[0] = BKV::BKV_INT32;
        buffer[1] = nameLen;
        std::memcpy(buffer + 2, name, sizeof(wchar_t) * nameLen); // Copy name
        std::memcpy(buffer + c, &data, sizeof(int32_t));

        return std::shared_ptr<uint8_t>(buffer, std::free);
    }
    std::shared_ptr<uint8_t> Serializer::generate(const wchar_t* name, const uint8_t nameLen, uint64_t data) {
        size_t c = 2 + (sizeof(wchar_t) * nameLen);
        uint8_t *buffer = new uint8_t[c + sizeof(uint64_t)];
        data = Endianness::hton(data);

        buffer[0] = BKV::BKV_UINT64;
        buffer[1] = nameLen;
        std::memcpy(buffer + 2, name, sizeof(wchar_t) * nameLen); // Copy name
        std::memcpy(buffer + c, &data, sizeof(uint64_t));

        return std::shared_ptr<uint8_t>(buffer, std::free);
    }
    std::shared_ptr<uint8_t> Serializer::generate(const wchar_t* name, const uint8_t nameLen, int64_t data) {
        size_t c = 2 + (sizeof(wchar_t) * nameLen);
        uint8_t *buffer = new uint8_t[c + sizeof(int64_t)];
        data = Endianness::hton(data);

        buffer[0] = BKV::BKV_INT64;
        buffer[1] = nameLen;
        std::memcpy(buffer + 2, name, sizeof(wchar_t) * nameLen); // Copy name
        std::memcpy(buffer + c, &data, sizeof(int64_t));

        return std::shared_ptr<uint8_t>(buffer, std::free);
    }
    std::shared_ptr<uint8_t> Serializer::generate(const wchar_t* name, const uint8_t nameLen, const wchar_t* data, uint16_t len) {
        size_t c = 2 + (sizeof(wchar_t) * nameLen);
        uint8_t *buffer = new uint8_t[c + sizeof(uint16_t) + (sizeof(wchar_t) * len)];

        buffer[0] = BKV::BKV_STRING;
        buffer[1] = nameLen;
        std::memcpy(buffer + 2, name, sizeof(wchar_t) * nameLen); // Copy name
        std::memcpy(buffer + c + sizeof(uint16_t), data, sizeof(wchar_t) * len); // Copy string after string length
        len = Endianness::hton(len);
        std::memcpy(buffer + c, &len, sizeof(uint16_t)); // Copy string length before string

        return std::shared_ptr<uint8_t>(buffer, std::free);
    }
}
