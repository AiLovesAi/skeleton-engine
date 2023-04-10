#include "gm_bkv_builder.hpp"

#include "../gm_endianness.hpp"

namespace game {
    [[nodiscard]] BKV BKV_Builder::build() {
        // Input validation
        if (_depth.size()) {
            throw std::runtime_error(FormatString::formatString(
                "There are %lu unclosed compounds in BKV Builder in build().",
                _depth.size()
            ).get());
        }
        
        // Add full compound size
        // NOTE: Would be two, but the closing compound has not been added
        int64_t size = _buffer._head - 1 - BKV::BKV_COMPOUND_SIZE;
        if ((size <= 0) || (size > BKV::BKV_COMPOUND_MAX)) {
            throw std::runtime_error(FormatString::formatString(
                "BKV Builder main compound bigger than maximum size: %ld/%u",
                size, BKV::BKV_COMPOUND_MAX
            ).get());
        }

        // Set compound length
        const uint32_t len = Endianness::hton(static_cast<uint32_t>(size));
        std::memcpy(_buffer._bkv + 2, &len, BKV::BKV_COMPOUND_SIZE);

        // Copy buffer to resulting BKV
        uint8_t* buffer = static_cast<uint8_t*>(std::malloc(_buffer._head + 1));
        std::memcpy(buffer, _buffer._bkv, _buffer._head);

        // Close main compound
        buffer[_buffer._head] = BKV::BKV_END;

        std::shared_ptr<const uint8_t> data(buffer, std::free);
        return BKV(_buffer._head + 1, data);
    }

    void BKV_Builder::_setKeyTag(const UTF8Str& key, const uint8_t tag, const uint64_t size) {
        // Input validation
        if (key.length() > BKV::BKV_KEY_MAX) {
            throw std::runtime_error(FormatString::formatString(
                "Key length in BKV Builder exceeds maximum in openCompound(): %ld/%u",
                key.length(), BKV::BKV_KEY_MAX
            ).get());
        }

        // Allocate space
        try {
            StringBuffer::checkResize(_buffer._bkv,
                static_cast<int64_t>(_buffer._head + 1 + BKV::BKV_KEY_SIZE + key.length() + size),
                _buffer._head, _buffer._capacity
            );
        } catch (std::runtime_error &e) { throw; }

        // Set tag
        _buffer._bkv[_buffer._head++] = tag;

        // Copy key/length
        _buffer._bkv[_buffer._head++] = key.length();
        std::memcpy(_buffer._bkv + _buffer._head, key.get(), key.length());
        _buffer._head += key.length();
    }

    BKV_Builder BKV_Builder::openCompound(const UTF8Str& key) {
        try {
            _setKeyTag(key, BKV::BKV_COMPOUND, BKV::BKV_COMPOUND_SIZE);
        } catch (std::runtime_error& e) { throw; }

        // Will return here to add size when the compound closes
        _depth.push(_buffer._head);
        _buffer._head += BKV::BKV_COMPOUND_SIZE;

        if (_depth.size() > BKV::BKV_COMPOUND_DEPTH_MAX) {
            throw std::runtime_error(FormatString::formatString(
                "Reached maximum compound depth in BKV Builder in openCompound(): %lu/%u",
                _depth.size(), BKV::BKV_COMPOUND_DEPTH_MAX
            ).get());
        }

        return *this;
    }

    BKV_Builder BKV_Builder::closeCompound() {
        // Set tag
        try {
            StringBuffer::checkResize(_buffer._bkv, _buffer._head + 1, _buffer._head, _buffer._capacity);
        } catch (std::runtime_error &e) { throw; }
        _buffer._bkv[_buffer._head++] = BKV::BKV_END;

        int64_t size = _buffer._head - _depth.top() - BKV::BKV_COMPOUND_SIZE;
        if ((size <= 0) || (size > BKV::BKV_COMPOUND_MAX)) {
            throw std::runtime_error(FormatString::formatString(
                "BKV Builder compound bigger than maximum size at index %d: %ld/%u",
                _depth.top(), size, BKV::BKV_COMPOUND_MAX
            ).get());
        }

        // Set compound length
        const uint32_t len = Endianness::hton(static_cast<uint32_t>(size));
        std::memcpy(_buffer._bkv + _depth.top(), &len, BKV::BKV_COMPOUND_SIZE);
        _depth.pop();

        return *this;
    }

