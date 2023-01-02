#include "gm_sbkv.hpp"

#include "../gm_buffer_memory.hpp"
#include "../gm_endianness.hpp"

#include "../gm_logger.hpp"
#include <sstream>

#include <cstring>
#include <memory>
#include <stdexcept>

namespace game {
    template <> const char SBKV::BKVSuffixMap<uint8_t>::suffix[] = "UB";
    template <> const char SBKV::BKVSuffixMap<int8_t>::suffix[] = "B";
    template <> const char SBKV::BKVSuffixMap<uint16_t>::suffix[] = "US";
    template <> const char SBKV::BKVSuffixMap<int16_t>::suffix[] = "S";
    template <> const char SBKV::BKVSuffixMap<uint32_t>::suffix[] = "U";
    template <> const char SBKV::BKVSuffixMap<int32_t>::suffix[] = "";
    template <> const char SBKV::BKVSuffixMap<uint64_t>::suffix[] = "UL";
    template <> const char SBKV::BKVSuffixMap<int64_t>::suffix[] = "L";
    template <> const char SBKV::BKVSuffixMap<float>::suffix[] = "F";
    template <> const char SBKV::BKVSuffixMap<double>::suffix[] = "";

    template <typename T>
    void setSBKVCopyValue(const uint8_t* data, char*& sbkv, int64_t& i, int64_t& head, const std::string& val) {
        std::memcpy(sbkv + head, val.c_str(), val.length());
        head += val.length();
        std::memcpy(sbkv + head, SBKV::BKVSuffixMap<T>::suffix, sizeof(SBKV::BKVSuffixMap<T>::suffix) - 1);
        head += sizeof(SBKV::BKVSuffixMap<T>::suffix) - 1;
        sbkv[head++] = ',';
    }

    template <typename T>
    void setSBKVValue(const uint8_t* data, char*& sbkv, int64_t& i, int64_t& head, int64_t& capacity) {
        const uint8_t keyLen = data[++i];
    std::stringstream m;
    m << "Key length: " << std::to_string(keyLen);
    Logger::log(LOG_INFO, m.str());
        T value;
        std::memcpy(&value, data + i + keyLen + 1, sizeof(T));
        value = Endianness::ntoh(value);

        const std::string val = std::to_string(value);
        try {
            BufferMemory::checkResize(sbkv,
                static_cast<int64_t>(head + keyLen + val.length() + sizeof(SBKV::BKVSuffixMap<T>::suffix)),
                head, capacity
            );
        } catch (std::runtime_error& e) { throw; }

        // Key
        std::memcpy(sbkv + head, data + i + 1, keyLen);
        head += keyLen;
        i += BKV::BKV_KEY_SIZE + keyLen + sizeof(T);
        sbkv[head++] = ':';

        setSBKVCopyValue<T>(data, sbkv, i, head, val);
        i--;
    }

