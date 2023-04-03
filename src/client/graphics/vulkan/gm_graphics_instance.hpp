#pragma once

#include "../window/gm_window.hpp"

#include <vulkan/vulkan.hpp>

#include <vector>

namespace game {
    class GraphicsInstance {
        public:
            // Constructors
            GraphicsInstance(Window& window);
            ~GraphicsInstance();

            GraphicsInstance(const GraphicsInstance &) = delete;
            GraphicsInstance &operator=(const GraphicsInstance &) = delete;
            GraphicsInstance(GraphicsInstance &&) = delete;
            GraphicsInstance &operator=(GraphicsInstance &&) = delete;

            // Functions
            VkInstance instance() { return _instance; }
            VkSurfaceKHR surface() { return _surface; }

            // Variables
            bool enableValidationLayers = false;

            const std::vector<const char *> validationLayers = {"VK_LAYER_KHRON_OSvalidation"};
        private:
            // Functions
            void createInstance();
            void setupDebugMessenger();

            bool checkValidationLayerSupport();
            std::vector<const char *> getRequiredExtensions();
            void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
            void checkGFLWHasRequiredInstanceExtensions();

            // Variables
            Window& _window;

            VkInstance _instance;
            VkSurfaceKHR _surface;
            
            VkDebugUtilsMessengerEXT _debugMessenger;
    };
}