    template<typename T>
    BKV_Builder BKV_Builder::_setInt(const UTF8Str& key, const T value, const uint8_t tag) {
        try {
            _setKeyTag(key, tag, sizeof(T));
        } catch (std::runtime_error& e) { throw; }

        // Set value
        const T v = Endianness::hton(value);
        std::memcpy(_buffer._bkv + _buffer._head, &v, sizeof(T));
        _buffer._head += sizeof(T);

        return *this;
    }

    template<typename T>
    BKV_Builder BKV_Builder::_setIntArray(const UTF8Str& key, const T* value,
        const uint16_t arraySize, const uint8_t tag
    ) {
        // Input validation
        if (!value) {
            Logger::crash("Array value pointer was null in _setIntArray().");
        }

        try {
            _setKeyTag(key, tag, BKV::BKV_ARRAY_SIZE + (arraySize * sizeof(T)));
        } catch (std::runtime_error& e) { throw; }
        
        // Set array size
        const uint16_t arraySizeNet = Endianness::hton(arraySize);
        std::memcpy(_buffer._bkv + _buffer._head, &arraySizeNet, BKV::BKV_ARRAY_SIZE);
        _buffer._head += BKV::BKV_ARRAY_SIZE;

        T v;
        for (uint16_t i = 0; i < arraySize; i++) {
            v = Endianness::hton(value[i]);
            std::memcpy(_buffer._bkv + _buffer._head, &v, sizeof(T));
            _buffer._head += sizeof(T);
        }

        return *this;
    }
    
    template<typename T>
    BKV_Builder BKV_Builder::_setFloat(const UTF8Str& key, const T value, const uint8_t tag) {
        try {
            _setKeyTag(key, tag, sizeof(T));
        } catch (std::runtime_error& e) { throw; }

        // Set value
        const T v = Endianness::htonf(value);
        std::memcpy(_buffer._bkv + _buffer._head, &v, sizeof(T));
        _buffer._head += sizeof(T);

        return *this;
    }

    template<typename T>
    BKV_Builder BKV_Builder::_setFloatArray(const UTF8Str& key, const T* value,
        const uint16_t arraySize, const uint8_t tag
    ) {
        // Input validation
        if (!value) {
            Logger::crash("Array value pointer was null in _setFloatArray().");
        }

        try {
            _setKeyTag(key, tag, BKV::BKV_ARRAY_SIZE + (arraySize * sizeof(T)));
        } catch (std::runtime_error& e) { throw; }
        
        // Set array size
        const uint16_t arraySizeNet = Endianness::hton(arraySize);
        std::memcpy(_buffer._bkv + _buffer._head, &arraySizeNet, BKV::BKV_ARRAY_SIZE);
        _buffer._head += BKV::BKV_ARRAY_SIZE;

        T v;
        for (uint16_t i = 0; i < arraySize; i++) {
            v = Endianness::htonf(value[i]);
            std::memcpy(_buffer._bkv + _buffer._head, &v, sizeof(T));
            _buffer._head += sizeof(T);
        }

        return *this;
    }
    
    BKV_Builder BKV_Builder::setBool(const UTF8Str& key, const bool value) {
        try {
            _setKeyTag(key, BKV::BKV_BOOL, sizeof(bool));
        } catch (std::runtime_error& e) { throw; }

        // Set value
        _buffer._bkv[_buffer._head++] = value;

        return *this;
    }

    BKV_Builder BKV_Builder::setString(const UTF8Str& key, const UTF8Str& value) {
        try {
            _setKeyTag(key, BKV::BKV_STR, BKV::BKV_STR_SIZE + value.length());
        } catch (std::runtime_error& e) { throw; }

        // Set string length
        const uint16_t len = Endianness::hton(value.length());
        std::memcpy(_buffer._bkv + _buffer._head, &len, BKV::BKV_STR_SIZE);
        _buffer._head += BKV::BKV_STR_SIZE;

        // Set value
        std::memcpy(_buffer._bkv + _buffer._head, value.get(), value.length());
        _buffer._head += value.length();

        return *this;
    }

