#include "gm_string_buffer.hpp"

#include "../file/gm_logger.hpp"

#include <stdexcept>

namespace game {
    UTF8Str StringBuffer::str() {
        char* copy = static_cast<char*>(std::malloc(_len + 1));
        std::memcpy(copy, _buffer, _len);
        copy[_len] = '\0';
        return UTF8Str{static_cast<int64_t>(_len), std::shared_ptr<const char>(copy, std::free)};
    }

    size_t StringBuffer::append(const char* str, const size_t len) {
        try {
            checkResize(_buffer, _len + len, _len, _capacity);
        } catch (std::runtime_error& e) { Logger::crash(e.what()); }
        std::memcpy(_buffer + _len, str, len);
        _len += len;
        return _len;
    }
    size_t StringBuffer::append(const UTF8Str& str) {
        const size_t len = static_cast<size_t>(str.length());
        try {
            checkResize(_buffer, _len + len, _len, _capacity);
        } catch (std::runtime_error& e) { Logger::crash(e.what()); }
        std::memcpy(_buffer + _len, str.get(), len);
        _len += len;
        return _len;
    }
    size_t StringBuffer::append(const char c) {
        try {
            checkResize(_buffer, _len + 1, _len, _capacity);
        } catch (std::runtime_error& e) { Logger::crash(e.what()); }
        _buffer[_len++] = c;
        return _len;
    }
}
