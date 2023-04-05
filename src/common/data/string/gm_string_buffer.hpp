#pragma once

#include "gm_string.hpp"

#include <cstring>
#include <memory>

namespace game {
    class StringBuffer {
        public:
            // Constructors
            StringBuffer() : _capacity{BUFSIZ} {
                _buffer = static_cast<char*>(std::malloc(BUFSIZ));
            }
            StringBuffer(const size_t capacity) : _capacity{capacity} {
                _buffer = static_cast<char*>(std::malloc(BUFSIZ));
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
            inline void clear() { _len = 0; }
            UTF8Str str();
            
            inline void set(const char*__restrict__ str) { set(str, std::strlen(str)); }
            inline void set(const char*__restrict__ str, const size_t len) {
                std::memcpy(_buffer, str, len);
                _len = len;
            }

            inline size_t append(const char*__restrict__ str) { return append(str, std::strlen(str)); }
            size_t append(const char*__restrict__ str, const size_t len);
            size_t append(const UTF8Str& str);
            size_t append(const char c);

            /// @brief Checks if @p size fits in @p ptr, given its @p capacity.
            /// If there is not room in @p ptr, it is reallocated with double the capacity.
            /// @param ptr Pointer to check
            /// @param size Size that may overflow capacity
            /// @param prevSize Last size before current check for overflow checking
            /// @param capacity The current capacity of the buffer
            template <typename T1, typename T2>
            static void checkResize(T1*& ptr, const T2 size, const T2 prevSize, T2& capacity) {
                if ((size * 2) < prevSize) {
                    throw std::runtime_error(
                        FormatString::formatString(
                            "New buffer size overflows to be less than previous size: (%ld * 2) < %d.", size, prevSize
                        ).get()
                    );
                }

                if (size > capacity) {
                    capacity = size * 2;
                    ptr = static_cast<T1*>(std::realloc(ptr, capacity));
                }
            }

        protected:
            friend class FormatString;

            // Functions
            template <typename T1, typename T2>
            static void _checkResize(T1*& ptr, const T2 size, const T2 prevSize, T2& capacity) {
                if ((size * 2) < prevSize) {
                    throw std::runtime_error("New buffer size overflows to be less than previous size in string format function.");
                }
                
                if (size > capacity) {
                    capacity = size * 2;
                    ptr = static_cast<T1*>(std::realloc(ptr, capacity));
                }
            }
        
        private:
            // Variables
            size_t _len = 0;
            size_t _capacity = 0;
            char* _buffer = nullptr;
    };
}
