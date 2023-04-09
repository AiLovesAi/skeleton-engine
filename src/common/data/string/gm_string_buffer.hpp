#pragma once

#include "gm_string.hpp"

#include <cstring>
#include <memory>

namespace game {
    class StringBuffer {
        #define STRBUFSIZ 64
        public:
            // Constructors
            StringBuffer() : _capacity{STRBUFSIZ} {
                _buffer = static_cast<char*>(std::malloc(STRBUFSIZ));
            }
            StringBuffer(const size_t capacity) : _capacity{capacity} {
                _buffer = static_cast<char*>(std::malloc(STRBUFSIZ));
            }
            StringBuffer(const char*__restrict__ str, const size_t len) : _len{len}, _capacity{len + 1} {
                _buffer = static_cast<char*>(std::malloc(_capacity));
            }
            StringBuffer(const char*__restrict__ str) {
                _len = std::strlen(str);
                _capacity = _len + 1;
                _buffer = static_cast<char*>(std::malloc(_capacity));
                std::memcpy(_buffer, str, _len);
            }
            ~StringBuffer() {
                std::free(_buffer);
            }

            // Functions
            char* get();
            inline size_t len () { return _len; }
            inline void clear() { _len = 0; }
            UTF8Str str();

            bool cmp(const char c, const size_t index) { return _buffer[index] == c; }
            
            inline void set(const char*__restrict__ str) { set(str, std::strlen(str)); }
            inline void set(const char*__restrict__ str, const size_t len) {
                std::memcpy(_buffer, str, len);
                _len = len;
            }

            inline size_t append(const char*__restrict__ str) { return append(str, std::strlen(str)); }
            size_t append(const char*__restrict__ str, const size_t len);
            size_t append(const UTF8Str& str);
            size_t append(const char c);

            size_t setIndex(const char c, const size_t index);

            /// @brief Checks if @p size fits in @p ptr, given its @p capacity.
            /// If there is not room in @p ptr, it is reallocated with double the capacity.
            /// @param ptr Pointer to check
            /// @param size Size that may overflow capacity
            /// @param prevSize Last size before current check for overflow checking
            /// @param capacity The current capacity of the buffer
            template <typename T1, typename T2>
            static inline void checkResize(T1*& ptr, const T2 size, const T2 prevSize, T2& capacity) {
                void* p = static_cast<void*>(ptr);
                size_t c = static_cast<size_t>(capacity);
                _checkResize(p, static_cast<size_t>(size), static_cast<size_t>(prevSize), c);
                ptr = static_cast<T1*>(p);
                capacity = static_cast<T2>(c);
            }

        protected:
            friend class FormatString;

            // Functions
            template <typename T1, typename T2>
            static inline void checkResizeNoFormat(T1*& ptr, const T2 size, const T2 prevSize, T2& capacity) {
                void* p = static_cast<void*>(ptr);
                size_t c = static_cast<size_t>(capacity);
                _checkResizeNoFormat(p, static_cast<size_t>(size), static_cast<size_t>(prevSize), c);
                ptr = static_cast<T1*>(p);
                capacity = static_cast<T2>(c);
            }
        
        private:
            // Functions
            static void _checkResize(void*& ptr, const size_t size, const size_t prevSize, size_t& capacity);
            static void _checkResizeNoFormat(void*& ptr, const size_t size, const size_t prevSize, size_t& capacity);
            
            // Variables
            size_t _len = 0;
            size_t _capacity = 0;
            char* _buffer = nullptr;
    };
}
