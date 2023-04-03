#pragma once

#include "gm_graphics_device.hpp"

namespace game {
    class ModelBuffer {
        public:
            // Constructors
            ModelBuffer(
                GraphicsDevice& graphicsDevice,
                VkDeviceSize instanceSize,
                uint32_t instanceCount,
                VkBufferUsageFlags usageFlags,
                VkMemoryPropertyFlags memoryPropertyFlags,
                VkDeviceSize minOffsetAlignment = 1
            );
            ~ModelBuffer();
            
            ModelBuffer(const ModelBuffer&) = delete;
            ModelBuffer& operator=(const ModelBuffer&) = delete;
            ModelBuffer(ModelBuffer&&) = default;
            ModelBuffer& operator=(ModelBuffer&&) = default;

            // Functions
            VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
            void unmap();
            
            void writeToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
            VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
            VkDescriptorBufferInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
            VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
            
            void writeToIndex(void* data, int index);
            VkResult flushIndex(int index);
            VkDescriptorBufferInfo descriptorInfoAt(int index);
            VkResult invalidateIndex(int index);
            
            VkBuffer buffer() const { return _buffer; }
            void* mappedMemory() const { return _mapped; }
            uint32_t instanceCount() const { return _instanceCount; }
            VkDeviceSize instanceSize() const { return _instanceSize; }
            VkDeviceSize alignmentSize() const { return _instanceSize; }
            VkBufferUsageFlags usageFlags() const { return _usageFlags; }
            VkMemoryPropertyFlags memoryPropertyFlags() const { return _memoryPropertyFlags; }
            VkDeviceSize bufferSize() const { return _bufferSize; }
            
        private:
            // Functions
            static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);
            
            // Variables
            GraphicsDevice& _graphicsDevice;
            void* _mapped = nullptr;
            VkBuffer _buffer = VK_NULL_HANDLE;
            VkDeviceMemory _memory = VK_NULL_HANDLE;
            
            VkDeviceSize _bufferSize;
            VkDeviceSize _instanceSize;
            uint32_t _instanceCount;
            VkDeviceSize _alignmentSize;
            VkBufferUsageFlags _usageFlags;
            VkMemoryPropertyFlags _memoryPropertyFlags;
    };
}
