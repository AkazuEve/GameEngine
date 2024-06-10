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
		UIElement() = default;
		~UIElement() = default;

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
	static std::vector<UIWindow> m_UIWindows;

	static ImVec2* m_pViewportSize;

	void InitImGui() {
		m_pCurrentWindow = glfwGetCurrentContext();
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

	void FreeImGui() { //Free all resources
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void RegisterElement(UIElement* element, std::string windowName, bool windowEnabled) {
		bool foundWindow = false;

		for (unsigned int i{ 0 }; i < m_UIWindows.size(); i++) {
			if (m_UIWindows[i].windowName == windowName) {
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
		//Now I realize that it didn't work before cause the range based for loop was working with copies, I'm bad at this lmao
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

	void RenderViewport(std::string title, GLuint* texture, ImVec2& viewportSize) {
		ImGui::Begin(title.c_str());
		ImGui::BeginChild(title.c_str());

		viewportSize = ImGui::GetWindowSize();
		ImGui::Image(*(ImTextureID*)texture, viewportSize, ImVec2(0, 1), ImVec2(1, 0));
		
		ImGui::EndChild();
		ImGui::End();
	}

	void RenderViewport(std::string title, GLuint* texture) {
		ImGui::Begin(title.c_str());
		ImGui::BeginChild(title.c_str());

		ImVec2 bufferSize;
		bufferSize = ImGui::GetWindowSize();
		ImGui::Image(*(ImTextureID*)texture, bufferSize, ImVec2(0, 1), ImVec2(1, 0));

		ImGui::EndChild();
		ImGui::End();
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
				for (unsigned int i{ 0 }; i < window.elementCount; i++) {
					window.element[i]->OnUIRender();
				}
					ImGui::End();
			}
		}
	}

	void EndFrame() {   //Render UI
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		static GLFWwindow* backup_current_context; backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}