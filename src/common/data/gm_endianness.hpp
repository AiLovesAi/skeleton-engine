#pragma once

#include "gm_type_aliaser.hpp"

#include <cstdint>
#include <bit>

namespace game {
    class Endianness {
        public:
            // Functions
            template <typename T>
            static inline T swapBytes(const T data) {
                return ByteSwapper<T, sizeof(T)>().swap(data);
            }

            template <typename T>
            static inline T hton(const T data) {
                if constexpr (std::endian::native == std::endian::big) {
                    return data;
                } else {
                    return swapBytes(data);
                }
            }
            template <typename T>
            static inline T ntoh(const T data) {
                hton(data);
            }
            
        private:
            // Functions
            static inline uint16_t swapBytes2(const uint16_t data) {
                return (data >> 8) | (data << 8);
            }
            static inline uint32_t swapBytes4(const uint32_t data) {
                return ((data >> 24) & 0x000000ff) |
                    ((data >> 8) & 0x0000ff00) |
                    ((data << 8) & 0x00ff0000) |
                    ((data << 24) & 0xff000000);
            }
            static inline uint64_t swapBytes8(const uint64_t data) {
                return ((data >> 56) & 0x00000000000000ff) |
                    ((data >> 40) & 0x000000000000ff00) |
                    ((data >> 24) & 0x0000000000ff0000) |
                    ((data >> 8) & 0x00000000ff000000) |
                    ((data << 8) & 0x000000ff00000000) |
                    ((data << 24) & 0x0000ff0000000000) |
                    ((data << 40) & 0x00ff000000000000) |
                    ((data << 56) & 0xff00000000000000);
            }

            // Classes
            template <typename T, size_t tSize> class ByteSwapper;
            template <typename T> class ByteSwapper<T,2> {
                public:
                    T swap(const T data) const {
                        uint16_t result = swapBytes2(TypeAliaser<T, uint16_t>(data).get());
                        return TypeAliaser<uint16_t, T>(result).get();
                    }
            };
            
            template <typename T> class ByteSwapper<T,4> {
                public:
                    T swap(const T data) const {
                        uint16_t result = swapBytes4(TypeAliaser<T, uint16_t>(data).get());
                        return TypeAliaser<uint16_t, T>(result).get();
                    }
            };
            
            template <typename T> class ByteSwapper<T,8> {
                public:
                    T swap(const T data) const {
                        uint16_t result = swapBytes8(TypeAliaser<T, uint16_t>(data).get());
                        return TypeAliaser<uint16_t, T>(result).get();
                    }
            };
    };
}