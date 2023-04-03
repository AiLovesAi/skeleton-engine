#include "gm_descriptors.hpp"

#include <common/data/file/gm_logger.hpp>

namespace game {
    /* Descriptor Set Layout Builder */

    DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::addBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count
    ) {
        if (_bindings.count(binding) != 0) Logger::crash("Binding already in use");

        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = count;
        layoutBinding.stageFlags = stageFlags;
        _bindings[binding] = layoutBinding;

        return *this;
    }
    
    std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::build() const {
        return std::make_unique<DescriptorSetLayout>(_graphicsDevice, _bindings);
    }

    /* Descriptor Set Layout */

    DescriptorSetLayout::DescriptorSetLayout(
        GraphicsDevice& graphicsDevice, std::unordered_map<uint32_t,
        VkDescriptorSetLayoutBinding> bindings
    ) : _graphicsDevice{graphicsDevice}, _bindings{bindings} {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        for (auto kv : bindings) {
            setLayoutBindings.push_back(kv.second);
        }
        
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();
        
        if (vkCreateDescriptorSetLayout(
                _graphicsDevice.device(),
                &descriptorSetLayoutInfo,
                nullptr,
                &_descriptorSetLayout
            ) != VK_SUCCESS
        ) {
            Logger::crash("Failed to create descriptor set layout.");
        }
    }
    
    DescriptorSetLayout::~DescriptorSetLayout() {
        vkDestroyDescriptorSetLayout(_graphicsDevice.device(), _descriptorSetLayout, nullptr);
    }

    /* Descriptor Pool Builder */

    DescriptorPool::Builder &DescriptorPool::Builder::addPoolSize(
        VkDescriptorType descriptorType,
        uint32_t count
    ) {
        _poolSizes.push_back({descriptorType, count});
        return *this;
    }
    
    DescriptorPool::Builder &DescriptorPool::Builder::setPoolFlags(VkDescriptorPoolCreateFlags flags) {
        _poolFlags = flags;
        return *this;
    }
    DescriptorPool::Builder &DescriptorPool::Builder::setMaxSets(uint32_t count) {
        _maxSets = count;
        return *this;
    }
    
    std::unique_ptr<DescriptorPool> DescriptorPool::Builder::build() const {
        return std::make_unique<DescriptorPool>(_graphicsDevice, _maxSets, _poolFlags, _poolSizes);
    }

    /* Descriptor Pool */

    DescriptorPool::DescriptorPool(
        GraphicsDevice& graphicsDevice,
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize> &poolSizes
    ) : _graphicsDevice{graphicsDevice} {
        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        descriptorPoolInfo.maxSets = maxSets;
        descriptorPoolInfo.flags = poolFlags;
        
        if (vkCreateDescriptorPool(_graphicsDevice.device(), &descriptorPoolInfo, nullptr, &_descriptorPool) != VK_SUCCESS) {
            Logger::crash("Failed to create descriptor pool!");
        }
    }
    
    DescriptorPool::~DescriptorPool() {
        vkDestroyDescriptorPool(_graphicsDevice.device(), _descriptorPool, nullptr);
    }
    
    bool DescriptorPool::allocateDescriptor(
        const VkDescriptorSetLayout descriptorSetLayout,
        VkDescriptorSet &descriptor
    ) const {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = _descriptorPool;
        allocInfo.pSetLayouts = &descriptorSetLayout;
        allocInfo.descriptorSetCount = 1;
        
        // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
        // a new pool whenever an old pool fills up. But this is beyond our current scope
        if (vkAllocateDescriptorSets(_graphicsDevice.device(), &allocInfo, &descriptor) != VK_SUCCESS) {
            return false;
        }
        return true;
    }
    
    void DescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const {
        vkFreeDescriptorSets(
            _graphicsDevice.device(),
            _descriptorPool,
            static_cast<uint32_t>(descriptors.size()),
            descriptors.data()
        );
    }
    
    void DescriptorPool::resetPool() {
        vkResetDescriptorPool(_graphicsDevice.device(), _descriptorPool, 0);
    }

    /* Descriptor Writer */

    DescriptorWriter::DescriptorWriter(
        DescriptorSetLayout& setLayout,
        DescriptorPool& pool
    ) : _setLayout{setLayout}, _pool{pool} {}
    
    DescriptorWriter &DescriptorWriter::writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo) {
        if (_setLayout._bindings.count(binding) != 1) Logger::crash("Layout does not contain specified binding.");
    
        auto &bindingDescription = _setLayout._bindings[binding];
        
        if (bindingDescription.descriptorCount != 1) {
            Logger::crash("Binding single descriptor info, but binding expects multiple.");
        }
        
        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pBufferInfo = bufferInfo;
        write.descriptorCount = 1;
        
        _writes.push_back(write);
        return *this;
    }
    
    DescriptorWriter &DescriptorWriter::writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo) {
        if (_setLayout._bindings.count(binding) != 1) Logger::crash("Layout does not contain specified binding.");
        
        auto &bindingDescription = _setLayout._bindings[binding];
        
        if (bindingDescription.descriptorCount != 1) {
            Logger::crash("Binding single descriptor info, but binding expects multiple.");
        }
        
        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pImageInfo = imageInfo;
        write.descriptorCount = 1;
        
        _writes.push_back(write);
        return *this;
    }
    
    bool DescriptorWriter::build(VkDescriptorSet &set) {
        bool success = _pool.allocateDescriptor(_setLayout.descriptorSetLayout(), set);
        if (!success) return false;

        overwrite(set);
        return true;
    }
    
    void DescriptorWriter::overwrite(VkDescriptorSet &set) {
        for (auto &write : _writes) {
            write.dstSet = set;
        }

        vkUpdateDescriptorSets(_pool._graphicsDevice.device(), _writes.size(), _writes.data(), 0, nullptr);
    }
}