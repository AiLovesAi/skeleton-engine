#pragma once

#include <memory>
#include <cstdint>
#include <sstream>
#include <stdexcept>

namespace game {
    class BufferMemory {
        public:
            /// @brief Checks if @p size fits in @p ptr, given its @p capacity.
            /// If there is not room in @p ptr, it is reallocated with double the capacity.
            /// @param ptr Pointer to check
            /// @param size Size that may overflow capacity
            /// @param prevSize Last size before current check for overflow checking
            /// @param capacity The current capacity of the buffer
            template <typename T1, typename T2>
            static inline void checkResize(T1*& ptr, const T2 size, const T2 prevSize, T2& capacity) {
                if (size < prevSize) {
                    std::stringstream msg;
                    msg << "New buffer size overflows to a number less than the previous: " << size << " < " << prevSize << ".";
                    throw std::overflow_error(msg.str());
                }
                if (size > capacity) {
                    capacity <<= 1; // Capacity doubles
                    ptr = static_cast<T1*>(std::realloc(ptr, capacity));
                }
            }
    };
}
