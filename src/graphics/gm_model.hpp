#pragma once

#include "gm_graphics_device.hpp"
#include "gm_model_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vulkan/vulkan.hpp>

#include <vector>

namespace game {
    class Model {
        public:
            // Types
            struct Vertex {
                alignas(16) glm::vec3 position{};
                alignas(16) glm::vec3 color{};
                alignas(16) glm::vec3 normal{};
                alignas(16) glm::vec3 uv{};

                static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
                static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

                bool operator==(const Vertex &other) const {
                    return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
                }
            };

            struct Builder {
                std::vector<Vertex> vertices{};
                std::vector<uint32_t> indices{};

                void loadModel(const std::string &filePath);
            };

            // Constructors
            Model(GraphicsDevice* device, const Model::Builder& builder);
            ~Model();

            Model(const Model &) = delete;
            Model &operator=(const Model &) = delete;
            Model(Model&&) = default;
            Model &operator=(Model&&) = default;

            static std::unique_ptr<Model> createModelFromFile(GraphicsDevice* device, const std::string filePath);

            void bind(VkCommandBuffer commandBuffer);
            void draw(VkCommandBuffer commandBuffer);
    
        private:
            void createVertexBuffers(const std::vector<Vertex>& vertices);
            void createIndexBuffers(const std::vector<uint32_t>& indices);

            GraphicsDevice* graphicsDevice_;

            std::unique_ptr<ModelBuffer> vertexBuffer_;
            uint32_t vertexCount_;
            
            bool hasIndexBuffer_ = false;
            std::unique_ptr<ModelBuffer> indexBuffer_;
            uint32_t indexCount_;
    };
}
