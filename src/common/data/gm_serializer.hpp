#pragma once

#include "gm_endianness.hpp"

#include <cstdint>
#include <memory>
#include <type_traits>

namespace game {
    class Serializer {
        public:
            // Constructors
            Serializer() { resizeBuffer(BUFSIZ); }
            ~Serializer() { std::free(buffer_); }

            // Functions
            void write(const void* data, const size_t size);

            template<typename T>
            void write(const T data) {
                static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value,
                    "Generic write only supports primitive data types.");
                const T networkData = Endianness::hton(data);
                write(&networkData, sizeof(T));
            }
        
        private:
            // Functions
            void resizeBuffer(const size_t size);

            // Variables
            uint8_t* buffer_ = nullptr;
            size_t head_ = 0;
            size_t capacity_ = 0;
    };
}
