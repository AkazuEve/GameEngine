#pragma once

#include <vector>
#include <string>
#include <functional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Dependecies/imgui/imgui.h"
#include "Dependecies/imgui/imgui_impl_glfw.h"
#include "Dependecies/imgui/imgui_impl_opengl3.h"

#include "Console.hpp"

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

	struct UIFunction {
		UIFunction(void (*func)(), bool* isEnaled, std::string name) {
			this->func = func;
			this->isEnaled = isEnaled;
			this->name = name;
		}
		void (*func)();
		bool* isEnaled;
		std::string name;
	};

	static GLFWwindow* m_pCurrentWindow = nullptr;
	static std::vector<UIWindow> m_UIWindows;

	static std::vector<UIFunction> m_UIFunctions;

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

		m_UIFunctions.push_back(UIFunction(Console::ConsoleUI, &Console::isEnabled, Console::name));
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
				Console::SendLine(std::string("Fount array with name: "), windowName, CONSOLE_MESSAGE_DEBUG);
			}
		}

		if (!foundWindow) {
			m_UIWindows.push_back(windowName);
			m_UIWindows[m_UIWindows.size() - 1].enabled = windowEnabled;
			Console::SendLine("Pushed in not yet existing window: ", windowName, CONSOLE_MESSAGE_DEBUG);
		}
		
		//It worky now
		//Now I realize that it didn't work before cause the iterator based for loop was working with copies, I'm bad at this lmao
		for (unsigned int i{ 0 }; i < m_UIWindows.size(); i++) {
			if (m_UIWindows[i].windowName == windowName) {
				if (!(m_UIWindows[i].elementCount >= 10)) {
					m_UIWindows[i].element[m_UIWindows[i].elementCount] = element;
					m_UIWindows[i].elementCount++;
					Console::SendLine("Found existing window: ", windowName, " and function ptr has been pushed in", CONSOLE_MESSAGE_DEBUG);
				}
			}
		}
	}

	void RegisterFunction(void (*func)(), bool* isEnabled, std::string name) {
		m_UIFunctions.push_back(UIFunction(func, isEnabled, name));
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

	void RenderUI() {   //Render all UI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		static int width, height;
		glfwGetWindowSize(m_pCurrentWindow, &width, &height);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Exit")) { glfwSetWindowShouldClose(m_pCurrentWindow, true); }
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Windows")) {
				for (int i{ 0 }; i < m_UIWindows.size(); i++) {
					ImGui::Checkbox(m_UIWindows[i].windowName.c_str(), &m_UIWindows[i].enabled);
				}
				for (unsigned int i{ 0 }; i < m_UIFunctions.size(); i++) {
					ImGui::Checkbox(m_UIFunctions[i].name.c_str(), m_UIFunctions[i].isEnaled);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		ImGui::DockSpaceOverViewport();

		for (unsigned int w{ 0 }; w < m_UIWindows.size(); w++) {
			if (m_UIWindows[w].enabled) {
				ImGui::Begin(m_UIWindows[w].windowName.c_str());
				for (unsigned int i{ 0 }; i < m_UIWindows[w].elementCount; i++) {
					m_UIWindows[w].element[i]->OnUIRender();
				}
				ImGui::End();
			}
		}

		for (unsigned int i{ 0 }; i < m_UIFunctions.size(); i++) {
			if(*m_UIFunctions[i].isEnaled)
			m_UIFunctions[i].func();
		}


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		//This is here for the future when i want to go back to multi-vieport stuff
		//static GLFWwindow* backup_current_context; backup_current_context = glfwGetCurrentContext();
		//ImGui::UpdatePlatformWindows();
		//ImGui::RenderPlatformWindowsDefault();
		//glfwMakeContextCurrent(backup_current_context);
	}
}