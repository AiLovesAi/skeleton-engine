#include "gm_bkv.hpp"

#include "gm_bkv_buffer.hpp"
#include "../gm_buffer_memory.hpp"
#include "../gm_endianness.hpp"

#include <cstring>
#include <sstream>
#include <stdexcept>
#include <type_traits>

namespace game {
    template <> const uint8_t BKV::BKVTypeMap<bool>::tagID = BKV_UI8;
    template <> const uint8_t BKV::BKVTypeMap<uint8_t>::tagID = BKV_UI8;
    template <> const uint8_t BKV::BKVTypeMap<int8_t>::tagID = BKV_I8;
    template <> const uint8_t BKV::BKVTypeMap<uint16_t>::tagID = BKV_UI16;
    template <> const uint8_t BKV::BKVTypeMap<int16_t>::tagID = BKV_I16;
    template <> const uint8_t BKV::BKVTypeMap<uint32_t>::tagID = BKV_UI32;
    template <> const uint8_t BKV::BKVTypeMap<int32_t>::tagID = BKV_I32;
    template <> const uint8_t BKV::BKVTypeMap<uint64_t>::tagID = BKV_UI64;
    template <> const uint8_t BKV::BKVTypeMap<int64_t>::tagID = BKV_I64;
    template <> const uint8_t BKV::BKVTypeMap<float>::tagID = BKV_FLOAT;
    template <> const uint8_t BKV::BKVTypeMap<double>::tagID = BKV_DOUBLE;
    
    template <> const char BKV::BKVSuffixMap<uint8_t>::suffix[] = "ub";
    template <> const char BKV::BKVSuffixMap<int8_t>::suffix[] = "b";
    template <> const char BKV::BKVSuffixMap<uint16_t>::suffix[] = "us";
    template <> const char BKV::BKVSuffixMap<int16_t>::suffix[] = "s";
    template <> const char BKV::BKVSuffixMap<uint32_t>::suffix[] = "u";
    template <> const char BKV::BKVSuffixMap<int32_t>::suffix[] = "";
    template <> const char BKV::BKVSuffixMap<uint64_t>::suffix[] = "ul";
    template <> const char BKV::BKVSuffixMap<int64_t>::suffix[] = "l";
    template <> const char BKV::BKVSuffixMap<float>::suffix[] = "f";
    template <> const char BKV::BKVSuffixMap<double>::suffix[] = "";

    BKV::BKV(const BKV_t& bkv) {
        resizeBuffer(bkv.size);
        std::memcpy(buffer_, bkv.data.get(), bkv.size);
    }
    BKV::BKV(const UTF8Str& stringified) {
        BKV_t bkv = bkvFromSBKV(stringified);
        resizeBuffer(bkv.size);
        std::memcpy(buffer_, bkv.data.get(), bkv.size);
    }

    void BKV::resizeBuffer(const size_t size) {
        buffer_ = static_cast<uint8_t*>(std::realloc(buffer_, size));
        capacity_ = size;
    }

    void BKV::write(const BKV_t& bkv) {
        size_t resultHead = head_ + bkv.size;

        if (resultHead > capacity_) {
            resizeBuffer(std::max(capacity_ * 2, resultHead));
        }
        
        std::memcpy(buffer_ + head_, bkv.data.get(), bkv.size);
        head_ = resultHead;
    }

