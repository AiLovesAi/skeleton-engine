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
            
            VkBuffer buffer() const { return buffer_; }
            void* mappedMemory() const { return mapped_; }
            uint32_t instanceCount() const { return instanceCount_; }
            VkDeviceSize instanceSize() const { return instanceSize_; }
            VkDeviceSize alignmentSize() const { return instanceSize_; }
            VkBufferUsageFlags usageFlags() const { return usageFlags_; }
            VkMemoryPropertyFlags memoryPropertyFlags() const { return memoryPropertyFlags_; }
            VkDeviceSize bufferSize() const { return bufferSize_; }
            
        private:
            // Functions
            static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);
            
            // Variables
            GraphicsDevice& graphicsDevice_;
            void* mapped_ = nullptr;
            VkBuffer buffer_ = VK_NULL_HANDLE;
            VkDeviceMemory memory_ = VK_NULL_HANDLE;
            
            VkDeviceSize bufferSize_;
            VkDeviceSize instanceSize_;
            uint32_t instanceCount_;
            VkDeviceSize alignmentSize_;
            VkBufferUsageFlags usageFlags_;
            VkMemoryPropertyFlags memoryPropertyFlags_;
    };
}
