#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <map>
#include <string>

namespace game {

class Window {
    public:
        // Constructors
        Window(const std::string& title);
        Window(int w, int h, const std::string& title);
        ~Window();

        Window(const Window &) = delete;
        Window &operator=(const Window &) = delete;
        Window(Window&&) = default;
        Window &operator=(Window&&) = default;

        // Functions
        static void init();

        GLFWwindow* window() const { return window_; }
        bool shouldClose() { return glfwWindowShouldClose(window_); }
        VkExtent2D extent() { return {static_cast<uint32_t>(width_), static_cast<uint32_t>(height_)}; }
        bool wasResized() { return frameBufferResized_; }
        bool isFocused() { return focused_; }
        void resetWindowResizedFlag() { frameBufferResized_ = false; }

        void show();
        void createWindowSurface(const VkInstance instance, VkSurfaceKHR* surface);
    
    private:
        // Functions
        void createWindow(const std::string& title, const GLFWvidmode* mode);

        // Callback functions
        static void errorCallback(int error, const char* description);
        static void framebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height);
        static void focusCallback(GLFWwindow* glfwWindow, int focused);

        // Variables
        int width_;
        int height_;
        bool frameBufferResized_ = false;
        bool focused_ = true;

        GLFWwindow* window_;
};
}
