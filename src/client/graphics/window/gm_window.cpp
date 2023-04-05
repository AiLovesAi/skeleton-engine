#include "gm_window.hpp"

#include <common/headers/file.hpp>
#include <common/headers/string.hpp>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>

namespace game {
    Window::Window(const UTF8Str& title) {
        // Get monitor for window position, width, and height
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        {
            const char* monitorName = glfwGetMonitorName(monitor);
            UTF8Str msg = FormatString::formatString("Using monitor: %s", monitorName);
            Logger::log(LOG_INFO, msg);
        }

        // Set window width & height
        _width = mode->width / 2;
        _height = mode->height / 2;
        
        createWindow(title, mode);
    }

    Window::Window(int w, int h, const UTF8Str& title) : _width{w}, _height{h} {
        // Get monitor for window position
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        {
            const char* monitorName = glfwGetMonitorName(monitor);
            UTF8Str msg = FormatString::formatString("Using monitor: %s", monitorName);
            Logger::log(LOG_INFO, msg);
        }

        createWindow(title, mode);
    }

    Window::~Window() {
        glfwDestroyWindow(_window);
    }

    void Window::createWindow(const UTF8Str& title, const GLFWvidmode* mode) {
        // Set window hints
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    
        // Create window
        _window = glfwCreateWindow(_width, _height, title.get(), nullptr, nullptr);
        glfwSetWindowPos(_window, (mode->width - _width) / 2, (mode->height - _height) / 2);
        glfwSetWindowUserPointer(_window, this);

        // Get icon
        GLFWimage icons[1];
        char* path = static_cast<char*>(std::malloc(File::executableDir().length() + sizeof("../assets/icon.png")));
        std::memcpy(path, File::executableDir().get(), File::executableDir().length());
        std::memcpy(path + File::executableDir().length(), "../assets/icon.png", sizeof("../assets/icon.png"));

        icons[0].pixels = stbi_load(path, &icons[0].width, &icons[0].height, 0, 4);
        std::free(path);
        glfwSetWindowIcon(_window, 1, icons);
        stbi_image_free(icons[0].pixels);

        // Set window callbacks
        glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);
        glfwSetWindowFocusCallback(_window, focusCallback);
    }

    void Window::init() {
        glfwSetErrorCallback(reinterpret_cast<GLFWerrorfun>(errorCallback));
    }

    void Window::show() {
        glfwShowWindow(_window);
        glfwFocusWindow(_window);
    }

    void Window::createWindowSurface(const VkInstance instance, VkSurfaceKHR* surface) {
        if (glfwCreateWindowSurface(instance, _window, nullptr, surface) != VK_SUCCESS) {
            Logger::crash("Failed to create window surface.");
        }
    }

    void Window::errorCallback(int error, const char*__restrict__ description) {
        UTF8Str msg = FormatString::formatString("GLFW error code %d: %s", error, description);
        Logger::log(LOG_ERR, msg);
    }

    void Window::framebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height) {
        Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
        window->_frameBufferResized = true;
        window->_width = width;
        window->_height = height;
    }

    void Window::focusCallback(GLFWwindow* glfwWindow, int focused) {
        Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));

        window->_focused = focused;
    }
}
