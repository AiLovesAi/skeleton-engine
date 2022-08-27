#include "gm_descriptors.hpp"

#include "../util/gm_logger.hpp"

namespace game {
    /* Descriptor Set Layout Builder */

    DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::addBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count
    ) {
        if (bindings.count(binding) != 0) Logger::crash("Binding already in use");

        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = count;
        layoutBinding.stageFlags = stageFlags;
        bindings[binding] = layoutBinding;

        return *this;
    }
    
    std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::build() const {
        return std::make_unique<DescriptorSetLayout>(graphicsDevice, bindings);
    }

    /* Descriptor Set Layout */

    DescriptorSetLayout::DescriptorSetLayout(
        GraphicsDevice* device, std::unordered_map<uint32_t,
        VkDescriptorSetLayoutBinding> bindings
    ) : graphicsDevice{device}, bindings{bindings} {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        for (auto kv : bindings) {
            setLayoutBindings.push_back(kv.second);
        }
        
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();
        
        if (vkCreateDescriptorSetLayout(
                device->getDevice(),
                &descriptorSetLayoutInfo,
                nullptr,
                &descriptorSetLayout
            ) != VK_SUCCESS
        ) {
            Logger::crash("Failed to create descriptor set layout.");
        }
    }
    
    DescriptorSetLayout::~DescriptorSetLayout() {
        vkDestroyDescriptorSetLayout(graphicsDevice->getDevice(), descriptorSetLayout, nullptr);
    }

    /* Descriptor Pool Builder */

    DescriptorPool::Builder &DescriptorPool::Builder::addPoolSize(
        VkDescriptorType descriptorType,
        uint32_t count
    ) {
        poolSizes.push_back({descriptorType, count});
        return *this;
    }
    
    DescriptorPool::Builder &DescriptorPool::Builder::setPoolFlags(VkDescriptorPoolCreateFlags flags) {
        poolFlags = flags;
        return *this;
    }
    DescriptorPool::Builder &DescriptorPool::Builder::setMaxSets(uint32_t count) {
        maxSets = count;
        return *this;
    }
    
    std::unique_ptr<DescriptorPool> DescriptorPool::Builder::build() const {
        return std::make_unique<DescriptorPool>(graphicsDevice, maxSets, poolFlags, poolSizes);
    }

    /* Descriptor Pool */

    DescriptorPool::DescriptorPool(
        GraphicsDevice* device,
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize> &poolSizes
    ) : graphicsDevice{device} {
        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        descriptorPoolInfo.maxSets = maxSets;
        descriptorPoolInfo.flags = poolFlags;
        
        if (vkCreateDescriptorPool(device->getDevice(), &descriptorPoolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            Logger::crash("Failed to create descriptor pool!");
        }
    }
    
    DescriptorPool::~DescriptorPool() {
        vkDestroyDescriptorPool(graphicsDevice->getDevice(), descriptorPool, nullptr);
    }
    
    bool DescriptorPool::allocateDescriptor(
        const VkDescriptorSetLayout descriptorSetLayout,
        VkDescriptorSet &descriptor
    ) const {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.pSetLayouts = &descriptorSetLayout;
        allocInfo.descriptorSetCount = 1;
        
        // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
        // a new pool whenever an old pool fills up. But this is beyond our current scope
        if (vkAllocateDescriptorSets(graphicsDevice->getDevice(), &allocInfo, &descriptor) != VK_SUCCESS) {
            return false;
        }
        return true;
    }
    
    void DescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const {
        vkFreeDescriptorSets(
            graphicsDevice->getDevice(),
            descriptorPool,
            static_cast<uint32_t>(descriptors.size()),
            descriptors.data()
        );
    }
    
    void DescriptorPool::resetPool() {
        vkResetDescriptorPool(graphicsDevice->getDevice(), descriptorPool, 0);
    }

    /* Descriptor Writer */

    DescriptorWriter::DescriptorWriter(
        DescriptorSetLayout& setLayout,
        DescriptorPool& pool
    ) : setLayout{setLayout}, pool{pool} {}
    
    DescriptorWriter &DescriptorWriter::writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo) {
        if (setLayout.bindings.count(binding) != 1) Logger::crash("Layout does not contain specified binding.");
    
        auto &bindingDescription = setLayout.bindings[binding];
        
        if (bindingDescription.descriptorCount != 1) {
            Logger::crash("Binding single descriptor info, but binding expects multiple.");
        }
        
        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pBufferInfo = bufferInfo;
        write.descriptorCount = 1;
        
        writes.push_back(write);
        return *this;
    }
    
    DescriptorWriter &DescriptorWriter::writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo) {
        if (setLayout.bindings.count(binding) != 1) Logger::crash("Layout does not contain specified binding.");
        
        auto &bindingDescription = setLayout.bindings[binding];
        
        if (bindingDescription.descriptorCount != 1) {
            Logger::crash("Binding single descriptor info, but binding expects multiple.");
        }
        
        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pImageInfo = imageInfo;
        write.descriptorCount = 1;
        
        writes.push_back(write);
        return *this;
    }
    
    bool DescriptorWriter::build(VkDescriptorSet &set) {
        bool success = pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set);
        if (!success) return false;

        overwrite(set);
        return true;
    }
    
    void DescriptorWriter::overwrite(VkDescriptorSet &set) {
        for (auto &write : writes) {
            write.dstSet = set;
        }

        vkUpdateDescriptorSets(pool.graphicsDevice->getDevice(), writes.size(), writes.data(), 0, nullptr);
    }
}