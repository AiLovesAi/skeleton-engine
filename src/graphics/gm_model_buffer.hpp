#pragma once

#include "gm_graphics_device.hpp"

namespace game {
    class ModelBuffer {
        public:
            // Constructors
            ModelBuffer(
                GraphicsDevice* device,
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
            VkDescriptorBufferInfo descriptorInfoForIndex(int index);
            VkResult invalidateIndex(int index);
            
            VkBuffer getBuffer() const { return buffer; }
            void* getMappedMemory() const { return mapped; }
            uint32_t getInstanceCount() const { return instanceCount; }
            VkDeviceSize getInstanceSize() const { return instanceSize; }
            VkDeviceSize getAlignmentSize() const { return instanceSize; }
            VkBufferUsageFlags getUsageFlags() const { return usageFlags; }
            VkMemoryPropertyFlags getMemoryPropertyFlags() const { return memoryPropertyFlags; }
            VkDeviceSize getBufferSize() const { return bufferSize; }
            
        private:
            // Functions
            static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);
            
            // Variables
            GraphicsDevice* device;
            void* mapped = nullptr;
            VkBuffer buffer = VK_NULL_HANDLE;
            VkDeviceMemory memory = VK_NULL_HANDLE;
            
            VkDeviceSize bufferSize;
            VkDeviceSize instanceSize;
            uint32_t instanceCount;
            VkDeviceSize alignmentSize;
            VkBufferUsageFlags usageFlags;
            VkMemoryPropertyFlags memoryPropertyFlags;
    };
}
