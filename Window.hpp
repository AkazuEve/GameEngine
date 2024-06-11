#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Dependecies/imgui/imgui.h"
#include "stb/stbi.h"

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

    std::stringstream debugMessage;

    debugMessage << "---------------" << std::endl;
    debugMessage << "Debug message (" << id << "): " << message << std::endl;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             debugMessage << "Source: API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   debugMessage << "Source: Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: debugMessage << "Source: Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     debugMessage << "Source: Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     debugMessage << "Source: Application"; break;
    case GL_DEBUG_SOURCE_OTHER:           debugMessage << "Source: Other"; break;
    } debugMessage << std::endl;

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               debugMessage << "Type: Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: debugMessage << "Type: Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  debugMessage << "Type: Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         debugMessage << "Type: Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         debugMessage << "Type: Performance"; break;
    case GL_DEBUG_TYPE_MARKER:              debugMessage << "Type: Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          debugMessage << "Type: Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           debugMessage << "Type: Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:               debugMessage << "Type: Other"; break;
    } debugMessage << std::endl;

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:         debugMessage << "Severity: high"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       debugMessage << "Severity: medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          debugMessage << "Severity: low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: debugMessage << "Severity: notification"; break;
    } debugMessage << std::endl;
    debugMessage << std::endl;

    Console::SendLine(debugMessage.str(), CONSOLE_MESSAGE_ERROR);
}

class Window: UIManager::UIElement {
public:
    Window(unsigned int width, unsigned int height) {
        glfwInit();

        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
        glfwWindowHint(GLFW_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        
        m_window = glfwCreateWindow(width, height, "Engine", nullptr, nullptr);

        glfwMakeContextCurrent(m_window);
        glfwSwapInterval(m_swapInterval);

        gladLoadGL();

        Console::SendLine("OpenGL debug enabled", CONSOLE_MESSAGE_SUCCESS);
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);


        //Load and set icon
        GLFWimage image;
        image.pixels = stbi_load("Resources/icon.png", &image.width, &image.height, 0, 4); //rgba channels 
        glfwSetWindowIcon(m_window, 1, &image);
        stbi_image_free(image.pixels);


        UIManager::RegisterElement(this, "App", true);
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
    bool m_decorated = false;

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