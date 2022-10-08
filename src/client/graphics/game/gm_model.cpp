#include "gm_model.hpp"

#include <common/data/gm_logger.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <cstring>
#include <unordered_map>

namespace std {
    template <typename T, typename... Rest>
    static void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
        seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        (hashCombine(seed, rest), ...);
    };

    template <>
    struct hash<game::Model::Vertex> {
        size_t operator()(game::Model::Vertex const& vertex) const {
            size_t seed = 0;
            hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
            return seed;
        }
    };
}

namespace game {
    Model::Model(GraphicsDevice& graphicsDevice, const Model::Builder& builder) : graphicsDevice_{graphicsDevice} {
        createVertexBuffers(builder.vertices);
        createIndexBuffers(builder.indices);
    }

    std::unique_ptr<Model> Model::createModelFromFile(GraphicsDevice& graphicsDevice, const std::string filePath) {
        Builder builder{};
        builder.loadModel(filePath);

        return std::make_unique<Model>(graphicsDevice, builder);
    }

    void Model::createVertexBuffers(const std::vector<Vertex> &vertices) {
        vertexCount_ = static_cast<uint32_t>(vertices.size());
        if (vertexCount_ < 3) Logger::crash("Vertex count must be at least 3.");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount_;
        uint32_t vertexSize = sizeof(vertices[0]);

        ModelBuffer stagingBuffer {
            graphicsDevice_,
            vertexSize,
            vertexCount_,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        };
        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void *) vertices.data());

        vertexBuffer_ = std::make_unique<ModelBuffer>(
            graphicsDevice_,
            vertexSize,
            vertexCount_,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

        graphicsDevice_.copyBuffer(stagingBuffer.buffer(), vertexBuffer_->buffer(), bufferSize);
    }

    void Model::createIndexBuffers(const std::vector<uint32_t> &indices) {
        indexCount_ = static_cast<uint32_t>(indices.size());
        hasIndexBuffer_ = indexCount_ > 0;

        if (!hasIndexBuffer_) return;

        VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount_;
        uint32_t indexSize = sizeof(indices[0]);

        ModelBuffer stagingBuffer {
            graphicsDevice_,
            indexSize,
            indexCount_,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        };
        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void *) indices.data());

        indexBuffer_ = std::make_unique<ModelBuffer>(
            graphicsDevice_,
            indexSize,
            indexCount_,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

        graphicsDevice_.copyBuffer(stagingBuffer.buffer(), indexBuffer_->buffer(), bufferSize);
    }

    void Model::bind(VkCommandBuffer commandBuffer) {
        VkBuffer buffers[] = {vertexBuffer_->buffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

        if (hasIndexBuffer_) {
            vkCmdBindIndexBuffer(commandBuffer, indexBuffer_->buffer(), 0, VK_INDEX_TYPE_UINT32);
        }
    }

    void Model::draw(VkCommandBuffer commandBuffer) {
        if (hasIndexBuffer_) {
            vkCmdDrawIndexed(commandBuffer, indexCount_, 1, 0, 0, 0);
        } else {
            vkCmdDraw(commandBuffer, vertexCount_, 1, 0, 0);
        }
    }

    std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
        attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
        attributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
        attributeDescriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)});
        
        return attributeDescriptions;
    }

    void Model::Builder::loadModel(const std::string &filePath) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str())) {
            throw std::runtime_error(warn + err);
        }

        vertices.clear();
        indices.clear();

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};
        for (const auto &shape : shapes) {
            for (const auto &index : shape.mesh.indices) {
                Vertex vertex{};

                if (index.vertex_index >= 0) {
                    vertex.position = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };

                    vertex.color = {
                        attrib.colors[3 * index.vertex_index + 0],
                        attrib.colors[3 * index.vertex_index + 1],
                        attrib.colors[3 * index.vertex_index + 2]
                    };
                }

                if (index.normal_index >= 0) {
                    vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    };
                }

                if (index.texcoord_index >= 0) {
                    vertex.uv = {
                        attrib.texcoords[3 * index.texcoord_index + 0],
                        attrib.texcoords[3 * index.texcoord_index + 1],
                        attrib.texcoords[3 * index.texcoord_index + 2]
                    };
                }

                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }
                indices.push_back(uniqueVertices[vertex]);
            }
        }
    }
}
