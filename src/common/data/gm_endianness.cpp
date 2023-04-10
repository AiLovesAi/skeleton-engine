#include "gm_endianness.hpp"

namespace game {
    float32_t Endianness::swapBytesFloat32(const float32_t data) {
        float32_t res;
        const char *src = (const char *) &data;
        char *dst = (char *) &res;
        for (uint8_t i = 0; i < sizeof(float32_t); i++) {
            dst[i] = src[sizeof(float32_t) - i - 1];
        }
        return res;
    }
    float64_t Endianness::swapBytesFloat64(const float64_t data) {
        float64_t res;
        const char *src = (const char *) &data;
        char *dst = (char *) &res;
        for (uint8_t i = 0; i < sizeof(float64_t); i++) {
            dst[i] = src[sizeof(float64_t) - i - 1];
        }
        return res;
    }
    float128_t Endianness::swapBytesFloat128(const float128_t data) {
        float128_t res;
        const char *src = (const char *) &data;
        char *dst = (char *) &res;
        for (uint8_t i = 0; i < sizeof(float128_t); i++) {
            dst[i] = src[sizeof(float128_t) - i - 1];
        }
        return res;
    }
}
