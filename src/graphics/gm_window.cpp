#include "gm_window.hpp"

#include "../gm_game.hpp"
#include "../util/gm_logger.hpp"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>

#include <sstream>

namespace game {

    Window::Window(int w, int h, std::string name) : width{w}, height{h}, windowTitle{name} {
        if (!Game::isServer) initWindow();
    }

    Window::~Window() {
        if (!Game::isServer) {
            glfwDestroyWindow(window);
            glfwTerminate();
        }
    }

    void Window::initWindow() {
        glfwSetErrorCallback(reinterpret_cast<GLFWerrorfun>(errorCallback));
        if (glfwInit() != GLFW_TRUE) {
            Logger::crash("Failed to initialize GLFW.");
        }

        // Get monitor for window position
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        {
            const char* name = glfwGetMonitorName(monitor);
            std::stringstream msg;
            msg << "Using monitor: " << name;
            Logger::logMsg(LOG_INFO, msg.str());
        }
        
        // Set window hints
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    
        // Create window
        window = glfwCreateWindow(width, height, windowTitle.c_str(), nullptr, nullptr);
        glfwSetWindowPos(window, (mode->width - width) / 2, (mode->height - height) / 2);
        glfwSetWindowUserPointer(window, this);

        // Get icon
        GLFWimage icons[1];
        icons[0].pixels = stbi_load("assets/icon.png", &icons[0].width, &icons[0].height, 0, 4);
        glfwSetWindowIcon(window, 1, icons);
        stbi_image_free(icons[0].pixels);

        // Set window callbacks
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
        glfwSetWindowFocusCallback(window, focusCallback);
    }

    void Window::showWindow() {
        glfwShowWindow(window);
        glfwFocusWindow(window);
    }

    void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
            Logger::crash("Failed to create window surface.");
        }
    }

    void Window::errorCallback(int error, const char* description) {
        std::stringstream msg;
        msg << "GLFW error code " << error << ": " << description;
        Logger::logMsg(LOG_ERR, msg.str());
    }

    void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        Window* mcpWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        mcpWindow->frameBufferResized = true;
        mcpWindow->width = width;
        mcpWindow->height = height;
    }

    void Window::focusCallback(GLFWwindow* window, int focused) {
        Window* mcpWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

        if (focused) {
            mcpWindow->focused = true;
        } else {
            mcpWindow->focused = false;
        }
    }
}
