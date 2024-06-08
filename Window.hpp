#pragma once

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Dependecies/imgui/imgui.h"

#include "Debug.hpp"
#include "UI.hpp"

static void APIENTRY glDebugOutput(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    // ignore non-significant error/warning codes
    //if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
    case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
    case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}

class Window: UIManager::UIElement {
public:
    Window(unsigned int width, unsigned int height) {
        glfwInit();

        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
        glfwWindowHint(GLFW_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
        
        m_window = glfwCreateWindow(width, height, "Engine", nullptr, nullptr);

        glfwMakeContextCurrent(m_window);
        glfwSwapInterval(m_swapInterval);

        gladLoadGL();

        #ifdef _DEBUG
            DEBUGPRINT("OpenGL debug enabled");
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(glDebugOutput, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        #endif //  DEBUG

            UIManager::RegisterElement(GetUIElementPtr(), "App", true);
    }
    ~Window() {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    Window(const Window&) = delete;
    Window operator=(const Window&) = delete;
    
    GLFWwindow* GetWindowPtr() { return m_window; };

    bool ShouldRunNextFrame() { 
        glfwPollEvents(); 
        glfwSwapBuffers(m_window); 

        static ImGuiIO& io = ImGui::GetIO();
        static int windowPosX, windowPosY;

        if (io.MouseDown[0] && io.KeyCtrl) {
            glfwGetWindowPos(m_window, &windowPosX, &windowPosY);
            glfwSetWindowPos(m_window, (windowPosX += (int)(io.MouseDelta.x / 1.4f)), (windowPosY += (int)(io.MouseDelta.y / 1.4f)));
        }

        return !glfwWindowShouldClose(m_window); 
    }

private:
	GLFWwindow* m_window;
    bool m_swapInterval = 1;
    bool m_decorated = 1;

    virtual void OnUIRender() override {
        if(ImGui::TreeNode("Window Debug")) {
            if (ImGui::Checkbox("Lock framerate", &m_swapInterval)) {
                glfwSwapInterval(m_swapInterval);
            }
            if (ImGui::Checkbox("Decorated", &m_decorated)) {
                glfwSetWindowAttrib(m_window, GLFW_DECORATED, m_decorated);
            }
            static int width, height;
            glfwGetWindowSize(m_window, &width, &height);
            ImGui::Text(std::format("Window width/height: {}/{}", width, height).c_str());
            ImGui::TreePop();
        }
    }
};