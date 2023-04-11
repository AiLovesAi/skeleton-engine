#include "gm_sbkv.hpp"

#include "../../headers/string.hpp"
#include "../gm_endianness.hpp"

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
    template <> const char SBKV::BKVSuffixMap<float32_t>::suffix[] = "F";
    template <> const char SBKV::BKVSuffixMap<float128_t>::suffix[] = "";

    template <typename T>
    inline void setSBKVCopyValue(const uint8_t* data, char*& sbkv, int64_t& i, int64_t& head, const UTF8Str& val) {
        std::memcpy(sbkv + head, val.get(), val.length());
        head += val.length();
        std::memcpy(sbkv + head, SBKV::BKVSuffixMap<T>::suffix, sizeof(SBKV::BKVSuffixMap<T>::suffix) - 1);
        head += sizeof(SBKV::BKVSuffixMap<T>::suffix) - 1;
        sbkv[head++] = ',';
    }

    char getEscapeChar(const char c) {
        switch (c) {
            case '\"': return '\"';
            case '\'': return '\'';
            case '\\': return '\\';
            case '\0': return '0';
            case '\1': return '1';
            case '\2': return '2';
            case '\3': return '3';
            case '\4': return '4';
            case '\5': return '5';
            case '\6': return '6';
            case '\7': return 'a';
            case '\b': return 'b';
            case '\f': return 'f';
            case '\n': return 'n';
            case '\r': return 'r';
            case '\t': return 't';
            case '\v': return 'v';
            default: return -1;
        }
    }

    void setStr(const uint8_t* data, char*& sbkv, int64_t& i, int64_t& head, int64_t& capacity, const uint16_t len) {
        sbkv[head++] = '"';

        char c, escapeChar = -1;
        int16_t escapeChars = 0;
        for (int64_t j = 0; j < len; j++) {
            c = data[i++];
            escapeChar = getEscapeChar(c);
            if (escapeChar > 0) {
                escapeChars++;
                
                try {
                    StringBuffer::checkResize(sbkv, head + (len - j) + 2 + escapeChars, head, capacity);
                } catch (std::runtime_error& e) { throw; }

                sbkv[head++] = '\\';
                sbkv[head++] = escapeChar;
                escapeChar = -1;
            } else {
                sbkv[head++] = c;
            }
        }
        
        sbkv[head++] = '"';
    }

    template <typename T>
    inline void setSBKVValue(const uint8_t* data, char*& sbkv, int64_t& i, int64_t& head, int64_t& capacity) {
        const uint8_t keyLen = data[++i];
        T value;
        std::memcpy(&value, data + i + keyLen + 1, sizeof(T));
        value = Endianness::ntoh(value);

        const UTF8Str val = FormatString::intToStr(value);
        try {
            StringBuffer::checkResize(sbkv,
                static_cast<int64_t>(head + keyLen + val.length() + 2 + sizeof(SBKV::BKVSuffixMap<T>::suffix)),
                head, capacity
            );
        } catch (std::runtime_error& e) { throw; }

        // Key
        i += BKV::BKV_KEY_SIZE;
        setStr(data, sbkv, i, head, capacity, keyLen);
        i += sizeof(T);
        sbkv[head++] = ':';

        setSBKVCopyValue<T>(data, sbkv, i, head, val);
        i--;
    }

    template <typename T>
    inline void setSBKVArray(const uint8_t* data, char*& sbkv, int64_t& i, int64_t& head, int64_t& capacity) {
        const uint8_t keyLen = data[++i];
        try {
            StringBuffer::checkResize(sbkv, head + keyLen + 4, head, capacity);
        } catch (std::runtime_error& e) { throw; }
        i += BKV::BKV_KEY_SIZE;
        setStr(data, sbkv, i, head, capacity, keyLen);
        sbkv[head++] = ':';
        sbkv[head++] = '[';
        
        // Array size
        uint16_t size;
        std::memcpy(&size, data + i, BKV::BKV_ARRAY_SIZE);
        i += BKV::BKV_ARRAY_SIZE;
        size = Endianness::ntoh(size);

        // Values
        T v;
        UTF8Str val;
        for (uint16_t index = 0; index < size; index++) {
            std::memcpy(&v, data + i, sizeof(T));
            i += sizeof(T);
            v = Endianness::ntoh(v);
            val = FormatString::intToStr(v);

            try {
                StringBuffer::checkResize(sbkv,
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

    inline void setSBKVValueDouble(const uint8_t* data, char*& sbkv, int64_t& i, int64_t& head, int64_t& capacity) {
        const uint8_t keyLen = data[++i];
        float128_t v;
        std::memcpy(&v, data + i + keyLen + 1, sizeof(float128_t));
        v = Endianness::ntohf(v);

        const UTF8Str val = FormatString::floatToStr(v);
        try {
            StringBuffer::checkResize(sbkv,
                static_cast<int64_t>(head + keyLen + val.length() + 2 + sizeof(SBKV::BKVSuffixMap<float128_t>::suffix)),
                head, capacity
            );
        } catch (std::runtime_error& e) { throw; }

        // Key
        i += BKV::BKV_KEY_SIZE;
        setStr(data, sbkv, i, head, capacity, keyLen);
        i += sizeof(float128_t);
        sbkv[head++] = ':';

        // Value
        setSBKVCopyValue<float128_t>(data, sbkv, i, head, val);
        i--;
    }

    inline void setSBKVArrayDouble(const uint8_t* data, char*& sbkv, int64_t& i, int64_t& head, int64_t& capacity) {
        const uint8_t keyLen = data[++i];
        try {
            StringBuffer::checkResize(sbkv, head + keyLen + 4, head, capacity);
        } catch (std::runtime_error& e) { throw; }
        i += BKV::BKV_KEY_SIZE;
        setStr(data, sbkv, i, head, capacity, keyLen);
        sbkv[head++] = ':';
        sbkv[head++] = '[';
        
        // Array size
        uint16_t size;
        std::memcpy(&size, data + i, BKV::BKV_ARRAY_SIZE);
        i += BKV::BKV_ARRAY_SIZE;
        size = Endianness::ntoh(size);

        // Values
        float128_t v;
        UTF8Str val;
        for (uint16_t index = 0; index < size; index++) {
            std::memcpy(&v, data + i, sizeof(float128_t));
            i += sizeof(float128_t);
            v = Endianness::ntohf(v);
            val = FormatString::floatToStr(v);

            try {
                StringBuffer::checkResize(sbkv,
                    static_cast<int64_t>(head + val.length() + sizeof(SBKV::BKVSuffixMap<float128_t>::suffix) + 1),
                    head, capacity
                );
            } catch (std::runtime_error& e) { throw; }
            setSBKVCopyValue<float128_t>(data, sbkv, i, head, val);
        }
        sbkv[head - 1] = ']'; // Replace last comma with close bracket
        sbkv[head++] = ',';
    }

    inline void setSBKVValueFloat(const uint8_t* data, char*& sbkv, int64_t& i, int64_t& head, int64_t& capacity) {
        const uint8_t keyLen = data[++i];
        float32_t v;
        std::memcpy(&v, data + i + keyLen + 1, sizeof(float32_t));
        v = Endianness::ntohf(v);

        const UTF8Str val = FormatString::floatToStr(v);
        try {
            StringBuffer::checkResize(sbkv,
                static_cast<int64_t>(head + keyLen + val.length() + 2 + sizeof(SBKV::BKVSuffixMap<float32_t>::suffix)),
                head, capacity
            );
        } catch (std::runtime_error& e) { throw; }

        // Key
        i += BKV::BKV_KEY_SIZE;
        setStr(data, sbkv, i, head, capacity, keyLen);
        i += sizeof(float32_t);
        sbkv[head++] = ':';

        // Value
        setSBKVCopyValue<float32_t>(data, sbkv, i, head, val);
        i--;
    }

    inline void setSBKVArrayFloat(const uint8_t* data, char*& sbkv, int64_t& i, int64_t& head, int64_t& capacity) {
        const uint8_t keyLen = data[++i];
        try {
            StringBuffer::checkResize(sbkv, head + keyLen + 4, head, capacity);
        } catch (std::runtime_error& e) { throw; }
        i += BKV::BKV_KEY_SIZE;
        setStr(data, sbkv, i, head, capacity, keyLen);
        sbkv[head++] = ':';
        sbkv[head++] = '[';
        
        // Array size
        uint16_t size;
        std::memcpy(&size, data + i, BKV::BKV_ARRAY_SIZE);
        i += BKV::BKV_ARRAY_SIZE;
        size = Endianness::ntoh(size);

        // Values
        float32_t v;
        UTF8Str val;
        for (uint16_t index = 0; index < size; index++) {
            std::memcpy(&v, data + i, sizeof(float32_t));
            i += sizeof(float32_t);
            v = Endianness::ntohf(v);
            val = FormatString::floatToStr(v);

            try {
                StringBuffer::checkResize(sbkv,
                    static_cast<int64_t>(head + val.length() + sizeof(SBKV::BKVSuffixMap<float32_t>::suffix) + 1),
                    head, capacity
                );
            } catch (std::runtime_error& e) { throw; }
            setSBKVCopyValue<float32_t>(data, sbkv, i, head, val);
        }
        sbkv[head - 1] = ']'; // Replace last comma with close bracket
        sbkv[head++] = ',';
    }

    inline void setSBKVValueString(const uint8_t* data, char*& sbkv, int64_t& i, int64_t& head, int64_t& capacity) {
        const uint8_t keyLen = data[++i];
        uint16_t len;
        std::memcpy(&len, data + i + BKV::BKV_KEY_SIZE + keyLen, BKV::BKV_STR_SIZE);
        len = Endianness::ntoh(len);

        try {
            StringBuffer::checkResize(sbkv, head + keyLen + len + 4, head, capacity);
        } catch (std::runtime_error& e) { throw; }

        // Key
        i += BKV::BKV_KEY_SIZE;
        setStr(data, sbkv, i, head, capacity, keyLen);
        i += BKV::BKV_STR_SIZE;
        sbkv[head++] = ':';

        // Value
        try {
            setStr(data, sbkv, i, head, capacity, len);
        } catch (std::runtime_error& e) { throw; }

        sbkv[head++] = ',';
        i--;
    }

    inline void setSBKVArrayString(const uint8_t* data, char*& sbkv, int64_t& i, int64_t& head, int64_t& capacity) {
        const uint8_t keyLen = data[++i];
        try {
            StringBuffer::checkResize(sbkv, head + keyLen + 4, head, capacity);
        } catch (std::runtime_error& e) { throw; }
        i += BKV::BKV_KEY_SIZE;
        setStr(data, sbkv, i, head, capacity, keyLen);
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

            StringBuffer::checkResize(sbkv, head + len + 4, head, capacity);
            try {
                setStr(data, sbkv, i, head, capacity, len);
            } catch (std::runtime_error& e) { throw; }
            sbkv[head++] = ',';
        }
        sbkv[head - 1] = ']'; // Replace last comma with close bracket
        sbkv[head++] = ',';
        i--;
    }

    inline void setSBKVValueBool(const uint8_t* data, char*& sbkv, int64_t& i, int64_t& head, int64_t& capacity) {
        const uint8_t keyLen = data[++i];

        try {
            StringBuffer::checkResize(sbkv, head + keyLen + 6, head, capacity);
        } catch (std::runtime_error& e) { throw; }

        // Key
        std::memcpy(sbkv + head, data + i + BKV::BKV_KEY_SIZE, keyLen);
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
    }

    inline void openSBKVCompound(const uint8_t* data, char*& sbkv, int64_t& i, int64_t& head, int64_t& depth, int64_t& capacity) {
        depth++;
        const uint8_t keyLen = data[++i];
        if (depth <= 1) {
            try {
                StringBuffer::checkResize(sbkv, head + 1, head, capacity);
            } catch (std::runtime_error& e) { throw; }

            sbkv[head++] = '{';
        } else {
            try {
                StringBuffer::checkResize(sbkv, head + keyLen + 2, head, capacity);
            } catch (std::runtime_error& e) { throw; }

            // Key
            i += BKV::BKV_KEY_SIZE;
            setStr(data, sbkv, i, head, capacity, keyLen);

            sbkv[head++] = ':';
            sbkv[head++] = '{';
            i--;
        }

        i += BKV::BKV_COMPOUND_SIZE;
    }

    inline void closeSBKVCompound(char*& sbkv, int64_t& i, int64_t& head, int64_t& depth, int64_t& capacity) {
        depth--;
        sbkv[head - 1] = '}'; // Replace last comma with close brace
        if (depth > 0) {
            try {
                StringBuffer::checkResize(sbkv, head + 1, head, capacity);
            } catch (std::runtime_error& e) { throw; }
            sbkv[head++] = ',';
        }
    }

    inline void sbkvParse(const uint8_t* data, char*& sbkv, int64_t& i, int64_t& head, int64_t& depth, int64_t& capacity) {
        switch(data[i]) {
            case BKV::BKV_END: // },
                closeSBKVCompound(sbkv, i, head, depth, capacity);
                break;
            case BKV::BKV_COMPOUND: // Key:{
                openSBKVCompound(data, sbkv, i, head, depth, capacity);
                break;
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
            case BKV::BKV_STR: // Key:Str,
                setSBKVValueString(data, sbkv, i, head, capacity);
                break;
            case BKV::BKV_STR_ARRAY: // Key:[Str1,Str2,Str3],
                setSBKVArrayString(data, sbkv, i, head, capacity);
                break;
            case BKV::BKV_BOOL: // Key:true/false,
                setSBKVValueBool(data, sbkv, i, head, capacity);
                break;
            default: {
                throw std::runtime_error(FormatString::formatString(
                    "Invalid character in BKV at index %ld: %02x.",
                    i, data[i]
                ).get());
            }
        }
    }

    UTF8Str SBKV::sbkvFromBKV(const BKV& bkv) {
        int64_t capacity = bkv.size(); // Should be at least bkvSize
        char* sbkv = static_cast<char*>(std::malloc(capacity));

        const uint8_t* data = bkv.get();
        int64_t head, i, depth;
        for (head = 0, i = 0, depth = 0; i < bkv.size(); i++) {
            sbkvParse(data, sbkv, i, head, depth, capacity);
        }

        // Null terminate string
        try {
            StringBuffer::checkResize(sbkv, head + 1, head, capacity);
        } catch (std::runtime_error& e) { throw; }
        sbkv[head++] = '\0';

        // Reallocate to use only as much memory as necessary
        capacity = head;
        sbkv = static_cast<char*>(std::realloc(sbkv, capacity));
        return UTF8Str{sbkv, capacity - 1};
    }
}
