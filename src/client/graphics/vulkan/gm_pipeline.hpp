#pragma once

#include "gm_graphics_device.hpp"

#include <string>
#include <vector>

namespace game {
    struct PipelineConfigInfo {
        PipelineConfigInfo() = default;
        PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo &operator=(const PipelineConfigInfo&) = delete;

        std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        std::vector<VkDynamicState> dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

    class Pipeline {
        public:
            // Constructors
            Pipeline(
                GraphicsDevice& graphicsDevice,
                const std::string& vertFilepath,
                const std::string& fragFilePath,
                const PipelineConfigInfo& configInfo
            );
            ~Pipeline();

            Pipeline(const Pipeline&) = delete;
            Pipeline &operator=(const Pipeline&) = delete;
            Pipeline(Pipeline&&) = default;
            Pipeline &operator=(Pipeline&&) = default;

            // Functions
            void bind(VkCommandBuffer commandBuffer);

            static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
            static void enableAlphaBlending(PipelineConfigInfo& configInfo);
        
        private:
            // Functions
            void createGraphicsPipeline(
                const std::string& vertFilepath,
                const std::string& fragFilePath,
                const PipelineConfigInfo& configInfo
            );

            void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
            
            static std::vector<char> readFile(const std::string filePath);

            // Variables
            GraphicsDevice& _graphicsDevice;
            VkPipeline _graphicsPipeline;
            VkShaderModule _vertShaderModule;
            VkShaderModule _fragShaderModule;
    };
}