    BKV_Builder BKV_Builder::setStringArray(const UTF8Str& key, const UTF8Str* value, const uint16_t arraySize) {
        // Input validation
        if (!value) {
            Logger::crash("Array value pointer was null in setStringArray().");
        }

        try {
            _setKeyTag(key, BKV::BKV_STR, BKV::BKV_ARRAY_SIZE);
        } catch (std::runtime_error& e) { throw; }
        
        // Set array size
        const uint16_t arraySizeNet = Endianness::hton(arraySize);
        std::memcpy(_buffer._bkv + _buffer._head, &arraySizeNet, BKV::BKV_ARRAY_SIZE);
        _buffer._head += BKV::BKV_ARRAY_SIZE;

        // Add strings
        uint16_t len;
        for (uint16_t i = 0; i < arraySize; i++) {
            // Allocate space    
            try {
                StringBuffer::checkResize(_buffer._bkv, _buffer._head + value[i].length(), _buffer._head, _buffer._capacity);
            } catch (std::runtime_error &e) { throw; }

            // Set string length
            len = Endianness::hton(value[i].length());
            std::memcpy(_buffer._bkv + _buffer._head, &len, BKV::BKV_STR_SIZE);
            _buffer._head += BKV::BKV_STR_SIZE;

            // Set value
            std::memcpy(_buffer._bkv + _buffer._head, value[i].get(), value[i].length());
            _buffer._head += value[i].length();
        }

        return *this;
    }
    
    // Specializations
    template<> BKV_Builder BKV_Builder::setValue<int8_t>(const UTF8Str& key, const int8_t value) {
        Logger::log(LOG_INFO, FormatString::formatString("Parsing int8: %d", value));
        return BKV_Builder::_setInt(key, value, BKV::BKV_I8);
    }
    template<> BKV_Builder BKV_Builder::setValue<uint8_t>(const UTF8Str& key, const uint8_t value) {
        Logger::log(LOG_INFO, FormatString::formatString("Parsing uint8: %u", value));
        return BKV_Builder::_setInt(key, value, BKV::BKV_UI8);
    }
    template<> BKV_Builder BKV_Builder::setValue<int16_t>(const UTF8Str& key, const int16_t value) {
        Logger::log(LOG_INFO, FormatString::formatString("Parsing int16: %d", value));
        return BKV_Builder::_setInt(key, value, BKV::BKV_I16);
    }
    template<> BKV_Builder BKV_Builder::setValue<uint16_t>(const UTF8Str& key, const uint16_t value) {
        Logger::log(LOG_INFO, FormatString::formatString("Parsing uint16: %u", value));
        return BKV_Builder::_setInt(key, value, BKV::BKV_UI16);
    }
    template<> BKV_Builder BKV_Builder::setValue<int32_t>(const UTF8Str& key, const int32_t value) {
        Logger::log(LOG_INFO, FormatString::formatString("Parsing int32: %d", value));
        return BKV_Builder::_setInt(key, value, BKV::BKV_I32);
    }
    template<> BKV_Builder BKV_Builder::setValue<uint32_t>(const UTF8Str& key, const uint32_t value) {
        Logger::log(LOG_INFO, FormatString::formatString("Parsing uint32: %u", value));
        return BKV_Builder::_setInt(key, value, BKV::BKV_UI32);
    }
    template<> BKV_Builder BKV_Builder::setValue<int64_t>(const UTF8Str& key, const int64_t value) {
        Logger::log(LOG_INFO, FormatString::formatString("Parsing int64: %ld", value));
        return BKV_Builder::_setInt(key, value, BKV::BKV_I64);
    }
    template<> BKV_Builder BKV_Builder::setValue<uint64_t>(const UTF8Str& key, const uint64_t value) {
        Logger::log(LOG_INFO, FormatString::formatString("Parsing uint64: %lu", value));
        return BKV_Builder::_setInt(key, value, BKV::BKV_UI64);
    }
    template<> BKV_Builder BKV_Builder::setValue<float32_t>(const UTF8Str& key, const float32_t value) {
        Logger::log(LOG_INFO, FormatString::formatString("Parsing float32: %f", value));
        return BKV_Builder::_setFloat(key, value, BKV::BKV_FLOAT);
    }
    template<> BKV_Builder BKV_Builder::setValue<float128_t>(const UTF8Str& key, const float128_t value) {
        Logger::log(LOG_INFO, FormatString::formatString("Parsing float128: %lf", value));
        return BKV_Builder::_setFloat(key, value, BKV::BKV_DOUBLE);
    }
    
