#pragma once

#include "gm_graphics_device.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

namespace game {
    class DescriptorSetLayout {
        public:
            class Builder {
                public:
                    Builder(GraphicsDevice* device) : graphicsDevice{device} {}
 
                    Builder& addBinding(
                        uint32_t binding,
                        VkDescriptorType descriptorType,
                        VkShaderStageFlags stageFlags,
                        uint32_t count = 1
                    );
                    std::unique_ptr<DescriptorSetLayout> build() const;
                
                private:
                    GraphicsDevice* graphicsDevice;
                    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
            };

            // Constructors
            DescriptorSetLayout(
                GraphicsDevice* mcpGraphicsDevice,
                std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings
            );
            ~DescriptorSetLayout();

            DescriptorSetLayout(const DescriptorSetLayout &) = delete;
            DescriptorSetLayout &operator=(const DescriptorSetLayout &) = delete;

            // Functions
            VkDescriptorSetLayout descriptorSetLayout() const { return descriptorSetLayout_; }

        private:
            GraphicsDevice* graphicsDevice_;
            VkDescriptorSetLayout descriptorSetLayout_;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings_;
            
            friend class DescriptorWriter;
    };

    class DescriptorPool {
        public:
            class Builder {
            public:
                Builder(GraphicsDevice* device) : graphicsDevice{device} {}
            
                Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
                Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
                Builder& setMaxSets(uint32_t count);
                std::unique_ptr<DescriptorPool> build() const;
            
            private:
                GraphicsDevice* graphicsDevice;
                std::vector<VkDescriptorPoolSize> poolSizes{};
                uint32_t maxSets = 1000;
                VkDescriptorPoolCreateFlags poolFlags = 0;
            };
    
            // Constructors
            DescriptorPool(
                GraphicsDevice* mcpGraphicsDevice,
                uint32_t maxSets,
                VkDescriptorPoolCreateFlags poolFlags,
                const std::vector<VkDescriptorPoolSize> &poolSizes);
            ~DescriptorPool();

            DescriptorPool(const DescriptorPool &) = delete;
            DescriptorPool &operator=(const DescriptorPool &) = delete;
            
            // Functions
            bool allocateDescriptor(
                const VkDescriptorSetLayout descriptorSetLayout,
                VkDescriptorSet &descriptor
            ) const;
            
            void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;
            
            void resetPool();
        
        private:
            GraphicsDevice* graphicsDevice_;
            VkDescriptorPool descriptorPool_;
            
            friend class DescriptorWriter;
    };
    
    class DescriptorWriter {
        public:
            DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool);
            
            DescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
            DescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);
            
            bool build(VkDescriptorSet &set);
            void overwrite(VkDescriptorSet &set);
        
        private:
            DescriptorSetLayout& setLayout_;
            DescriptorPool& pool_;
            std::vector<VkWriteDescriptorSet> writes_;
    };
}
