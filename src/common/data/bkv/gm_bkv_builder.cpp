#include "gm_bkv_builder.hpp"

#include "../gm_endianness.hpp"

namespace game {
    [[nodiscard]] BKV BKV_Builder::build() {
        // Input validation
        if (_depth.size()) {
            UTF8Str msg = FormatString::formatString("There are %lu unclosed compounds in BKV Builder in build().",
                _depth.size()
            );
            throw std::runtime_error(msg.get());
        }
        
        // Add full compound size
        // NOTE: Would be two, but the closing compound has not been added
        int64_t size = _buffer._head - 1 - BKV::BKV_COMPOUND_SIZE;
        if ((size <= 0) || (size > BKV::BKV_COMPOUND_MAX)) {
            UTF8Str msg = FormatString::formatString("BKV Builder main compound bigger than maximum size: %ld/%u",
                size, BKV::BKV_COMPOUND_MAX
            );
            throw std::runtime_error(msg.get());
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

    void BKV_Builder::openCompound(const UTF8Str& key) {
        // Allocate space
        try {
            StringBuffer::checkResize(_buffer._bkv, _buffer._head + key.length() + BKV::BKV_COMPOUND_SIZE + BKV::BKV_KEY_SIZE + 1,
                _buffer._head, _buffer._capacity
            );
        } catch (std::runtime_error &e) { throw; }

        // Set tag
        _buffer._bkv[_buffer._head++] = BKV::BKV_COMPOUND;

        // Copy key/length
        _buffer._bkv[_buffer._head++] = key.length();
        std::memcpy(_buffer._bkv + _buffer._head, key.get(), key.length());
        _buffer._head += key.length();

        // Will return here to add size when the compound closes
        _depth.push(_buffer._head);
        _buffer._head += BKV::BKV_COMPOUND_SIZE;

        if (_depth.size() > BKV::BKV_COMPOUND_DEPTH_MAX) {
            UTF8Str msg = FormatString::formatString("Reached maximum compound depth in BKV Builder in openCompound(): %lu/%u",
                _depth.size(), BKV::BKV_COMPOUND_DEPTH_MAX
            );
            throw std::runtime_error(msg.get());
        }
    }

    void BKV_Builder::closeCompound() {
        // Set tag
        try {
            StringBuffer::checkResize(_buffer._bkv, _buffer._head + 1, _buffer._head, _buffer._capacity);
        } catch (std::runtime_error &e) { throw; }
        _buffer._bkv[_buffer._head++] = BKV::BKV_END;

        int64_t size = _buffer._head - _depth.top() - BKV::BKV_COMPOUND_SIZE;
        if ((size <= 0) || (size > BKV::BKV_COMPOUND_MAX)) {
            UTF8Str msg = FormatString::formatString("BKV Builder compound bigger than maximum size at index %d: %ld/%u",
                _depth.top(), size, BKV::BKV_COMPOUND_MAX
            );
            throw std::runtime_error(msg.get());
        }

        // Set compound length
        const uint32_t len = Endianness::hton(static_cast<uint32_t>(size));
        std::memcpy(_buffer._bkv + _depth.top(), &len, BKV::BKV_COMPOUND_SIZE);
        _depth.pop();
    }

    template<typename T>
    void BKV_Builder::setInt(const UTF8Str& key, const T  value) {
        // TODO
    }

    template<typename T>
    void BKV_Builder::setIntArray(const UTF8Str& key, const T* value, const uint32_t size) {
        
    }

    template<typename T>
    void BKV_Builder::setFloat(const UTF8Str& key, const T value) {

    }

    template<typename T>
    void BKV_Builder::setFloatArray(const UTF8Str& key, const T* value) {

    }

    void BKV_Builder::setBool(const UTF8Str& key, const bool value) {

    }

    void BKV_Builder::setBoolArray(const UTF8Str& key, const bool* value) {

    }

    void BKV_Builder::setStr(const UTF8Str& key, const UTF8Str& value) {
        
    }

    void BKV_Builder::setStrArray(const UTF8Str& key, const UTF8Str* value, const uint16_t size) {
        
    }
}
