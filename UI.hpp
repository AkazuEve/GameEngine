#pragma once

#include "Debug.hpp"

#include <vector>
#include <string>
#include <functional>

#include "Dependecies/imgui/imgui.h"
#include "Dependecies/imgui/imgui_impl_glfw.h"
#include "Dependecies/imgui/imgui_impl_opengl3.h"

namespace UIManager {
	class UIElement {
	public:
		UIElement() {};
		~UIElement() {};

		UIElement* GetUIElementPtr() { return this; };

		virtual void OnUIRender() = 0;
	private:
	};

	struct UIWindow {
		UIWindow(std::string name) : windowName(name) { 
			for (unsigned int i{ 0 }; i < 10; i++) {
				element[i] = nullptr;
			}
		}

		UIElement* element[10];
		unsigned int elementCount = 0;
		
		std::string windowName;
		bool enabled{ true };
	};

	static GLFWwindow* m_pCurrentWindow = nullptr;

	static GLuint m_frameBuffer;
	static GLuint m_frameBufferColorTexture;
	static GLuint m_frameBufferDepthTexture;

	static std::vector<UIWindow> m_UIWindows;

	static ImVec2* m_pViewportSize;

	void InitImGui(GLFWwindow* window, ImVec2* viewportSize) {
		m_pCurrentWindow = window;
		m_pViewportSize = viewportSize;
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

		glGenFramebuffers(1, &m_frameBuffer);
		glGenTextures(1, &m_frameBufferColorTexture);
		glGenTextures(1, &m_frameBufferDepthTexture);

		glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

		glBindTexture(GL_TEXTURE_2D, m_frameBufferColorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_pViewportSize->x, m_pViewportSize->y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, m_frameBufferDepthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_pViewportSize->x, m_pViewportSize->y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_frameBufferColorTexture, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_frameBufferDepthTexture, 0);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FreeImGui() { //Free all resources
		glDeleteTextures(1, &m_frameBufferColorTexture);
		glDeleteTextures(1, &m_frameBufferDepthTexture);
		glDeleteFramebuffers(1, &m_frameBuffer);

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void RegisterElement(UIElement* element, std::string windowName, bool windowEnabled) {
		bool foundWindow = false;
		for (UIWindow window : m_UIWindows) {
			if (window.windowName == windowName) {
				foundWindow = true;
				DEBUGPRINT("Fount array with name: " << windowName);
			}
		}

		if (!foundWindow) {
			m_UIWindows.push_back(windowName);
			m_UIWindows[m_UIWindows.size() - 1].enabled = windowEnabled;
			DEBUGPRINT("Pushed in not yet existing window: " << windowName);
		}
		
		//It worky now
		for (unsigned int i{ 0 }; i < m_UIWindows.size(); i++) {
			if (m_UIWindows[i].windowName == windowName) {
				if (!(m_UIWindows[i].elementCount >= 10)) {
					m_UIWindows[i].element[m_UIWindows[i].elementCount] = element;
					m_UIWindows[i].elementCount++;
					DEBUGPRINT("Found existing window: " << windowName << " and function ptr has been pushed in");
				}
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

		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void RenderUI() {   //Render all UI
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Exit")) { glfwSetWindowShouldClose(m_pCurrentWindow, true); }
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Windows")) {
				for (int i{ 0 }; i < m_UIWindows.size(); i++) {
					ImGui::Checkbox(m_UIWindows[i].windowName.c_str(), &m_UIWindows[i].enabled);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		ImGui::DockSpaceOverViewport();

		for (UIWindow window : m_UIWindows) {
			if (window.enabled) {
				ImGui::Begin(window.windowName.c_str());
				for (int i{ 0 }; i < window.elementCount; i++) {
					window.element[i]->OnUIRender();
				}
					ImGui::End();
			}
		}
		
		ImGui::Begin("Viewport");
		ImGui::BeginChild("Viewport");

		*m_pViewportSize = ImGui::GetWindowSize();

		glBindTexture(GL_TEXTURE_2D, m_frameBufferColorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)m_pViewportSize->x, (GLsizei)m_pViewportSize->y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		ImGui::Image(*(ImTextureID*)&m_frameBufferColorTexture, *m_pViewportSize, ImVec2(0, 1), ImVec2(1, 0));

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
		glViewport(0, 0, (GLsizei)m_pViewportSize->x, (GLsizei)m_pViewportSize->y);
	}
}