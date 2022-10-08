#pragma once

#include "gm_bkv.hpp"
#include "gm_endianness.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>

namespace game {
    class Serializer {
        public:
            // Constructors
            Serializer() { resizeBuffer(BUFSIZ); }
            ~Serializer() { std::free(buffer_); }

            // Functions
            void set(const std::string& name, const bool data) { set(name, static_cast<uint8_t>(data)); }
            void set(const std::string& name, const uint8_t data);
            void set(const std::string& name, const int8_t data);
            void set(const std::string& name, const uint16_t data);
            void set(const std::string& name, const int16_t data);
            void set(const std::string& name, const uint32_t data);
            void set(const std::string& name, const int32_t data);
            void set(const std::string& name, const uint64_t data);
            void set(const std::string& name, const int64_t data);
            void set(const std::string& name, const wchar_t* data, const uint16_t len);
        
        private:
            // Functions
            void resizeBuffer(const size_t size);
            void write(const void* data, const size_t size);
            // template<typename T>
            // void write(const T data) {
            //     static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value,
            //         "Generic write only supports primitive data types.");
            //     const T networkData = Endianness::hton(data);
            //     write(&networkData, sizeof(T));
            // }

            static std::shared_ptr<uint8_t> generate(const wchar_t* name, const uint8_t nameLen, uint8_t data);
            static std::shared_ptr<uint8_t> generate(const wchar_t* name, const uint8_t nameLen, int8_t data);
            static std::shared_ptr<uint8_t> generate(const wchar_t* name, const uint8_t nameLen, uint16_t data);
            static std::shared_ptr<uint8_t> generate(const wchar_t* name, const uint8_t nameLen, int16_t data);
            static std::shared_ptr<uint8_t> generate(const wchar_t* name, const uint8_t nameLen, uint32_t data);
            static std::shared_ptr<uint8_t> generate(const wchar_t* name, const uint8_t nameLen, int32_t data);
            static std::shared_ptr<uint8_t> generate(const wchar_t* name, const uint8_t nameLen, uint64_t data);
            static std::shared_ptr<uint8_t> generate(const wchar_t* name, const uint8_t nameLen, int64_t data);
            static std::shared_ptr<uint8_t> generate(const wchar_t* name, const uint8_t nameLen, const wchar_t* data, uint16_t len);

            // Variables
            uint8_t* buffer_ = nullptr;
            size_t head_ = 0;
            size_t capacity_ = 0;
    };
}
