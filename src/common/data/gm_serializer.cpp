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

    std::shared_ptr<uint8_t> Serializer::generateCompound(const UTF8Str& name) {
        uint8_t *buffer = new uint8_t[2 + name.len];

        buffer[0] = BKV::BKV_COMPOUND; // ID
        buffer[1] = name.len; // Name length
        std::memcpy(buffer + 2, name.str.get(), name.len); // Name

        return std::shared_ptr<uint8_t>(buffer, std::free);
    }
    template<typename T>
    std::shared_ptr<uint8_t> Serializer::generate(const UTF8Str& name, const T data) {
        static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value,
            "Generic generate() only supports primitive data types.");

        size_t c = 2 + name.len;
        uint8_t *buffer = new uint8_t[c + sizeof(T)];
        const T networkData = Endianness::hton(data);

        buffer[0] = BKV::BKVTypeMap<T>::tagID; // ID
        buffer[1] = name.len; // Name length
        std::memcpy(buffer + 2, name.str.get(), name.len); // Name
        std::memcpy(buffer + c, &networkData, sizeof(T)); // Data

        return std::shared_ptr<uint8_t>(buffer, std::free);
    }
    template<typename T>
    std::shared_ptr<uint8_t> Serializer::generateList(const UTF8Str& name, const T* data, uint32_t size) {
        static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value,
            "Generic generateList() only supports primitive data types.");

        size_t c = 2 + name.len + sizeof(uint32_t);
        uint8_t *buffer = new uint8_t[c + (sizeof(T) * size)];
        const uint32_t networkSize = Endianness::hton(size);

        buffer[0] = BKV::BKVTypeMap<T>::tagID + 1; // ID (TagID + 1 is an array of that type)
        buffer[1] = name.len; // Name length
        std::memcpy(buffer + 2, name.str.get(), name.len); // Name
        std::memcpy(buffer + c - sizeof(uint32_t), &networkSize, sizeof(uint32_t)); // List size
        T networkData;
        for (size_t i = 0; i < size; i++) {
            networkData = Endianness::hton(data[i]);
            std::memcpy(buffer + c + (i * sizeof(T)), &networkData, sizeof(T)); // Data (after size)
        }

        return std::shared_ptr<uint8_t>(buffer, std::free);
    }
    std::shared_ptr<uint8_t> Serializer::generateStr(const UTF8Str& name, const UTF8Str& data) {
        size_t c = 2 + name.len + sizeof(uint16_t);
        uint8_t *buffer = new uint8_t[c + (sizeof(wchar_t) * data.len)];
        const uint16_t networkLen = Endianness::hton(data.len);

        buffer[0] = BKV::BKV_STR;
        buffer[1] = name.len;
        std::memcpy(buffer + 2, name.str.get(), name.len); // Name
        std::memcpy(buffer + c - sizeof(uint16_t), &networkLen, sizeof(uint16_t)); // String length
        std::memcpy(buffer + c, data.str.get(), sizeof(wchar_t) * data.len); // String

        return std::shared_ptr<uint8_t>(buffer, std::free);
    }
    std::shared_ptr<uint8_t> Serializer::generateStrList(const UTF8Str& name, const UTF8Str* data, const uint32_t size) {
        size_t c = 2 + name.len + sizeof(uint32_t);
        size_t head = 0;
        for (size_t i = 0; i < size; i++) head += data[i].len;
        uint8_t *buffer = new uint8_t[c + head + (sizeof(uint32_t) * size)];
        uint32_t networkSize = Endianness::hton(size);

        buffer[0] = BKV::BKV_STR_ARRAY; // ID
        buffer[1] = name.len; // Name length
        std::memcpy(buffer + 2, name.str.get(), name.len); // Name
        std::memcpy(buffer + c - sizeof(uint32_t), &networkSize, sizeof(uint32_t)); // List size
        for (size_t i = 0, head = 0; i < size; i++) {
            networkSize = Endianness::hton(data[i].len);
            std::memcpy(buffer + head, &networkSize, sizeof(uint32_t)); // String length
            std::memcpy(buffer + head + sizeof(uint32_t), data[i].str.get(), data[i].len); // String
            head += data[i].len + sizeof(uint32_t);
        }

        return std::shared_ptr<uint8_t>(buffer, std::free);
    }
}
