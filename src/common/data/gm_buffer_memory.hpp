#pragma once

#include <memory>
#include <cstdint>

namespace game {
    class BufferMemory {
        public:
            /// @brief Checks if @p size fits in @p ptr, given its @p capacity.
            /// If there is not room in @p ptr, it is reallocated with double the capacity.
            /// @param ptr Pointer to check
            /// @param size Size that may overflow capacity
            /// @param capacity The current capacity of the buffer
            template <typename T1, typename T2>
            static inline void checkResize(T1*& ptr, const T2 size, T2& capacity) {
                if (size > capacity) {
                    capacity <<= 1;
                    ptr = std::realloc(ptr, capacity);
                }
            }
    };
}
