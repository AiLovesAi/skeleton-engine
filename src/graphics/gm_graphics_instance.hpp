#pragma once

#include "gm_window.hpp"

#include <vulkan/vulkan.hpp>

#include <vector>

namespace game {
    class GraphicsInstance {
        public:
            // Constructors
            GraphicsInstance(Window* window);
            ~GraphicsInstance();

            GraphicsInstance(const GraphicsInstance &) = delete;
            GraphicsInstance &operator=(const GraphicsInstance &) = delete;
            GraphicsInstance(GraphicsInstance &&) = delete;
            GraphicsInstance &operator=(GraphicsInstance &&) = delete;

            // Functions
            VkInstance instance() { return instance_; }
            VkSurfaceKHR surface() { return surface_; }

            // Variables
            bool enableValidationLayers = false;

            const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
        private:
            // Functions
            void createInstance();
            void setupDebugMessenger();

            bool checkValidationLayerSupport();
            std::vector<const char *> getRequiredExtensions();
            void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
            void checkGFLWHasRequiredInstanceExtensions();

            // Variables
            Window* window_;

            VkInstance instance_;
            VkSurfaceKHR surface_;
            
            VkDebugUtilsMessengerEXT debugMessenger_;
    };
}
