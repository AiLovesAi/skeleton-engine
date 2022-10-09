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
            // Types
            typedef struct UTF8Str_ {
                uint16_t len;
                std::shared_ptr<char> str;
            } UTF8Str;

            // Constructors
            Serializer() { resizeBuffer(BUFSIZ); }
            ~Serializer() { std::free(buffer_); }

            // Functions
            template<typename T>
            void set(const std::string& name, const T data);
            template<typename T>
            void setList(const std::string& name, const T* data, const uint32_t size);
            void setStr(const std::string& name, const UTF8Str& data);
            void setStrList(const std::string& name, const UTF8Str* data, const uint16_t size);

            template<typename T>
            T get(const std::string& name);
        
        private:
            // Functions
            void resizeBuffer(const size_t size);
            void write(const void* data, const size_t size);

            static std::shared_ptr<uint8_t> generateCompound(const UTF8Str& name);
            static inline uint8_t generateCompoundEnd() { return BKV::BKV_END; }
            template<typename T>
            static std::shared_ptr<uint8_t> generate(const UTF8Str& name, const T data);
            template<typename T>
            static std::shared_ptr<uint8_t> generateList(const UTF8Str& name, const T* data, const uint32_t size);
            static std::shared_ptr<uint8_t> generateStr(const UTF8Str& name, const UTF8Str& data);
            static std::shared_ptr<uint8_t> generateStrList(const UTF8Str& name, const UTF8Str* data, const uint32_t size);

            // Variables
            uint8_t* buffer_ = nullptr;
            size_t head_ = 0;
            size_t capacity_ = 0;
    };
}
