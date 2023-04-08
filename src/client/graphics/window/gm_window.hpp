#pragma once

#include <common/headers/string.hpp>
#include <common/headers/float.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <map>
#include <string>

namespace game {

class Window {
    public:
        // Constructors
        Window(const UTF8Str& title);
        Window(int w, int h, const UTF8Str& title);
        ~Window();

        Window(const Window &) = delete;
        Window &operator=(const Window &) = delete;
        Window(Window&&) = default;
        Window &operator=(Window&&) = default;

        // Functions
        static void init();

        GLFWwindow* window() const { return _window; }
        bool shouldClose() { return glfwWindowShouldClose(_window); }
        VkExtent2D extent() { return {static_cast<uint32_t>(_width), static_cast<uint32_t>(_height)}; }
        float32_t aspectRatio() { return static_cast<float32_t>(_width) / static_cast<float32_t>(_height); }
        bool wasResized() { return _frameBufferResized; }
        bool isFocused() { return _focused; }
        void resetWindowResizedFlag() { _frameBufferResized = false; }

        void show();
        void createWindowSurface(const VkInstance instance, VkSurfaceKHR* surface);
    
    private:
        // Functions
        void createWindow(const UTF8Str& title, const GLFWvidmode* mode);

        // Callback functions
        static void errorCallback(int error, const char*__restrict__ description);
        static void framebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height);
        static void focusCallback(GLFWwindow* glfwWindow, int focused);

        // Variables
        int _width;
        int _height;
        bool _frameBufferResized = false;
        bool _focused = true;

        GLFWwindow* _window;
};
}
