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
    char* StringBuffer::get() {
        try {
            checkResize(_buffer, _len + 1, _len, _capacity);
        } catch (std::runtime_error& e) { Logger::crash(e.what()); }
        _buffer[_len] = '\0';
        return _buffer;
    }

    size_t StringBuffer::append(const char*__restrict__ str, const size_t len) {
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

    size_t StringBuffer::setIndex(const char c, const size_t index) {
        if (index < _len) {
            _buffer[index] = c;
        } else if (index == _len) {
            try {
                checkResize(_buffer, _len + 1, _len, _capacity);
            } catch (std::runtime_error& e) { Logger::crash(e.what()); }
            _buffer[_len++] = c;
        } else {
            Logger::crash(FormatString::formatString(
                "Index is greater than one more than length in setIndex(): %ld > (%ld + 1)", index, _len
            ));
        }
        return _len;
    }

    void StringBuffer::_checkResize(void*& ptr, const size_t size, const size_t prevSize, size_t& capacity) {
        if ((size * 2) < prevSize) {
            throw std::runtime_error(
                FormatString::formatString(
                    "New buffer size overflows to be less than previous size: (%lu * 2) < %lu.", size, prevSize
                ).get()
            );
        }

        if (size > capacity) {
            capacity = size * 2;
            ptr = ::realloc(ptr, capacity);
        }
    }

    void StringBuffer::_checkResizeNoFormat(void*& ptr, const size_t size, const size_t prevSize, size_t& capacity) {
        if ((size * 2) < prevSize) {
            throw std::runtime_error("New buffer size overflows to be less than previous size in string format function.");
        }
        
        if (size > capacity) {
            capacity = size * 2;
            ptr = std::realloc(ptr, capacity);
        }
    }

}
