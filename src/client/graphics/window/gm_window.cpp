#include "gm_window.hpp"

#include <common/data/gm_file.hpp>
#include <common/data/gm_logger.hpp>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>

#include <sstream>

namespace game {
    Window::Window(const std::string& title) {
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

    Window::Window(int w, int h, const std::string& title) : width_{w}, height_{h} {
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

    void Window::createWindow(const std::string& title, const GLFWvidmode* mode) {
        // Set window hints
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    
        // Create window
        window_ = glfwCreateWindow(width_, height_, title.c_str(), nullptr, nullptr);
        glfwSetWindowPos(window_, (mode->width - width_) / 2, (mode->height - height_) / 2);
        glfwSetWindowUserPointer(window_, this);

        // Get icon
        GLFWimage icons[1];
        icons[0].pixels = stbi_load(File::executableDir().append("../assets/icon.png").c_str(), &icons[0].width, &icons[0].height, 0, 4);
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
