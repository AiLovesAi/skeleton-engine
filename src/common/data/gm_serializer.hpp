#pragma once

#include <cstdint>
#include <memory>
#include <type_traits>

namespace game {
    class Serializer {
        public:
            // Constructors
            Serializer() { resizeBuffer(8); }
            ~Serializer() { std::free(buffer_); }

            // Functions
            void write(const void* data, const size_t size);

            template<typename T>
            void write(const T data) {
                static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value,
                    "Generic write only supports primitive data types.");
                write(&data, sizeof(data));
            }
        
        private:
            // Functions
            void resizeBuffer(const size_t size);

            // Variables
            char* buffer_ = nullptr;
            size_t head_ = 0;
            size_t capacity_ = 0;
    };
}
