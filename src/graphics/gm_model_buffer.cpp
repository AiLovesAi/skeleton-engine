#include "gm_model_buffer.hpp"

#include "../util/gm_logger.hpp"

namespace game {
    ModelBuffer::ModelBuffer(
        GraphicsDevice* device,
        VkDeviceSize instanceSize,
        uint32_t instanceCount,
        VkBufferUsageFlags usageFlags,
        VkMemoryPropertyFlags memoryPropertyFlags,
        VkDeviceSize minOffsetAlignment
    ) : device{device},
        instanceSize{instanceSize},
        instanceCount{instanceCount},
        usageFlags{usageFlags},
        memoryPropertyFlags{memoryPropertyFlags}
    {
        if (device == nullptr) Logger::crash("Graphics device passed to ModelBuffer is null.");

        alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
        bufferSize = alignmentSize * instanceCount;
        device->createBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer, memory);
    }

    ModelBuffer::~ModelBuffer() {
        unmap();
        vkDestroyBuffer(device->getDevice(), buffer, nullptr);
        vkFreeMemory(device->getDevice(), memory, nullptr);
    }

    VkDeviceSize ModelBuffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
        if (minOffsetAlignment > 0) {
            return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
        }
        return instanceSize;
    }

    VkResult ModelBuffer::map(VkDeviceSize size, VkDeviceSize offset) {
        if(!(buffer && memory)) Logger::crash("Called map on buffer before create.");
        return vkMapMemory(device->getDevice(), memory, offset, size, 0, &mapped);
    }

    void ModelBuffer::unmap() {
        if (mapped) {
            vkUnmapMemory(device->getDevice(), memory);
            mapped = nullptr;
        }
    }

    void ModelBuffer::writeToBuffer(void *data, VkDeviceSize size, VkDeviceSize offset) {
        if (!mapped) Logger::crash("Cannot copy to unmapped buffer.");
        
        if (size == VK_WHOLE_SIZE) {
            memcpy(mapped, data, bufferSize);
        } else {
            char *memOffset = (char *)mapped;
            memOffset += offset;
            memcpy(memOffset, data, size);
        }
    }

    VkResult ModelBuffer::flush(VkDeviceSize size, VkDeviceSize offset) {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkFlushMappedMemoryRanges(device->getDevice(), 1, &mappedRange);
    }

    VkResult ModelBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkInvalidateMappedMemoryRanges(device->getDevice(), 1, &mappedRange);
    }

    VkDescriptorBufferInfo ModelBuffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
        return VkDescriptorBufferInfo{ buffer, offset, size};
    }

    void ModelBuffer::writeToIndex(void *data, int index) { writeToBuffer(data, instanceSize, index * alignmentSize); }

    VkResult ModelBuffer::flushIndex(int index) { return flush(alignmentSize, index * alignmentSize); }

    VkDescriptorBufferInfo ModelBuffer::descriptorInfoForIndex(int index) {
        return descriptorInfo(alignmentSize, index * alignmentSize);
    }

    VkResult ModelBuffer::invalidateIndex(int index) {
        return invalidate(alignmentSize, index * alignmentSize);
    }
}
