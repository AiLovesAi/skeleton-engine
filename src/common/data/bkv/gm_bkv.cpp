#include "gm_bkv.hpp"

#include "../gm_logger.hpp"
#include "gm_bkv_buffer.hpp"
#include "../gm_buffer_memory.hpp"
#include "../gm_endianness.hpp"

#include <cstring>
#include <sstream>
#include <stdexcept>
#include <type_traits>

namespace game {
    BKV_t BKV::bkvFromSBKV(const UTF8Str& stringified) {
        const char* sbkv = stringified.str.get();
        BKV_Buffer buf;

        for (int64_t i = 0; i < stringified.len; i++) {
            try { buf.state()->parse(buf, sbkv[i]); } catch (std::runtime_error &e) { throw; }
        }

        return BKV_t{.size = buf.size(), .data = buf.data()};
    }

    /*BKV::BKV_t BKV::bkvCompound(const UTF8Str& name) {
        if (name.len > UINT8_MAX) {
            std::stringstream msg;
            msg << "Too many characters in BKV name: " << name.len << "/" << UINT8_MAX << " characters.";
            throw std::runtime_error(msg.str());
        }
        const int64_t allocSize = 2 + name.len;
        uint8_t *buffer = new uint8_t[allocSize];

        buffer[0] = BKV::BKV_COMPOUND; // ID
        buffer[1] = name.len; // Name length
        std::memcpy(buffer + 2, name.str.get(), name.len); // Name

        return BKV_t{.size = allocSize, .data = std::shared_ptr<uint8_t>(buffer, std::free)};
    }
    template<typename T>
    BKV::BKV_t BKV::bkv(const UTF8Str& name, const T data) {
        static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value,
            "Generic bkv() only supports primitive data types.");

        if (name.len > UINT8_MAX) {
            std::stringstream msg;
            msg << "Too many characters in BKV name: " << name.len << "/" << UINT8_MAX << " characters.";
            throw std::runtime_error(msg.str());
        }
        const int64_t allocSize = 2 + name.len + sizeof(T);
        uint8_t *buffer = new uint8_t[allocSize];
        const T networkData = Endianness::hton(data);

        buffer[0] = BKV::BKVTypeMap<T>::tagID; // ID
        buffer[1] = name.len; // Name length
        std::memcpy(buffer + 2, name.str.get(), name.len); // Name
        std::memcpy(buffer + allocSize - sizeof(T), &networkData, sizeof(T)); // Data

        return BKV_t{.size = allocSize, .data = std::shared_ptr<uint8_t>(buffer, std::free)};
    }
    template<typename T>
    BKV::BKV_t BKV::bkvList(const UTF8Str& name, const T* data, uint32_t size) {
        static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value,
            "Generic bkvList() only supports primitive data types.");

        if (name.len > UINT8_MAX) {
            std::stringstream msg;
            msg << "Too many characters in BKV name: " << name.len << "/" << UINT8_MAX << " characters.";
            throw std::runtime_error(msg.str());
        }
        const int64_t listStart = 2 + name.len + sizeof(uint32_t);
        const int64_t allocSize = listStart + (sizeof(T) * size);
        uint8_t *buffer = new uint8_t[allocSize];
        const uint32_t networkSize = Endianness::hton(size);

        buffer[0] = BKV::BKVTypeMap<T>::tagID + 1; // ID (TagID + 1 is an array of that type)
        buffer[1] = name.len; // Name length
        std::memcpy(buffer + 2, name.str.get(), name.len); // Name
        std::memcpy(buffer + listStart - sizeof(uint32_t), &networkSize, sizeof(uint32_t)); // List size
        T networkData;
        for (int64_t i = 0; i < size; i++) {
            networkData = Endianness::hton(data[i]);
            std::memcpy(buffer + listStart + (i * sizeof(T)), &networkData, sizeof(T)); // Data (after size)
        }

        return BKV_t{.size = allocSize, .data = std::shared_ptr<uint8_t>(buffer, std::free)};
    }
    BKV::BKV_t BKV::bkvStr(const UTF8Str& name, const UTF8Str& data) {
        if (name.len > UINT8_MAX) {
            std::stringstream msg;
            msg << "Too many characters in BKV name: " << name.len << "/" << UINT8_MAX << " characters.";
            throw std::runtime_error(msg.str());
        }
        if (data.len > UINT16_MAX) {
            std::stringstream msg;
            msg << "Too many characters in BKV string: " << name.len << "/65535 characters.";
            throw std::runtime_error(msg.str());
        }
        const int64_t strStart = 2 + name.len + sizeof(uint16_t);
        const int64_t allocSize = strStart + data.len;
        uint8_t *buffer = new uint8_t[allocSize];
        const uint16_t networkLen = Endianness::hton(data.len);

        buffer[0] = BKV::BKV_STR;
        buffer[1] = name.len;
        std::memcpy(buffer + 2, name.str.get(), name.len); // Name
        std::memcpy(buffer + strStart - sizeof(uint16_t), &networkLen, sizeof(uint16_t)); // String length
        std::memcpy(buffer + strStart, data.str.get(), data.len); // String

        return BKV_t{.size = allocSize, .data = std::shared_ptr<uint8_t>(buffer, std::free)};
    }
    BKV::BKV_t BKV::bkvStrList(const UTF8Str& name, const UTF8Str* data, const uint32_t size) {
        const int64_t listStart = 2 + name.len + sizeof(uint32_t);
        int64_t allocSize = listStart + (sizeof(uint32_t) * size);
        for (int64_t i = 0; i < size; i++) {
            if (data[i].len > UINT16_MAX) {
                std::stringstream msg;
                msg << "Too many characters in BKV string: " << name.len << "/65535 characters.";
                throw std::runtime_error(msg.str());
            }
            allocSize += data[i].len;
        }
        uint8_t *buffer = new uint8_t[allocSize];
        uint32_t networkSize = Endianness::hton(size);

        buffer[0] = BKV::BKV_STR_ARRAY; // ID
        buffer[1] = name.len; // Name length
        std::memcpy(buffer + 2, name.str.get(), name.len); // Name
        std::memcpy(buffer + listStart - sizeof(uint32_t), &networkSize, sizeof(uint32_t)); // List size
        for (int64_t i = 0, head = listStart; i < size; i++) {
            networkSize = Endianness::hton(data[i].len);
            std::memcpy(buffer + head, &networkSize, sizeof(uint32_t)); // String length
            head += sizeof(uint32_t);
            std::memcpy(buffer + head, data[i].str.get(), data[i].len); // String
            head += data[i].len;
        }

        return BKV_t{.size = allocSize, .data = std::shared_ptr<uint8_t>(buffer, std::free)};
    }*/
}