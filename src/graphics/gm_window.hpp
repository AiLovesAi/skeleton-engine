#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <map>
#include <string>

namespace game {

class Window {
    public:
        Window(int w, int h, std::string name);
        ~Window();

        Window(const Window &) = delete;
        Window &operator=(const Window &) = delete;
        Window(Window&&) = default;
        Window &operator=(Window&&) = default;

        GLFWwindow* getGLFWwindow() const { return window; }
        bool shouldClose() { return glfwWindowShouldClose(window); }
        VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
        bool windowResized() { return frameBufferResized; }
        bool windowFocused() { return focused; }

        void resetWindowResizedFlag() { frameBufferResized = false; }

        void showWindow();
        void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
    
    private:
        void initWindow();

        int width;
        int height;
        bool frameBufferResized = false;
        bool focused = true;

        std::string windowTitle;
        GLFWwindow* window;

        // Callback functions
        static void errorCallback(int error, const char* description);
        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
        static void focusCallback(GLFWwindow* window, int focused);
};
}
