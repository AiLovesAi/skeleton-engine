#include "gm_bkv.hpp"

#include "gm_endianness.hpp"

#include <cstring>
#include <stdexcept>
#include <sstream>
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

    inline void checkResize(char*& ptr, const size_t head, size_t& capacity) {
        if (head > capacity) {
            capacity *= 2;
            ptr = static_cast<char*>(std::realloc(ptr, capacity));
        }
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
                    checkResize(sbkv, head + 1, capacity);
                    sbkv[head++] = '}';
                    break;
                case BKV_COMPOUND: // "Name:{"
                    checkResize(sbkv, head + data[i] + 2, capacity);
                    // Name
                    std::memcpy(sbkv + head, data + i + 1, data[i]);
                    head += data[i];
                    i += data[i];
                    sbkv[head++] = ':';
                    sbkv[head++] = '{';
                    break;
                case BKV_UI8: { // Name:Xub
                    const std::string val = std::to_string(data[i++]);
                    checkResize(sbkv, head + data[i] + val.length() + 3, capacity);

                    // Name
                    std::memcpy(sbkv + head, data + i + 1, data[i]);
                    head += data[i];
                    i += data[i];
                    sbkv[head++] = ':';

                    // Value
                    std::memcpy(sbkv + head, val.c_str(), val.length()); // Value
                    head += val.length();
                    sbkv[head++] = 'u';
                    sbkv[head++] = 'b';
                } break;
                case BKV_UI8_ARRAY: { // Name:[Xub,Yub,Zub]
                    // Name
                    checkResize(sbkv, head + data[i] + 2, capacity);
                    std::memcpy(sbkv + head, data + i + 1, data[i]);
                    head += data[i];
                    i += data[i];
                    sbkv[head++] = ':';
                    sbkv[head++] = '[';
                    
                    // Array size
                    uint32_t size;
                    std::memcpy(&size, data + i, sizeof(uint32_t));
                    i += sizeof(uint32_t);
                    size = Endianness::ntoh(size);

                    // Values
                    std::string val;
                    for (uint32_t index = 0; index < size; index++) {
                        val = std::to_string(data[i++]);
                        checkResize(sbkv, head + val.length() + 3, capacity);
                        std::memcpy(sbkv + head, val.c_str(), val.length());
                        head += val.length();
                        sbkv[head++] = 'u';
                        sbkv[head++] = 'b';
                        sbkv[head++] = ',';
                    }
                    sbkv[head] = ']'; // Replace last comma with close bracket
                } break;
                case BKV_I8: { // Name:Xb
                    const std::string val = std::to_string(data[i++]); // TODO This is incorrect. Should be data[i + 1 + nameLen], move to after Name
                    checkResize(sbkv, head + data[i] + val.length() + 2, capacity);
                    std::memcpy(sbkv + head, data + i + 1, data[i]); // Name
                    head += data[i];
                    i += data[i];
                    sbkv[head++] = ':';
                    std::memcpy(sbkv + head, val.c_str(), val.length()); // Value
                    head += val.length();
                    sbkv[head++] = 'b';
                } break;
                case BKV_I8_ARRAY: { // Name:[Xb,Yb,Zb]
                    // Name
                    checkResize(sbkv, head + data[i] + 2, capacity);
                    std::memcpy(sbkv + head, data + i + 1, data[i]);
                    head += data[i];
                    i += data[i];
                    sbkv[head++] = ':';
                    sbkv[head++] = '[';
                    
                    // Array size
                    uint32_t size;
                    std::memcpy(&size, data + i, sizeof(uint32_t));
                    i += sizeof(uint32_t);
                    size = Endianness::ntoh(size);

                    // Values
                    std::string val;
                    for (uint32_t index = 0; index < size; index++) {
                        val = std::to_string(data[i++]);
                        checkResize(sbkv, head + val.length() + 2, capacity);
                        std::memcpy(sbkv + head, val.c_str(), val.length());
                        head += val.length();
                        sbkv[head++] = 'b';
                        sbkv[head++] = ',';
                    }
                    sbkv[head] = ']'; // Replace last comma with close bracket
                } break;
                case BKV_UI16: { // Name:Xus
                    // Val
                    uint16_t v;
                    std::memcpy(&v, data + i + data[i], sizeof(uint16_t));
                    v = Endianness::ntoh(v);

                    const std::string val = std::to_string(v);
                    checkResize(sbkv, head + data[i] + val.length() + 3, capacity);

                    // Name
                    std::memcpy(sbkv + head, data + i + 1, data[i]);
                    head += data[i];
                    i += data[i] + sizeof(uint16_t);
                    sbkv[head++] = ':';

                    // Value
                    std::memcpy(sbkv + head, val.c_str(), val.length()); // Value
                    head += val.length();
                    sbkv[head++] = 'u';
                    sbkv[head++] = 's';
                } break;
                case BKV_UI16_ARRAY: { // Name:[Xus,Yus,Zus]
                    // Name
                    checkResize(sbkv, head + data[i] + 2, capacity);
                    std::memcpy(sbkv + head, data + i + 1, data[i]);
                    head += data[i];
                    i += data[i];
                    sbkv[head++] = ':';
                    sbkv[head++] = '[';
                    
                    // Array size
                    uint32_t size;
                    std::memcpy(&size, data + i, sizeof(uint32_t));
                    i += sizeof(uint32_t);
                    size = Endianness::ntoh(size);

                    // Values
                    uint16_t v;
                    std::string val;
                    for (uint32_t index = 0; index < size; index++) {
                        std::memcpy(&v, data + i, sizeof(uint16_t));
                        i += sizeof(uint16_t);
                        v = Endianness::ntoh(v);
                        val = std::to_string(v);

                        checkResize(sbkv, head + val.length() + 3, capacity);
                        std::memcpy(sbkv + head, val.c_str(), val.length());
                        head += val.length();
                        sbkv[head++] = 'u';
                        sbkv[head++] = 's';
                        sbkv[head++] = ',';
                    }
                    sbkv[head] = ']'; // Replace last comma with close bracket
                } break;
                case BKV_I16: { // Name:Xs
                    // Val
                    int16_t v;
                    std::memcpy(&v, data + i + data[i], sizeof(int16_t));
                    v = Endianness::ntoh(v);

                    const std::string val = std::to_string(v);
                    checkResize(sbkv, head + data[i] + val.length() + 2, capacity);

                    // Name
                    std::memcpy(sbkv + head, data + i + 1, data[i]);
                    head += data[i];
                    i += data[i] + sizeof(int16_t);
                    sbkv[head++] = ':';

                    // Value
                    std::memcpy(sbkv + head, val.c_str(), val.length()); // Value
                    head += val.length();
                    sbkv[head++] = 's';
                } break;
                case BKV_I16_ARRAY: { // Name:[Xs,Ys,Zs]
                    // Name
                    checkResize(sbkv, head + data[i] + 2, capacity);
                    std::memcpy(sbkv + head, data + i + 1, data[i]);
                    head += data[i];
                    i += data[i];
                    sbkv[head++] = ':';
                    sbkv[head++] = '[';
                    
                    // Array size
                    uint32_t size;
                    std::memcpy(&size, data + i, sizeof(uint32_t));
                    i += sizeof(uint32_t);
                    size = Endianness::ntoh(size);

                    // Values
                    int16_t v;
                    std::string val;
                    for (uint32_t index = 0; index < size; index++) {
                        std::memcpy(&v, data + i, sizeof(int16_t));
                        i += sizeof(int16_t);
                        v = Endianness::ntoh(v);
                        val = std::to_string(v);

                        checkResize(sbkv, head + val.length() + 2, capacity);
                        std::memcpy(sbkv + head, val.c_str(), val.length());
                        head += val.length();
                        sbkv[head++] = 's';
                        sbkv[head++] = ',';
                    }
                    sbkv[head] = ']'; // Replace last comma with close bracket
                } break;
                case BKV_UI32: { // Name:Xu
                    // Val
                    uint32_t v;
                    std::memcpy(&v, data + i + data[i], sizeof(uint32_t));
                    v = Endianness::ntoh(v);

                    const std::string val = std::to_string(v);
                    checkResize(sbkv, head + data[i] + val.length() + 2, capacity);

                    // Name
                    std::memcpy(sbkv + head, data + i + 1, data[i]);
                    head += data[i];
                    i += data[i] + sizeof(uint32_t);
                    sbkv[head++] = ':';

                    // Value
                    std::memcpy(sbkv + head, val.c_str(), val.length()); // Value
                    head += val.length();
                    sbkv[head++] = 'u';
                } break;
                case BKV_UI32_ARRAY: { // Name:[Xu,Yu,Zu]
                    // Name
                    checkResize(sbkv, head + data[i] + 2, capacity);
                    std::memcpy(sbkv + head, data + i + 1, data[i]);
                    head += data[i];
                    i += data[i];
                    sbkv[head++] = ':';
                    sbkv[head++] = '[';
                    
                    // Array size
                    uint32_t size;
                    std::memcpy(&size, data + i, sizeof(uint32_t));
                    i += sizeof(uint32_t);
                    size = Endianness::ntoh(size);

                    // Values
                    uint32_t v;
                    std::string val;
                    for (uint32_t index = 0; index < size; index++) {
                        std::memcpy(&v, data + i, sizeof(uint32_t));
                        i += sizeof(uint32_t);
                        v = Endianness::ntoh(v);
                        val = std::to_string(v);

                        checkResize(sbkv, head + val.length() + 2, capacity);
                        std::memcpy(sbkv + head, val.c_str(), val.length());
                        head += val.length();
                        sbkv[head++] = 'u';
                        sbkv[head++] = ',';
                    }
                    sbkv[head] = ']'; // Replace last comma with close bracket
                } break;
                case BKV_I32: { // Name:X
                    // Val
                    int32_t v;
                    std::memcpy(&v, data + i + data[i], sizeof(int32_t));
                    v = Endianness::ntoh(v);

                    const std::string val = std::to_string(v);
                    checkResize(sbkv, head + data[i] + val.length() + 1, capacity);

                    // Name
                    std::memcpy(sbkv + head, data + i + 1, data[i]);
                    head += data[i];
                    i += data[i] + sizeof(int32_t);
                    sbkv[head++] = ':';

                    // Value
                    std::memcpy(sbkv + head, val.c_str(), val.length()); // Value
                    head += val.length();
                } break;
                case BKV_I32_ARRAY: { // Name:[X,Y,Z]
                    // Name
                    checkResize(sbkv, head + data[i] + 2, capacity);
                    std::memcpy(sbkv + head, data + i + 1, data[i]);
                    head += data[i];
                    i += data[i];
                    sbkv[head++] = ':';
                    sbkv[head++] = '[';
                    
                    // Array size
                    uint32_t size;
                    std::memcpy(&size, data + i, sizeof(uint32_t));
                    i += sizeof(uint32_t);
                    size = Endianness::ntoh(size);

                    // Values
                    int32_t v;
                    std::string val;
                    for (uint32_t index = 0; index < size; index++) {
                        std::memcpy(&v, data + i, sizeof(int32_t));
                        i += sizeof(int32_t);
                        v = Endianness::ntoh(v);
                        val = std::to_string(v);

                        checkResize(sbkv, head + val.length() + 1, capacity);
                        std::memcpy(sbkv + head, val.c_str(), val.length());
                        head += val.length();
                        sbkv[head++] = ',';
                    }
                    sbkv[head] = ']'; // Replace last comma with close bracket
                } break;
                case BKV_UI64: { // Name:Xul
                    // Val
                    uint64_t v;
                    std::memcpy(&v, data + i + data[i], sizeof(uint64_t));
                    v = Endianness::ntoh(v);

                    const std::string val = std::to_string(v);
                    checkResize(sbkv, head + data[i] + val.length() + 3, capacity);

                    // Name
                    std::memcpy(sbkv + head, data + i + 1, data[i]);
                    head += data[i];
                    i += data[i] + sizeof(uint64_t);
                    sbkv[head++] = ':';

                    // Value
                    std::memcpy(sbkv + head, val.c_str(), val.length()); // Value
                    head += val.length();
                    sbkv[head++] = 'u';
                    sbkv[head++] = 'l';
                } break;
                case BKV_UI64_ARRAY: { // Name:[Xul,Yul,Zul]
                    // Name
                    checkResize(sbkv, head + data[i] + 2, capacity);
                    std::memcpy(sbkv + head, data + i + 1, data[i]);
                    head += data[i];
                    i += data[i];
                    sbkv[head++] = ':';
                    sbkv[head++] = '[';
                    
                    // Array size
                    uint32_t size;
                    std::memcpy(&size, data + i, sizeof(uint32_t));
                    i += sizeof(uint32_t);
                    size = Endianness::ntoh(size);

                    // Values
                    uint64_t v;
                    std::string val;
                    for (uint32_t index = 0; index < size; index++) {
                        std::memcpy(&v, data + i, sizeof(uint64_t));
                        i += sizeof(uint64_t);
                        v = Endianness::ntoh(v);
                        val = std::to_string(v);

                        checkResize(sbkv, head + val.length() + 3, capacity);
                        std::memcpy(sbkv + head, val.c_str(), val.length());
                        head += val.length();
                        sbkv[head++] = 'u';
                        sbkv[head++] = 'l';
                        sbkv[head++] = ',';
                    }
                    sbkv[head] = ']'; // Replace last comma with close bracket
                } break;
                case BKV_I64: { // Name:Xl
                    // Val
                    int64_t v;
                    std::memcpy(&v, data + i + data[i], sizeof(int64_t));
                    v = Endianness::ntoh(v);

                    const std::string val = std::to_string(v);
                    checkResize(sbkv, head + data[i] + val.length() + 2, capacity);

                    // Name
                    std::memcpy(sbkv + head, data + i + 1, data[i]);
                    head += data[i];
                    i += data[i] + sizeof(int16_t);
                    sbkv[head++] = ':';

                    // Value
                    std::memcpy(sbkv + head, val.c_str(), val.length()); // Value
                    head += val.length();
                    sbkv[head++] = 'l';
                } break;
                case BKV_I64_ARRAY: { // Name:[Xl,Yl,Zl]
                    // Name
                    checkResize(sbkv, head + data[i] + 2, capacity);
                    std::memcpy(sbkv + head, data + i + 1, data[i]);
                    head += data[i];
                    i += data[i];
                    sbkv[head++] = ':';
                    sbkv[head++] = '[';
                    
                    // Array size
                    uint32_t size;
                    std::memcpy(&size, data + i, sizeof(uint32_t));
                    i += sizeof(uint32_t);
                    size = Endianness::ntoh(size);

                    // Values
                    int64_t v;
                    std::string val;
                    for (uint32_t index = 0; index < size; index++) {
                        std::memcpy(&v, data + i, sizeof(int64_t));
                        i += sizeof(int64_t);
                        v = Endianness::ntoh(v);
                        val = std::to_string(v);

                        checkResize(sbkv, head + val.length() + 2, capacity);
                        std::memcpy(sbkv + head, val.c_str(), val.length());
                        head += val.length();
                        sbkv[head++] = 'l';
                        sbkv[head++] = ',';
                    }
                    sbkv[head] = ']'; // Replace last comma with close bracket
                } break;
                case BKV_FLOAT: { // Name:X.f
                    // Val
                    float v;
                    std::memcpy(&v, data + i + data[i], sizeof(float));
                    v = Endianness::ntoh(v);

                    const std::string val = std::to_string(v);
                    checkResize(sbkv, head + data[i] + val.length() + 2, capacity);

                    // Name
                    std::memcpy(sbkv + head, data + i + 1, data[i]);
                    head += data[i];
                    i += data[i] + sizeof(float);
                    sbkv[head++] = ':';

                    // Value
                    std::memcpy(sbkv + head, val.c_str(), val.length()); // Value
                    head += val.length();
                    sbkv[head++] = 'f';
                } break;
                case BKV_FLOAT_ARRAY: { // Name:[X.f,Y.f,Z.f]
                    // Name
                    checkResize(sbkv, head + data[i] + 2, capacity);
                    std::memcpy(sbkv + head, data + i + 1, data[i]);
                    head += data[i];
                    i += data[i];
                    sbkv[head++] = ':';
                    sbkv[head++] = '[';
                    
                    // Array size
                    uint32_t size;
                    std::memcpy(&size, data + i, sizeof(uint32_t));
                    i += sizeof(uint32_t);
                    size = Endianness::ntoh(size);

                    // Values
                    float v;
                    std::string val;
                    for (uint32_t index = 0; index < size; index++) {
                        std::memcpy(&v, data + i, sizeof(float));
                        i += sizeof(float);
                        v = Endianness::ntoh(v);
                        val = std::to_string(v);

                        checkResize(sbkv, head + val.length() + 2, capacity);
                        std::memcpy(sbkv + head, val.c_str(), val.length());
                        head += val.length();
                        sbkv[head++] = 'f';
                        sbkv[head++] = ',';
                    }
                    sbkv[head] = ']'; // Replace last comma with close bracket
                } break;
                case BKV_DOUBLE: { // Name:X.
                    // Val
                    double v;
                    std::memcpy(&v, data + i + data[i], sizeof(double));
                    v = Endianness::ntoh(v);

                    const std::string val = std::to_string(v);
                    checkResize(sbkv, head + data[i] + val.length() + 1, capacity);

                    // Name
                    std::memcpy(sbkv + head, data + i + 1, data[i]);
                    head += data[i];
                    i += data[i] + sizeof(double);
                    sbkv[head++] = ':';

                    // Value
                    std::memcpy(sbkv + head, val.c_str(), val.length()); // Value
                    head += val.length();
                } break;
                case BKV_DOUBLE_ARRAY: { // Name:[X.,Y.,Z.]
                    // Name
                    checkResize(sbkv, head + data[i] + 2, capacity);
                    std::memcpy(sbkv + head, data + i + 1, data[i]);
                    head += data[i];
                    i += data[i];
                    sbkv[head++] = ':';
                    sbkv[head++] = '[';
                    
                    // Array size
                    uint32_t size;
                    std::memcpy(&size, data + i, sizeof(uint32_t));
                    i += sizeof(uint32_t);
                    size = Endianness::ntoh(size);

                    // Values
                    double v;
                    std::string val;
                    for (uint32_t index = 0; index < size; index++) {
                        std::memcpy(&v, data + i, sizeof(double));
                        i += sizeof(double);
                        v = Endianness::ntoh(v);
                        val = std::to_string(v);

                        checkResize(sbkv, head + val.length() + 1, capacity);
                        std::memcpy(sbkv + head, val.c_str(), val.length());
                        head += val.length();
                        sbkv[head++] = ',';
                    }
                    sbkv[head] = ']'; // Replace last comma with close bracket
                } break;
                case BKV_STR: { // Name:"Str"
                    // Val
                    size_t len;
                    std::memcpy(&len, data + i + data[i], sizeof(uint16_t));
                    len = Endianness::ntoh(len);

                    checkResize(sbkv, head + data[i] + len + 1, capacity);

                    // Name
                    std::memcpy(sbkv + head, data + i + 3, data[i]);
                    head += data[i];
                    i += data[i] + sizeof(uint16_t);
                    sbkv[head++] = ':';
                    sbkv[head++] = '"';

                    // Value
                    std::memcpy(sbkv + head, data + i, len); // Value
                    head += len;
                    i += len;
                    sbkv[head++] = '"';
                } break;
                    break;
                case BKV_STR_ARRAY: // Name:["Str1", "Str2", "Str3"]
                    // TODO
                    break;
            }
        }

        sbkv[head++] = '}';
        capacity = head + 1;
        sbkv = static_cast<char*>(std::realloc(sbkv, capacity));
        return UTF8Str{.len = capacity, .str = std::shared_ptr<char>(sbkv, std::free)};
    }
    
    BKV::BKV_t BKV::bkvFromSBKV(const UTF8Str& stringified) {
        // TODO
    }
    BKV::BKV_t BKV::bkvCompound(const UTF8Str& name) {
        if (name.len > 0xff) {
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

        if (name.len > 0xff) {
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

        if (name.len > 0xff) {
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
        std::memcpy(buffer + c - sizeof(uint32_t), &networkSize, sizeof(uint32_t)); // List size
        T networkData;
        for (size_t i = 0; i < size; i++) {
            networkData = Endianness::hton(data[i]);
            std::memcpy(buffer + listStart + (i * sizeof(T)), &networkData, sizeof(T)); // Data (after size)
        }

        return BKV_t{.size = allocSize, .data = std::shared_ptr<uint8_t>(buffer, std::free)};
    }
    BKV::BKV_t BKV::bkvStr(const UTF8Str& name, const UTF8Str& data) {
        if (name.len > 0xff) {
            std::stringstream msg;
            msg << "Too many characters in BKV name: " << name.len << "/255 characters.";
            throw std::length_error(msg.str());
        }
        if (data.len > 0xffff) {
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