    template <typename T>
    void setValueSBKV(const uint8_t* data, char*& sbkv, size_t& i, size_t& head, size_t& capacity) {
        // Val
        T v;
        std::memcpy(&v, data + i + 1 + data[i], sizeof(T));
        v = Endianness::ntoh(v);

        const std::string val = std::to_string(v);
        BufferMemory::checkResize(sbkv, head + data[i] + val.length() + sizeof(BKV::BKVSuffixMap<T>::suffix), capacity);

        // Name
        std::memcpy(sbkv + head, data + i + 1, data[i]);
        head += data[i];
        i += 1 + data[i] + sizeof(T);
        sbkv[head++] = ':';

        // Value
        std::memcpy(sbkv + head, val.c_str(), val.length()); // Value
        head += val.length();
        std::memcpy(sbkv + head, BKV::BKVSuffixMap<T>::suffix, sizeof(BKV::BKVSuffixMap<T>::suffix) - 1);
        head += sizeof(BKV::BKVSuffixMap<T>::suffix) - 1;
        sbkv[head++] = ',';
    }
    template <typename T>
    void setArraySBKV(const uint8_t* data, char*& sbkv, size_t& i,  size_t& head, size_t& capacity) {
        // Name
        BufferMemory::checkResize(sbkv, head + data[i] + 2, capacity);
        std::memcpy(sbkv + head, data + i + 1, data[i]);
        head += data[i];
        i += 1 + data[i];
        sbkv[head++] = ':';
        sbkv[head++] = '[';
        
        // Array size
        uint32_t size;
        std::memcpy(&size, data + i, sizeof(uint32_t));
        i += sizeof(uint32_t);
        size = Endianness::ntoh(size);

        // Values
        T v;
        std::string val;
        for (uint32_t index = 0; index < size; index++) {
            std::memcpy(&v, data + i, sizeof(T));
            i += sizeof(T);
            v = Endianness::ntoh(v);
            val = std::to_string(v);

            BufferMemory::checkResize(sbkv, head + val.length() + sizeof(BKV::BKVSuffixMap<T>::suffix) + 1, capacity);
            std::memcpy(sbkv + head, val.c_str(), val.length());
            head += val.length();
            std::memcpy(sbkv + head, BKV::BKVSuffixMap<T>::suffix, sizeof(BKV::BKVSuffixMap<T>::suffix) - 1);
            head += sizeof(BKV::BKVSuffixMap<T>::suffix) - 1;
            sbkv[head++] = ',';
        }
        sbkv[head - 1] = ']'; // Replace last comma with close bracket
        sbkv[head++] = ',';
    }
    BKV::UTF8Str BKV::sbkvFromBKV(const BKV_t& bkv) {
        size_t capacity = bkv.size; // Should be at least bkvSize
        char* sbkv = static_cast<char*>(std::malloc(capacity));
        sbkv[0] = '{';

        const uint8_t* data = bkv.data.get();
        size_t head, i;
        for (head = 0, i = 0; i < bkv.size; i++) {
            switch(data[i++]) {
                case BKV_END:
                    sbkv[head - 1] = '}'; // Replace last comma with close brace
                    break;
                case BKV_COMPOUND: // Name:{
                    BufferMemory::checkResize(sbkv, head + data[i] + 3, head, capacity);

                    // Name
                    std::memcpy(sbkv + head, data + i + 1, data[i]);
                    head += data[i];
                    i += 1 + data[i];

                    sbkv[head++] = ':';
                    sbkv[head++] = '{';
                    sbkv[head++] = ',';
                    break;
                case BKV_UI8: // Name:Xub
                    setValueSBKV<uint8_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV_UI8_ARRAY: // Name:[Xub,Yub,Zub],
                    setArraySBKV<uint8_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV_I8: // Name:Xb,
                    setValueSBKV<int8_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV_I8_ARRAY: // Name:[Xb,Yb,Zb],
                    setArraySBKV<int8_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV_UI16: // Name:Xus,
                    setValueSBKV<uint16_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV_UI16_ARRAY: // Name:[Xus,Yus,Zus],
                    setArraySBKV<uint16_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV_I16: // Name:Xs,
                    setValueSBKV<int16_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV_I16_ARRAY: // Name:[Xs,Ys,Zs]
                    setArraySBKV<int16_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV_UI32: // Name:Xu,
                    setValueSBKV<uint32_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV_UI32_ARRAY: // Name:[Xu,Yu,Zu]
                    setArraySBKV<uint32_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV_I32: // Name:X,
                    setValueSBKV<int32_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV_I32_ARRAY: // Name:[X,Y,Z],
                    setArraySBKV<int32_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV_UI64: // Name:Xul,
                    setValueSBKV<uint64_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV_UI64_ARRAY: // Name:[Xul,Yul,Zul],
                    setArraySBKV<uint64_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV_I64: // Name:Xl,
                    setValueSBKV<int64_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV_I64_ARRAY: // Name:[Xl,Yl,Zl],
                    setArraySBKV<int64_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV_FLOAT: // Name:X.f,
                    setValueSBKV<float>(data, sbkv, i, head, capacity);
                    break;
                case BKV_FLOAT_ARRAY: // Name:[X.f,Y.f,Z.f],
                    setArraySBKV<float>(data, sbkv, i, head, capacity);
                    break;
                case BKV_DOUBLE: // Name:X.,
                    setValueSBKV<double>(data, sbkv, i, head, capacity);
                    break;
                case BKV_DOUBLE_ARRAY: // Name:[X.,Y.,Z.],
                    setArraySBKV<double>(data, sbkv, i, head, capacity);
                    break;
                case BKV_STR: { // Name:Str,
                    // Val
                    size_t len;
                    std::memcpy(&len, data + i + 1 + data[i], sizeof(uint16_t));
                    len = Endianness::ntoh(len);

                    BufferMemory::checkResize(sbkv, head + data[i] + len + 2, head, capacity);

                    // Name
                    std::memcpy(sbkv + head, data + i + 1, data[i]);
                    head += data[i];
                    i += 1 + data[i] + sizeof(uint16_t);
                    sbkv[head++] = ':';

                    // Value
                    std::memcpy(sbkv + head, data + i, len);
                    head += len;
                    i += len;
                    sbkv[head++] = ',';
                } break;
                case BKV_STR_ARRAY: { // Name:[Str1,Str2,Str3],
                    // Name
                    BufferMemory::checkResize(sbkv, head + data[i] + 2, head, capacity);
                    std::memcpy(sbkv + head, data + i + 1, data[i]);
                    head += data[i];
                    i += 1 + data[i];
                    sbkv[head++] = ':';
                    sbkv[head++] = '[';

                    // Array size
                    uint32_t size;
                    std::memcpy(&size, data + i, sizeof(uint32_t));
                    i += sizeof(uint32_t);
                    size = Endianness::ntoh(size);

                    // Values
                    uint16_t len;
                    std::string str;
                    for (uint32_t index = 0; index < size; index++) {
                        std::memcpy(&len, data + i, sizeof(uint16_t));
                        i += sizeof(uint16_t);
                        len = Endianness::ntoh(len);

                        BufferMemory::checkResize(sbkv, head + len + 2, head, capacity);
                        std::memcpy(sbkv + head, data + i, len);
                        head += len;
                        i += len;
                        sbkv[head++] = ',';
                    }
                    sbkv[head - 1] = ']'; // Replace last comma with close bracket
                    sbkv[head++] = ',';
                } break;
            }
        }

        sbkv[head++] = '}';
        capacity = head + 1;
        sbkv = static_cast<char*>(std::realloc(sbkv, capacity));
        return UTF8Str{.len = capacity, .str = std::shared_ptr<char>(sbkv, std::free)};
    }
    
