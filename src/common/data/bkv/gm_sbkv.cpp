#include "gm_sbkv.hpp"

#include "../gm_buffer_memory.hpp"
#include "../gm_endianness.hpp"

#include <cstring>
#include <memory>
#include <stdexcept>

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

    template <typename T>
    void setSBKVCopyValue(const uint8_t* data, char*& sbkv, int64_t& i, int64_t& head, const std::string& val) {
        std::memcpy(sbkv + head, val.c_str(), val.length());
        head += val.length();
        std::memcpy(sbkv + head, BKV::BKVSuffixMap<T>::suffix, sizeof(BKV::BKVSuffixMap<T>::suffix) - 1);
        head += sizeof(BKV::BKVSuffixMap<T>::suffix) - 1;
        sbkv[head++] = ',';
    }

    template <typename T>
    void setSBKVValue(const uint8_t* data, char*& sbkv, int64_t& i, int64_t& head, int64_t& capacity) {
        const uint8_t keyLen = data[++i];
        T value;
        std::memcpy(&value, data + i + keyLen + 1, sizeof(T));
        value = Endianness::ntoh(value);

        const std::string val = std::to_string(value);
        try {
            BufferMemory::checkResize(sbkv,
                static_cast<int64_t>(head + keyLen + val.length() + sizeof(BKV::BKVSuffixMap<T>::suffix)),
                head, capacity
            );
        } catch (std::runtime_error& e) { throw; }

        // Key
        std::memcpy(sbkv + head, data + i + 1, keyLen);
        head += keyLen;
        i+= BKV::BKV_KEY_SIZE + keyLen + sizeof(T);
        sbkv[head++] = ':';

        setSBKVCopyValue<T>(data, sbkv, i, head, val);
    }

    template <typename T>
    void setSBKVArray(const uint8_t* data, char*& sbkv, int64_t& i,  int64_t& head, int64_t& capacity) {
        const uint8_t keyLen = data[++i];
        try {
            BufferMemory::checkResize(sbkv, head + keyLen + 2, head, capacity);
        } catch (std::runtime_error& e) { throw; }
        std::memcpy(sbkv + head, data + i + 1, keyLen);
        head += keyLen;
        i+= BKV::BKV_KEY_SIZE + keyLen;
        sbkv[head++] = ':';
        sbkv[head++] = '[';
        
        // Array size
        uint16_t size;
        std::memcpy(&size, data + i, BKV::BKV_ARRAY_SIZE);
        i += BKV::BKV_ARRAY_SIZE;
        size = Endianness::ntoh(size);

        // Values
        T v;
        std::string val;
        for (uint16_t index = 0; index < size; index++) {
            std::memcpy(&v, data + i, sizeof(T));
            i += sizeof(T);
            v = Endianness::ntoh(v);
            val = std::to_string(v);

            try {
                BufferMemory::checkResize(sbkv,
                    static_cast<int64_t>(head + val.length() + sizeof(BKV::BKVSuffixMap<T>::suffix) + 1),
                    head, capacity
                );
            } catch (std::runtime_error& e) { throw; }
            setSBKVCopyValue<T>(data, sbkv, i, head, val);
        }
        sbkv[head - 1] = ']'; // Replace last comma with close bracket
        sbkv[head++] = ',';
    }

    void setSBKVValueDouble(const uint8_t* data, char*& sbkv, int64_t& i, int64_t& head, int64_t& capacity) {
        const uint8_t keyLen = data[++i];
        double v;
        std::memcpy(&v, data + i + keyLen + 1, sizeof(double));
        v = Endianness::ntohd(v);

        const std::string val = std::to_string(v);
        try {
            BufferMemory::checkResize(sbkv,
                static_cast<int64_t>(head + keyLen + val.length() + sizeof(BKV::BKVSuffixMap<double>::suffix)),
                head, capacity
            );
        } catch (std::runtime_error& e) { throw; }

        // Key
        std::memcpy(sbkv + head, data + i + 1, keyLen);
        head += keyLen;
        i+= BKV::BKV_KEY_SIZE + keyLen + sizeof(double);
        sbkv[head++] = ':';

        // Value
        setSBKVCopyValue<double>(data, sbkv, i, head, val);
    }

    void setSBKVArrayDouble(const uint8_t* data, char*& sbkv, int64_t& i,  int64_t& head, int64_t& capacity) {
        const uint8_t keyLen = data[++i];
        try {
            BufferMemory::checkResize(sbkv, head + keyLen + 2, head, capacity);
        } catch (std::runtime_error& e) { throw; }
        std::memcpy(sbkv + head, data + i + 1, keyLen);
        head += keyLen;
        i+= BKV::BKV_KEY_SIZE + keyLen;
        sbkv[head++] = ':';
        sbkv[head++] = '[';
        
        // Array size
        uint16_t size;
        std::memcpy(&size, data + i, BKV::BKV_ARRAY_SIZE);
        i += BKV::BKV_ARRAY_SIZE;
        size = Endianness::ntoh(size);

        // Values
        double v;
        std::string val;
        for (uint16_t index = 0; index < size; index++) {
            std::memcpy(&v, data + i, sizeof(double));
            i += sizeof(double);
            v = Endianness::ntohd(v);
            val = std::to_string(v);

            try {
                BufferMemory::checkResize(sbkv,
                    static_cast<int64_t>(head + val.length() + sizeof(BKV::BKVSuffixMap<double>::suffix) + 1),
                    head, capacity
                );
            } catch (std::runtime_error& e) { throw; }
            setSBKVCopyValue<double>(data, sbkv, i, head, val);
        }
        sbkv[head - 1] = ']'; // Replace last comma with close bracket
        sbkv[head++] = ',';
    }

    void setSBKVValueFloat(const uint8_t* data, char*& sbkv, int64_t& i, int64_t& head, int64_t& capacity) {
        const uint8_t keyLen = data[++i];
        float v;
        std::memcpy(&v, data + i + keyLen + 1, sizeof(float));
        v = Endianness::ntohf(v);

        const std::string val = std::to_string(v);
        try {
            BufferMemory::checkResize(sbkv,
                static_cast<int64_t>(head + keyLen + val.length() + sizeof(BKV::BKVSuffixMap<float>::suffix)),
                head, capacity
            );
        } catch (std::runtime_error& e) { throw; }

        // Key
        std::memcpy(sbkv + head, data + i + 1, keyLen);
        head += keyLen;
        i+= BKV::BKV_KEY_SIZE + keyLen + sizeof(float);
        sbkv[head++] = ':';

        // Value
        setSBKVCopyValue<float>(data, sbkv, i, head, val);
    }

    void setSBKVArrayFloat(const uint8_t* data, char*& sbkv, int64_t& i,  int64_t& head, int64_t& capacity) {
        const uint8_t keyLen = data[++i];
        try {
            BufferMemory::checkResize(sbkv, head + keyLen + 2, head, capacity);
        } catch (std::runtime_error& e) { throw; }
        std::memcpy(sbkv + head, data + i + 1, keyLen);
        head += keyLen;
        i+= BKV::BKV_KEY_SIZE + keyLen;
        sbkv[head++] = ':';
        sbkv[head++] = '[';
        
        // Array size
        uint16_t size;
        std::memcpy(&size, data + i, BKV::BKV_ARRAY_SIZE);
        i += BKV::BKV_ARRAY_SIZE;
        size = Endianness::ntoh(size);

        // Values
        float v;
        std::string val;
        for (uint16_t index = 0; index < size; index++) {
            std::memcpy(&v, data + i, sizeof(float));
            i += sizeof(float);
            v = Endianness::ntohf(v);
            val = std::to_string(v);

            try {
                BufferMemory::checkResize(sbkv,
                    static_cast<int64_t>(head + val.length() + sizeof(BKV::BKVSuffixMap<float>::suffix) + 1),
                    head, capacity
                );
            } catch (std::runtime_error& e) { throw; }
            setSBKVCopyValue<float>(data, sbkv, i, head, val);
        }
        sbkv[head - 1] = ']'; // Replace last comma with close bracket
        sbkv[head++] = ',';
    }

    UTF8Str SBKV::sbkvFromBKV(const BKV_t& bkv) {
        int64_t capacity = bkv.size; // Should be at least bkvSize
        char* sbkv = static_cast<char*>(std::malloc(capacity));

        const uint8_t* data = bkv.data.get();
        int64_t head, i, depth;
        for (head = 0, i = 0, depth = 0; i < bkv.size; i++) {
            switch(data[i]) {
                case BKV::BKV_END:
                    depth--;
                    sbkv[head - 1] = '}'; // Replace last comma with close brace
                    if (depth > 0) {
                        try {
                            BufferMemory::checkResize(sbkv, head + 1, head, capacity);
                        } catch (std::runtime_error& e) { throw; }
                        sbkv[head++] = ',';
                    }
                    break;
                case BKV::BKV_COMPOUND: { // Key:{
                    depth++;
                    const uint8_t keyLen = data[++i];
                    if (depth <= 1) {
                        try {
                            BufferMemory::checkResize(sbkv, head + 1, head, capacity);
                        } catch (std::runtime_error& e) { throw; }

                        sbkv[head++] = '{';
                    } else {
                        try {
                            BufferMemory::checkResize(sbkv, head + keyLen + 2, head, capacity);
                        } catch (std::runtime_error& e) { throw; }

                        // Key
                        std::memcpy(sbkv + head, data + i + 1, keyLen);
                        head += keyLen;

                        sbkv[head++] = ':';
                        sbkv[head++] = '{';
                    }

                    i += BKV::BKV_KEY_SIZE + keyLen + BKV::BKV_COMPOUND_SIZE;
                } break;
                case BKV::BKV_UI8: // Key:Xub
                    setSBKVValue<uint8_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_UI8_ARRAY: // Key:[Xub,Yub,Zub],
                    setSBKVArray<uint8_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_I8: // Key:Xb,
                    setSBKVValue<int8_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_I8_ARRAY: // Key:[Xb,Yb,Zb],
                    setSBKVArray<int8_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_UI16: // Key:Xus,
                    setSBKVValue<uint16_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_UI16_ARRAY: // Key:[Xus,Yus,Zus],
                    setSBKVArray<uint16_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_I16: // Key:Xs,
                    setSBKVValue<int16_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_I16_ARRAY: // Key:[Xs,Ys,Zs]
                    setSBKVArray<int16_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_UI32: // Key:Xu,
                    setSBKVValue<uint32_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_UI32_ARRAY: // Key:[Xu,Yu,Zu]
                    setSBKVArray<uint32_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_I32: // Key:X,
                    setSBKVValue<int32_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_I32_ARRAY: // Key:[X,Y,Z],
                    setSBKVArray<int32_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_UI64: // Key:Xul,
                    setSBKVValue<uint64_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_UI64_ARRAY: // Key:[Xul,Yul,Zul],
                    setSBKVArray<uint64_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_I64: // Key:Xl,
                    setSBKVValue<int64_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_I64_ARRAY: // Key:[Xl,Yl,Zl],
                    setSBKVArray<int64_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_FLOAT: // Key:X.f,
                    setSBKVValueFloat(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_FLOAT_ARRAY: // Key:[X.f,Y.f,Z.f],
                    setSBKVArrayFloat(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_DOUBLE: // Key:X.,
                    setSBKVValueDouble(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_DOUBLE_ARRAY: // Key:[X.,Y.,Z.],
                    setSBKVArrayDouble(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_STR: { // Key:Str,
                    const uint8_t keyLen = data[++i];
                    uint16_t len;
                    std::memcpy(&len, data + i + 1 + keyLen, BKV::BKV_STR_SIZE);
                    len = Endianness::ntoh(len);

                    try {
                        BufferMemory::checkResize(sbkv, head + keyLen + len + 2, head, capacity);
                    } catch (std::runtime_error& e) { throw; }

                    // Key
                    std::memcpy(sbkv + head, data + i + 1, keyLen);
                    head += keyLen;
                    i+= BKV::BKV_KEY_SIZE + keyLen + BKV::BKV_STR_SIZE;
                    sbkv[head++] = ':';

                    // Value
                    std::memcpy(sbkv + head, data + i, len);
                    head += len;
                    i += len;
                    sbkv[head++] = ',';
                } break;
                case BKV::BKV_STR_ARRAY: { // Key:[Str1,Str2,Str3],
                    const uint8_t keyLen = data[++i];
                    try {
                        BufferMemory::checkResize(sbkv, head + keyLen + 2, head, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(sbkv + head, data + i + 1, keyLen);
                    head += keyLen;
                    i+= BKV::BKV_KEY_SIZE + keyLen;
                    sbkv[head++] = ':';
                    sbkv[head++] = '[';

                    // Array size
                    uint16_t size;
                    std::memcpy(&size, data + i, BKV::BKV_ARRAY_SIZE);
                    i += BKV::BKV_ARRAY_SIZE;
                    size = Endianness::ntoh(size);

                    // Values
                    uint16_t len;
                    std::string str;
                    for (uint32_t index = 0; index < size; index++) {
                        std::memcpy(&len, data + i, BKV::BKV_STR_SIZE);
                        i += BKV::BKV_STR_SIZE;
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

        capacity = head + 1;
        sbkv = static_cast<char*>(std::realloc(sbkv, capacity));
        return UTF8Str{.len = capacity, .str = std::shared_ptr<char>(sbkv, std::free)};
    }
}
