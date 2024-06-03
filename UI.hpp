#pragma once

#include "Debug.hpp"

#include <vector>
#include <string>
#include <functional>

#include "Dependecies/imgui/imgui.h"
#include "Dependecies/imgui/imgui_impl_glfw.h"
#include "Dependecies/imgui/imgui_impl_opengl3.h"

namespace UIManager {

	ImVec2 m_viewportSize{ 100.0f, 100.0f };

	struct UIWindow {
		UIWindow(std::string name) : windowName(name) {};

		std::vector<void (*)()> pUIFunctions;
		std::string windowName;
		bool enabled{ true };
	};

	class UIManager { 
	public:
		UIManager() {
			glGenFramebuffers(1, &m_frameBuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

			glGenTextures(1, &m_frameBufferTexture);
			glBindTexture(GL_TEXTURE_2D, m_frameBufferTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_frameBufferTexture, 0);
		}
		~UIManager() {
			//Free all resources
			glDeleteTextures(1, &m_frameBufferTexture);
			glDeleteFramebuffers(1, &m_frameBuffer);

			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}

		void InitImGui(GLFWwindow* window) {
			m_pCurrentWindow = window;
			//Setup ImGui
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
			//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
			//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     // Enable Multi-Viewport / Platform Windows

			io.ConfigViewportsNoTaskBarIcon = true;

			ImGui::StyleColorsDark();

			ImGui_ImplGlfw_InitForOpenGL(m_pCurrentWindow, true);
			ImGui_ImplOpenGL3_Init("#version 450");
		}

		void RegisterElement(void (*func)(), std::string windowName) {

			if (m_UIWindows.size() == 0) {
				m_UIWindows.push_back(UIWindow{windowName});
				DEBUGPRINT("Size 0");
			}

			bool foundWindow = false;
			for (UIWindow window : m_UIWindows) {
				if (window.windowName == windowName) {
					foundWindow = true;
					DEBUGPRINT("Fount array with name: " << windowName);
				}
			}

			if (!foundWindow) {
				m_UIWindows.push_back(windowName);
				DEBUGPRINT("Pushed in not yet existing window:" << windowName);
			}

			for (UIWindow window : m_UIWindows) {
				if (window.windowName == windowName) {
					window.pUIFunctions.push_back(func);
					DEBUGPRINT("Found existing window: " << windowName << " and function ptr has been pushed in");
				}
			}
		}

		void BeginFrame() {   //New frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			static int width, height;
			glfwGetWindowSize(m_pCurrentWindow, &width, &height);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, width, height);
		}

		void RenderUI() {   //Render all UI
			ImGui::DockSpaceOverViewport();

			/*
			if (ImGui::BeginMainMenuBar()) {
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Exit")) { glfwSetWindowShouldClose(pCurrentWindow, true); }
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Windows"))
				{
					ImGui::Checkbox("Debug", &debugWindow);
					ImGui::Checkbox("Profiler", &profilerWindow);
					ImGui::Checkbox("Models", &modelWindow);
					ImGui::Checkbox("Viewport", &viewportWindow);
					ImGui::Checkbox("Renderer", &rendererWindow);
					ImGui::Checkbox("ImGui Demo Window", &demoWindow);

					ImGui::EndMenu();
				}
				ImGui::EndMainMenuBar();
			}
			*/

				ImGui::Begin("Viewport");
				ImGui::BeginChild("Viewport");

				m_viewportSize = ImGui::GetWindowSize();

				glBindTexture(GL_TEXTURE_2D, m_frameBufferTexture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)m_viewportSize.x, (GLsizei)m_viewportSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
				ImGui::Image(*(ImTextureID*)&m_frameBufferTexture, m_viewportSize, ImVec2(0, 1), ImVec2(1, 0));

				ImGui::EndChild();
				ImGui::End();
			
		}

		void EndFrame() {   //Render UI
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			static GLFWwindow* backup_current_context; backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);

			glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
			glViewport(0, 0, (GLsizei)m_viewportSize.x, (GLsizei)m_viewportSize.y);
		}

	private:
		GLFWwindow* m_pCurrentWindow = nullptr;

		GLuint m_frameBuffer;
		GLuint m_frameBufferTexture;

		std::vector<UIWindow> m_UIWindows;
	};
}