    template<> BKV_Builder BKV_Builder::setValueArray<int8_t>(const UTF8Str& key, const int8_t* values, const uint16_t arraySize) {
        Logger::log(LOG_INFO, FormatString::formatString("Parsing int8 array of size: %u", arraySize));
        return BKV_Builder::_setIntArray(key, values, arraySize, BKV::BKV_I8);
    }
    template<> BKV_Builder BKV_Builder::setValueArray<uint8_t>(const UTF8Str& key, const uint8_t* values, const uint16_t arraySize) {
        Logger::log(LOG_INFO, FormatString::formatString("Parsing uint8 array of size: %u", arraySize));
        return BKV_Builder::_setIntArray(key, values, arraySize, BKV::BKV_UI8);
    }
    template<> BKV_Builder BKV_Builder::setValueArray<int16_t>(const UTF8Str& key, const int16_t* values, const uint16_t arraySize) {
        Logger::log(LOG_INFO, FormatString::formatString("Parsing int16 array of size: %u", arraySize));
        return BKV_Builder::_setIntArray(key, values, arraySize, BKV::BKV_I16);
    }
    template<> BKV_Builder BKV_Builder::setValueArray<uint16_t>(const UTF8Str& key, const uint16_t* values, const uint16_t arraySize) {
        Logger::log(LOG_INFO, FormatString::formatString("Parsing uint16 array of size: %u", arraySize));
        return BKV_Builder::_setIntArray(key, values, arraySize, BKV::BKV_UI16);
    }
    template<> BKV_Builder BKV_Builder::setValueArray<int32_t>(const UTF8Str& key, const int32_t* values, const uint16_t arraySize) {
        Logger::log(LOG_INFO, FormatString::formatString("Parsing int32 array of size: %u", arraySize));
        return BKV_Builder::_setIntArray(key, values, arraySize, BKV::BKV_I32);
    }
    template<> BKV_Builder BKV_Builder::setValueArray<uint32_t>(const UTF8Str& key, const uint32_t* values, const uint16_t arraySize) {
        Logger::log(LOG_INFO, FormatString::formatString("Parsing uint32 array of size: %u", arraySize));
        return BKV_Builder::_setIntArray(key, values, arraySize, BKV::BKV_UI32);
    }
    template<> BKV_Builder BKV_Builder::setValueArray<int64_t>(const UTF8Str& key, const int64_t* values, const uint16_t arraySize) {
        Logger::log(LOG_INFO, FormatString::formatString("Parsing int64 array of size: %u", arraySize));
        return BKV_Builder::_setIntArray(key, values, arraySize, BKV::BKV_I64);
    }
    template<> BKV_Builder BKV_Builder::setValueArray<uint64_t>(const UTF8Str& key, const uint64_t* values, const uint16_t arraySize) {
        return BKV_Builder::_setIntArray(key, values, arraySize, BKV::BKV_UI64);
        Logger::log(LOG_INFO, FormatString::formatString("Parsing uint64 array of size: %u", arraySize));
    }
    template<> BKV_Builder BKV_Builder::setValueArray<float32_t>(const UTF8Str& key, const float32_t* values, const uint16_t arraySize) {
        Logger::log(LOG_INFO, FormatString::formatString("Parsing float32 array of size: %u", arraySize));
        return BKV_Builder::_setFloatArray(key, values, arraySize, BKV::BKV_FLOAT);
    }
    template<> BKV_Builder BKV_Builder::setValueArray<float128_t>(const UTF8Str& key, const float128_t* values, const uint16_t arraySize) {
        Logger::log(LOG_INFO, FormatString::formatString("Parsing float128 array of size: %u", arraySize));
        return BKV_Builder::_setFloatArray(key, values, arraySize, BKV::BKV_DOUBLE);
    }
}
