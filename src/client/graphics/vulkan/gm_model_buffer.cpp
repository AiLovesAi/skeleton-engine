#include "gm_model_buffer.hpp"

#include <common/data/file/gm_logger.hpp>

namespace game {
    ModelBuffer::ModelBuffer(
        GraphicsDevice& graphicsDevice,
        VkDeviceSize instanceSize,
        uint32_t instanceCount,
        VkBufferUsageFlags usageFlags,
        VkMemoryPropertyFlags memoryPropertyFlags,
        VkDeviceSize minOffsetAlignment
    ) : _graphicsDevice{graphicsDevice},
        _instanceSize{instanceSize},
        _instanceCount{instanceCount},
        _usageFlags{usageFlags},
        _memoryPropertyFlags{memoryPropertyFlags}
    {
        _alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
        _bufferSize = _alignmentSize * instanceCount;
        _graphicsDevice.createBuffer(_bufferSize, usageFlags, memoryPropertyFlags, _buffer, _memory);
    }

    ModelBuffer::~ModelBuffer() {
        unmap();
        vkDestroyBuffer(_graphicsDevice.device(), _buffer, nullptr);
        vkFreeMemory(_graphicsDevice.device(), _memory, nullptr);
    }

    VkDeviceSize ModelBuffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
        if (minOffsetAlignment > 0) {
            return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
        }
        return instanceSize;
    }

    VkResult ModelBuffer::map(VkDeviceSize size, VkDeviceSize offset) {
        if(!(_buffer && _memory)) Logger::crash("Called map on buffer before create.");
        return vkMapMemory(_graphicsDevice.device(), _memory, offset, size, 0, &_mapped);
    }

    void ModelBuffer::unmap() {
        if (_mapped) {
            vkUnmapMemory(_graphicsDevice.device(), _memory);
            _mapped = nullptr;
        }
    }

    void ModelBuffer::writeToBuffer(void *data, VkDeviceSize size, VkDeviceSize offset) {
        if (!_mapped) Logger::crash("Cannot copy to unmapped buffer.");
        
        if (size == VK_WHOLE_SIZE) {
            memcpy(_mapped, data, _bufferSize);
        } else {
            char *memOffset = (char*) _mapped;
            memOffset += offset;
            memcpy(memOffset, data, size);
        }
    }

    VkResult ModelBuffer::flush(VkDeviceSize size, VkDeviceSize offset) {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = _memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkFlushMappedMemoryRanges(_graphicsDevice.device(), 1, &mappedRange);
    }

    VkResult ModelBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = _memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkInvalidateMappedMemoryRanges(_graphicsDevice.device(), 1, &mappedRange);
    }

    VkDescriptorBufferInfo ModelBuffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
        return VkDescriptorBufferInfo{_buffer, offset, size};
    }

    void ModelBuffer::writeToIndex(void *data, int index) { writeToBuffer(data, _instanceSize, index * _alignmentSize); }

    VkResult ModelBuffer::flushIndex(int index) { return flush(_alignmentSize, index * _alignmentSize); }

    VkDescriptorBufferInfo ModelBuffer::descriptorInfoAt(int index) {
        return descriptorInfo(_alignmentSize, index * _alignmentSize);
    }

    VkResult ModelBuffer::invalidateIndex(int index) {
        return invalidate(_alignmentSize, index * _alignmentSize);
    }
}
