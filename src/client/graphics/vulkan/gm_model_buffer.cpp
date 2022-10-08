#include "gm_model_buffer.hpp"

#include <common/data/gm_logger.hpp>

namespace game {
    ModelBuffer::ModelBuffer(
        GraphicsDevice& graphicsDevice,
        VkDeviceSize instanceSize,
        uint32_t instanceCount,
        VkBufferUsageFlags usageFlags,
        VkMemoryPropertyFlags memoryPropertyFlags,
        VkDeviceSize minOffsetAlignment
    ) : graphicsDevice_{graphicsDevice},
        instanceSize_{instanceSize},
        instanceCount_{instanceCount},
        usageFlags_{usageFlags},
        memoryPropertyFlags_{memoryPropertyFlags}
    {
        alignmentSize_ = getAlignment(instanceSize, minOffsetAlignment);
        bufferSize_ = alignmentSize_ * instanceCount;
        graphicsDevice_.createBuffer(bufferSize_, usageFlags, memoryPropertyFlags, buffer_, memory_);
    }

    ModelBuffer::~ModelBuffer() {
        unmap();
        vkDestroyBuffer(graphicsDevice_.device(), buffer_, nullptr);
        vkFreeMemory(graphicsDevice_.device(), memory_, nullptr);
    }

    VkDeviceSize ModelBuffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
        if (minOffsetAlignment > 0) {
            return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
        }
        return instanceSize;
    }

    VkResult ModelBuffer::map(VkDeviceSize size, VkDeviceSize offset) {
        if(!(buffer_ && memory_)) Logger::crash("Called map on buffer before create.");
        return vkMapMemory(graphicsDevice_.device(), memory_, offset, size, 0, &mapped_);
    }

    void ModelBuffer::unmap() {
        if (mapped_) {
            vkUnmapMemory(graphicsDevice_.device(), memory_);
            mapped_ = nullptr;
        }
    }

    void ModelBuffer::writeToBuffer(void *data, VkDeviceSize size, VkDeviceSize offset) {
        if (!mapped_) Logger::crash("Cannot copy to unmapped buffer.");
        
        if (size == VK_WHOLE_SIZE) {
            memcpy(mapped_, data, bufferSize_);
        } else {
            char *memOffset = (char*) mapped_;
            memOffset += offset;
            memcpy(memOffset, data, size);
        }
    }

    VkResult ModelBuffer::flush(VkDeviceSize size, VkDeviceSize offset) {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = memory_;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkFlushMappedMemoryRanges(graphicsDevice_.device(), 1, &mappedRange);
    }

    VkResult ModelBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = memory_;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkInvalidateMappedMemoryRanges(graphicsDevice_.device(), 1, &mappedRange);
    }

    VkDescriptorBufferInfo ModelBuffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
        return VkDescriptorBufferInfo{buffer_, offset, size};
    }

    void ModelBuffer::writeToIndex(void *data, int index) { writeToBuffer(data, instanceSize_, index * alignmentSize_); }

    VkResult ModelBuffer::flushIndex(int index) { return flush(alignmentSize_, index * alignmentSize_); }

    VkDescriptorBufferInfo ModelBuffer::descriptorInfoAt(int index) {
        return descriptorInfo(alignmentSize_, index * alignmentSize_);
    }

    VkResult ModelBuffer::invalidateIndex(int index) {
        return invalidate(alignmentSize_, index * alignmentSize_);
    }
}