    BKV::BKV_t BKV::bkvFromSBKV(const UTF8Str& stringified) {
        const char* sbkv = stringified.str.get();
        BKV_Buffer buf;
        buf.capacity = stringified.len; // Starting length
        buf.bkv = static_cast<uint8_t*>(std::malloc(buf.capacity));
        
        char c;
        for (size_t i = 0; i < stringified.len; i++) {
            c = sbkv[i];
            buf.state->parse(buf, c);
        }

        uint8_t nameLen = 0;
        uint16_t strLen = 0;
        uint32_t arraySize = 0;
        size_t strCapacity = BUFSIZ, head = 0; 
        char* str = static_cast<char*>(std::malloc(strCapacity));
        bool inName = true, isArray = false, isNumber = false;
        char strChar = 0, c;
        for (size_t i = 0; i < stringified.len; i++) {
            c = sbkv[i];
            if (inName) { // Get name
                if (c == ':') {
                    inName = false;
                    BufferMemory::checkResize(buf.bkv, head + 2 + nameLen, head, buf.capacity);
                    buf.bkv[head + 1] = nameLen;
                    std::memcpy(buf.bkv + head + 2, str, nameLen); // Copy name after tag and name length
                    continue;
                } else {
                    str[nameLen++] = c;
                    BufferMemory::checkResize(str, nameLen, strCapacity);
                    continue;
                }
                continue;
            } else { // Record data into string
                if (strChar) {
                    if ((c == ',' && strChar == DEFAULT_CHAR) || (c == strChar && sbkv[i - 1] != '\\')) {
                        strChar = 0;
                        if (!isArray) {
                            // Finished string
                            BufferMemory::checkResize(bkv, head + 2 + nameLen + sizeof(uint16_t) + strLen, head, capacity);
                            bkv[head] = BKV_STR;
                            head += 2 + nameLen;
                            strLen = Endianness::hton(strLen);
                            std::memcpy(bkv + head, &strLen, sizeof(uint16_t));
                            head += sizeof(uint16_t);
                            std::memcpy(bkv + head, str, strLen);

                            // Reset
                            strLen = 0;
                            nameLen = 0;
                            inName = true;
                            continue;
                        }
                        continue;
                    } else {
                        str[strLen++] = c;
                        BufferMemory::checkResize(str, strLen, strLen - 1, strCapacity);
                        continue;
                    }
                } else {
                    if (c == '[') {
                        isArray = true;
                        continue;
                    } else if (c == '-' || c == '.' || (c >= '0' && c <= '9')) {
                        isNumber = true;
                        continue;
                    } else if (c == '"' || c == '\'') {
                        strChar = c;
                        continue;
                    } else { // Default to unquoted string
                        strChar = DEFAULT_CHAR;
                    }
                }
            }
        }

        return BKV_t{};
    }
    BKV::BKV_t BKV::bkvCompound(const UTF8Str& name) {
        if (name.len > UINT8_MAX) {
            std::stringstream msg;
            msg << "Too many characters in BKV name: " << name.len << "/255 characters.";
            throw std::length_error(msg.str());
        }
        const size_t allocSize = 2 + name.len;
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
            msg << "Too many characters in BKV name: " << name.len << "/255 characters.";
            throw std::length_error(msg.str());
        }
        const size_t allocSize = 2 + name.len + sizeof(T);
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
            msg << "Too many characters in BKV name: " << name.len << "/255 characters.";
            throw std::length_error(msg.str());
        }
        const size_t listStart = 2 + name.len + sizeof(uint32_t);
        const size_t allocSize = listStart + (sizeof(T) * size);
        uint8_t *buffer = new uint8_t[allocSize];
        const uint32_t networkSize = Endianness::hton(size);

        buffer[0] = BKV::BKVTypeMap<T>::tagID + 1; // ID (TagID + 1 is an array of that type)
        buffer[1] = name.len; // Name length
        std::memcpy(buffer + 2, name.str.get(), name.len); // Name
        std::memcpy(buffer + listStart - sizeof(uint32_t), &networkSize, sizeof(uint32_t)); // List size
        T networkData;
        for (size_t i = 0; i < size; i++) {
            networkData = Endianness::hton(data[i]);
            std::memcpy(buffer + listStart + (i * sizeof(T)), &networkData, sizeof(T)); // Data (after size)
        }

        return BKV_t{.size = allocSize, .data = std::shared_ptr<uint8_t>(buffer, std::free)};
    }
    BKV::BKV_t BKV::bkvStr(const UTF8Str& name, const UTF8Str& data) {
        if (name.len > UINT8_MAX) {
            std::stringstream msg;
            msg << "Too many characters in BKV name: " << name.len << "/255 characters.";
            throw std::length_error(msg.str());
        }
        if (data.len > UINT16_MAX) {
            std::stringstream msg;
            msg << "Too many characters in BKV string: " << name.len << "/65535 characters.";
            throw std::length_error(msg.str());
        }
        const size_t strStart = 2 + name.len + sizeof(uint16_t);
        const size_t allocSize = strStart + data.len;
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
        const size_t listStart = 2 + name.len + sizeof(uint32_t);
        size_t allocSize = listStart + (sizeof(uint32_t) * size);
        for (size_t i = 0; i < size; i++) {
            if (data[i].len > 0xffff) {
                std::stringstream msg;
                msg << "Too many characters in BKV string: " << name.len << "/65535 characters.";
                throw std::length_error(msg.str());
            }
            allocSize += data[i].len;
        }
        uint8_t *buffer = new uint8_t[allocSize];
        uint32_t networkSize = Endianness::hton(size);

        buffer[0] = BKV::BKV_STR_ARRAY; // ID
        buffer[1] = name.len; // Name length
        std::memcpy(buffer + 2, name.str.get(), name.len); // Name
        std::memcpy(buffer + listStart - sizeof(uint32_t), &networkSize, sizeof(uint32_t)); // List size
        for (size_t i = 0, head = listStart; i < size; i++) {
            networkSize = Endianness::hton(data[i].len);
            std::memcpy(buffer + head, &networkSize, sizeof(uint32_t)); // String length
            head += sizeof(uint32_t);
            std::memcpy(buffer + head, data[i].str.get(), data[i].len); // String
            head += data[i].len;
        }

        return BKV_t{.size = allocSize, .data = std::shared_ptr<uint8_t>(buffer, std::free)};
    }
}