    template <typename T>
    void setSBKVArray(const uint8_t* data, char*& sbkv, int64_t& i,  int64_t& head, int64_t& capacity) {
        const uint8_t keyLen = data[++i];
    std::stringstream m;
    m << "Key length: " << std::to_string(keyLen);
    Logger::log(LOG_INFO, m.str());
        try {
            BufferMemory::checkResize(sbkv, head + keyLen + 2, head, capacity);
        } catch (std::runtime_error& e) { throw; }
        std::memcpy(sbkv + head, data + i + 1, keyLen);
        head += keyLen;
        i += BKV::BKV_KEY_SIZE + keyLen;
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
                    static_cast<int64_t>(head + val.length() + sizeof(SBKV::BKVSuffixMap<T>::suffix) + 1),
                    head, capacity
                );
            } catch (std::runtime_error& e) { throw; }
            setSBKVCopyValue<T>(data, sbkv, i, head, val);
        }
        sbkv[head - 1] = ']'; // Replace last comma with close bracket
        sbkv[head++] = ',';
        i--;
    }

    void setSBKVValueDouble(const uint8_t* data, char*& sbkv, int64_t& i, int64_t& head, int64_t& capacity) {
        const uint8_t keyLen = data[++i];
    std::stringstream m;
    m << "Key length: " << std::to_string(keyLen);
    Logger::log(LOG_INFO, m.str());
        double v;
        std::memcpy(&v, data + i + keyLen + 1, sizeof(double));
        v = Endianness::ntohd(v);

        const std::string val = std::to_string(v);
        try {
            BufferMemory::checkResize(sbkv,
                static_cast<int64_t>(head + keyLen + val.length() + sizeof(SBKV::BKVSuffixMap<double>::suffix)),
                head, capacity
            );
        } catch (std::runtime_error& e) { throw; }

        // Key
        std::memcpy(sbkv + head, data + i + 1, keyLen);
        head += keyLen;
        i += keyLen + sizeof(double);
        sbkv[head++] = ':';

        // Value
        setSBKVCopyValue<double>(data, sbkv, i, head, val);
    }

    void setSBKVArrayDouble(const uint8_t* data, char*& sbkv, int64_t& i,  int64_t& head, int64_t& capacity) {
        const uint8_t keyLen = data[++i];
    std::stringstream m;
    m << "Key length: " << std::to_string(keyLen);
    Logger::log(LOG_INFO, m.str());
        try {
            BufferMemory::checkResize(sbkv, head + keyLen + 2, head, capacity);
        } catch (std::runtime_error& e) { throw; }
        std::memcpy(sbkv + head, data + i + 1, keyLen);
        head += keyLen;
        i += BKV::BKV_KEY_SIZE + keyLen;
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
                    static_cast<int64_t>(head + val.length() + sizeof(SBKV::BKVSuffixMap<double>::suffix) + 1),
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
    std::stringstream m;
    m << "Key length: " << std::to_string(keyLen);
    Logger::log(LOG_INFO, m.str());
        float v;
        std::memcpy(&v, data + i + keyLen + 1, sizeof(float));
        v = Endianness::ntohf(v);

        const std::string val = std::to_string(v);
        try {
            BufferMemory::checkResize(sbkv,
                static_cast<int64_t>(head + keyLen + val.length() + sizeof(SBKV::BKVSuffixMap<float>::suffix)),
                head, capacity
            );
        } catch (std::runtime_error& e) { throw; }

        // Key
        std::memcpy(sbkv + head, data + i + 1, keyLen);
        head += keyLen;
        i += BKV::BKV_KEY_SIZE + keyLen + sizeof(float);
        sbkv[head++] = ':';

        // Value
        setSBKVCopyValue<float>(data, sbkv, i, head, val);
        i--;
    }

    void setSBKVArrayFloat(const uint8_t* data, char*& sbkv, int64_t& i,  int64_t& head, int64_t& capacity) {
        const uint8_t keyLen = data[++i];
    std::stringstream m;
    m << "Key length: " << std::to_string(keyLen);
    Logger::log(LOG_INFO, m.str());
        try {
            BufferMemory::checkResize(sbkv, head + keyLen + 2, head, capacity);
        } catch (std::runtime_error& e) { throw; }
        std::memcpy(sbkv + head, data + i + 1, keyLen);
        head += keyLen;
        i += BKV::BKV_KEY_SIZE + keyLen;
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
                    static_cast<int64_t>(head + val.length() + sizeof(SBKV::BKVSuffixMap<float>::suffix) + 1),
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
            std::stringstream m;
            m << "Parsing character: 0x" << std::hex << ((data[i] & 0xf0) >> 4) << std::hex << (data[i] & 0xf);
            Logger::log(LOG_INFO, m.str());
            switch(data[i]) {
                case BKV::BKV_END:
                Logger::log(LOG_INFO, "Closing compound.");
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
                    m.str("");
                    m << "Parsing compound with key length: " << std::to_string(keyLen);
                    Logger::log(LOG_INFO, m.str());
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

                    i += keyLen + BKV::BKV_COMPOUND_SIZE;
                } break;
                case BKV::BKV_UI8: // Key:Xub
                    Logger::log(LOG_INFO, "Parsing UI8");
                    setSBKVValue<uint8_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_UI8_ARRAY: // Key:[Xub,Yub,Zub],
                    Logger::log(LOG_INFO, "Parsing UI8 Array");
                    setSBKVArray<uint8_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_I8: // Key:Xb,
                    Logger::log(LOG_INFO, "Parsing I8");
                    setSBKVValue<int8_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_I8_ARRAY: // Key:[Xb,Yb,Zb],
                    Logger::log(LOG_INFO, "Parsing I8 Array");
                    setSBKVArray<int8_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_UI16: // Key:Xus,
                    Logger::log(LOG_INFO, "Parsing UI16");
                    setSBKVValue<uint16_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_UI16_ARRAY: // Key:[Xus,Yus,Zus],
                    Logger::log(LOG_INFO, "Parsing UI16 Array");
                    setSBKVArray<uint16_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_I16: // Key:Xs,
                    Logger::log(LOG_INFO, "Parsing I16");
                    setSBKVValue<int16_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_I16_ARRAY: // Key:[Xs,Ys,Zs]
                    Logger::log(LOG_INFO, "Parsing I16 Array");
                    setSBKVArray<int16_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_UI32: // Key:Xu,
                    Logger::log(LOG_INFO, "Parsing UI32");
                    setSBKVValue<uint32_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_UI32_ARRAY: // Key:[Xu,Yu,Zu]
                    Logger::log(LOG_INFO, "Parsing UI32 Array");
                    setSBKVArray<uint32_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_I32: // Key:X,
                    Logger::log(LOG_INFO, "Parsing I32");
                    setSBKVValue<int32_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_I32_ARRAY: // Key:[X,Y,Z],
                    Logger::log(LOG_INFO, "Parsing I32 Array");
                    setSBKVArray<int32_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_UI64: // Key:Xul,
                    Logger::log(LOG_INFO, "Parsing UI64");
                    setSBKVValue<uint64_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_UI64_ARRAY: // Key:[Xul,Yul,Zul],
                    Logger::log(LOG_INFO, "Parsing UI64 Array");
                    setSBKVArray<uint64_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_I64: // Key:Xl,
                    Logger::log(LOG_INFO, "Parsing I64");
                    setSBKVValue<int64_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_I64_ARRAY: // Key:[Xl,Yl,Zl],
                    Logger::log(LOG_INFO, "Parsing I64 Array");
                    setSBKVArray<int64_t>(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_FLOAT: // Key:X.f,
                    Logger::log(LOG_INFO, "Parsing Float");
                    setSBKVValueFloat(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_FLOAT_ARRAY: // Key:[X.f,Y.f,Z.f],
                    Logger::log(LOG_INFO, "Parsing Float Array");
                    setSBKVArrayFloat(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_DOUBLE: // Key:X.,
                    Logger::log(LOG_INFO, "Parsing Double");
                    setSBKVValueDouble(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_DOUBLE_ARRAY: // Key:[X.,Y.,Z.],
                    Logger::log(LOG_INFO, "Parsing Double Array");
                    setSBKVArrayDouble(data, sbkv, i, head, capacity);
                    break;
                case BKV::BKV_STR: { // Key:Str,
                    Logger::log(LOG_INFO, "Parsing String");
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
                    i += BKV::BKV_KEY_SIZE + keyLen + BKV::BKV_STR_SIZE;
                    sbkv[head++] = ':';

                    // Value
                    std::memcpy(sbkv + head, data + i, len);
                    head += len;
                    i += len;
                    sbkv[head++] = ',';
                    i--;
                } break;
                case BKV::BKV_STR_ARRAY: { // Key:[Str1,Str2,Str3],
                    Logger::log(LOG_INFO, "Parsing String Array");
                    const uint8_t keyLen = data[++i];
                    try {
                        BufferMemory::checkResize(sbkv, head + keyLen + 2, head, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(sbkv + head, data + i + 1, keyLen);
                    head += keyLen;
                    i += BKV::BKV_KEY_SIZE + keyLen;
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
                    i--;
                } break;
                case BKV::BKV_BOOL: {
                    Logger::log(LOG_INFO, "Parsing Boolean");
                    const uint8_t keyLen = data[++i];

                    try {
                        BufferMemory::checkResize(sbkv, head + keyLen + 6, head, capacity);
                    } catch (std::runtime_error& e) { throw; }

                    // Key
                    std::memcpy(sbkv + head, data + i + 1, keyLen);
                    head += keyLen;
                    i += BKV::BKV_KEY_SIZE + keyLen;
                    sbkv[head++] = ':';

                    // Value
                    if (data[i]) {
                        std::memcpy(sbkv + head, "true", 4);
                        head += 4;
                    } else {
                        std::memcpy(sbkv + head, "false", 5);
                        head += 5;
                    }
                    sbkv[head++] = ',';
                } break;
                default: {
                    std::stringstream msg;
                    msg << "Invalid character in BKV at index " << i << ": 0x" << std::hex << ((data[i] & 0xf0) >> 4) << std::hex << (data[i] & 0xf);
                    throw std::runtime_error(msg.str());
                }
            }
        }

        try {
            BufferMemory::checkResize(sbkv, head + 1, head, capacity);
        } catch (std::runtime_error& e) { throw; }
        sbkv[head++] = '\0';

        capacity = head;
        sbkv = static_cast<char*>(std::realloc(sbkv, capacity));
        return UTF8Str{.len = capacity, .str = std::shared_ptr<char>(sbkv, std::free)};
    }
}
