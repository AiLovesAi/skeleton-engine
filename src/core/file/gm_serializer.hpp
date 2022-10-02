#pragma once

#include <cstdint>
#include <cstring>
#include <memory>
#include <type_traits>

namespace game {
    class Serializer {
        public:
            // Constructors
            Serializer() { resizeBuffer(64); }
            ~Serializer() { std::free(buffer_); }

            // Functions
            void reset() { head_ = 0; }

            void writeBits(const uint8_t data, const size_t count);
            void writeBits(const void* data, size_t count);
            void writeBits(const bool data) { writeBits(static_cast<uint8_t>(data), 1); }
            template<typename T>
            void writeBits(const T data) {
                static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value,
                    "Generic write only supports primitive data types.");
                writeBits(&data, sizeof(T) * 8);
            }

            void writeBytes(const void* data, const size_t count) { writeBits(data, count << 3); }
            template<typename T>
            void write(const T data) {
                static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value,
                    "Generic write only supports primitive data types.");
                writeBytes(&data, sizeof(T));
            }
        
        private:
            // Functions
            void resizeBuffer(const size_t size) {
                buffer_ = static_cast<char*>(std::realloc(buffer_, size));
                capacity_ = size;
            }

            // Variables
            char* buffer_ = nullptr;
            // Note: Measured in bits.
            size_t head_ = 0;
            size_t capacity_ = 0;
    };
}
