#pragma once

#include "gm_bkv.hpp"

#include <cstdint>
#include <memory>
#include <string>

namespace game {
    class Serializer {
        public:
            // Constructors
            Serializer() { resizeBuffer(BUFSIZ); }
            ~Serializer() { std::free(buffer_); }

            // Functions
            template<typename T>
            void set(const std::string& name, const T data);
            template<typename T>
            void setList(const std::string& name, const T* data, const uint32_t size);
            void setStr(const std::string& name, const BKV::UTF8Str& data);
            void setStrList(const std::string& name, const BKV::UTF8Str* data, const uint16_t size);

            template<typename T>
            T get(const std::string& name);
        
        private:
            // Functions
            void resizeBuffer(const size_t size);
            void write(const void* data, const size_t size);

            // Variables
            uint8_t* buffer_ = nullptr;
            size_t head_ = 0;
            size_t capacity_ = 0;
    };
}
