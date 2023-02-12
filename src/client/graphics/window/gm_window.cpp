#include "gm_window.hpp"

#include <common/data/file/gm_file.hpp>
#include <common/data/file/gm_logger.hpp>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>

#include <sstream>

namespace game {
    Window::Window(const UTF8Str& title) {
        // Get monitor for window position, width, and height
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        {
            const char* monitorName = glfwGetMonitorName(monitor);
            std::stringstream msg;
            msg << "Using monitor: " << monitorName;
            Logger::log(LOG_INFO, msg.str());
        }

        // Set window width & height
        width_ = mode->width / 2;
        height_ = mode->height / 2;
        
        createWindow(title, mode);
    }

    Window::Window(int w, int h, const UTF8Str& title) : width_{w}, height_{h} {
        // Get monitor for window position
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        {
            const char* monitorName = glfwGetMonitorName(monitor);
            std::stringstream msg;
            msg << "Using monitor: " << monitorName;
            Logger::log(LOG_INFO, msg.str());
        }

        createWindow(title, mode);
    }

    Window::~Window() {
        glfwDestroyWindow(window_);
    }

    void Window::createWindow(const UTF8Str& title, const GLFWvidmode* mode) {
        // Set window hints
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    
        // Create window
        window_ = glfwCreateWindow(width_, height_, title.str.get(), nullptr, nullptr);
        glfwSetWindowPos(window_, (mode->width - width_) / 2, (mode->height - height_) / 2);
        glfwSetWindowUserPointer(window_, this);

        // Get icon
        GLFWimage icons[1];
        char* path = static_cast<char*>(std::malloc(File::executableDir().len + sizeof("../assets/icon.png")));
        std::memcpy(path, File::executableDir().str.get(), File::executableDir().len);
        std::memcpy(path + File::executableDir().len, "../assets/icon.png", sizeof("../assets/icon.png"));

        icons[0].pixels = stbi_load(path, &icons[0].width, &icons[0].height, 0, 4);
        std::free(path);
        glfwSetWindowIcon(window_, 1, icons);
        stbi_image_free(icons[0].pixels);

        // Set window callbacks
        glfwSetFramebufferSizeCallback(window_, framebufferResizeCallback);
        glfwSetWindowFocusCallback(window_, focusCallback);
    }

    void Window::init() {
        glfwSetErrorCallback(reinterpret_cast<GLFWerrorfun>(errorCallback));
    }

    void Window::show() {
        glfwShowWindow(window_);
        glfwFocusWindow(window_);
    }

    void Window::createWindowSurface(const VkInstance instance, VkSurfaceKHR* surface) {
        if (glfwCreateWindowSurface(instance, window_, nullptr, surface) != VK_SUCCESS) {
            Logger::crash("Failed to create window surface.");
        }
    }

    void Window::errorCallback(int error, const char* description) {
        std::stringstream msg;
        msg << "GLFW error code " << error << ": " << description;
        Logger::log(LOG_ERR, msg.str());
    }

    void Window::framebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height) {
        Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
        window->frameBufferResized_ = true;
        window->width_ = width;
        window->height_ = height;
    }

    void Window::focusCallback(GLFWwindow* glfwWindow, int focused) {
        Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));

        window->focused_ = focused;
    }
}
