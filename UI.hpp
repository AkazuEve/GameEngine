#pragma once

#include "Debug.hpp"

#include <map>
#include <vector>
#include <algorithm>

#include "Dependecies/imgui/imgui.h"
#include "Dependecies/imgui/imgui_impl_glfw.h"
#include "Dependecies/imgui/imgui_impl_opengl3.h"

#include "UIVariables.hpp"
#include "UIStructs.hpp"

namespace UIManager {
	void Init(GLFWwindow* window) {
		pCurrentWindow = window;

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

		ImGui_ImplGlfw_InitForOpenGL(pCurrentWindow, true);
		ImGui_ImplOpenGL3_Init("#version 450");

		glGenFramebuffers(1, &frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

		glGenTextures(1, &frameBufferTexture);
		glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 800, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferTexture, 0);
	}

	void Render() {
		static int width, height;
		glfwGetWindowSize(pCurrentWindow, &width, &height);
		static ImVec2 vportSize;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
		//New frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		//Render all UI windows (I am addicted to reinterpret_cast)
		ImGui::DockSpaceOverViewport();

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
				ImGui::Checkbox("ImGui Demo Window" ,&demoWindow);

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		if (viewportWindow) {
			ImGui::Begin("Viewport", &viewportWindow);
			ImGui::BeginChild("Viewport");

			vportSize = ImGui::GetWindowSize();
			viewportWidth = (unsigned int)vportSize.x;
			viewportHeight = (unsigned int)vportSize.y;

			glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)vportSize.x, (GLsizei)vportSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			ImGui::Image(*(ImTextureID*)&frameBufferTexture, vportSize, ImVec2(0, 1), ImVec2(1, 0));

			ImGui::EndChild();
			ImGui::End();
		}

		if (debugWindow) {
			ImGui::Begin("Debug", &debugWindow);
			for (void* element: registeredElements) {
				if (reinterpret_cast<UIElement*>(element)->GetType() == UI_DEBUG)
					reinterpret_cast<UIElement*>(element)->OnUIRender();
			}
			ImGui::End();
		}

		if (modelWindow) {
			ImGui::Begin("Models", &modelWindow);
			for (void* element: registeredElements) {
				if (reinterpret_cast<UIElement*>(element)->GetType() == UI_MODEL)
					reinterpret_cast<UIElement*>(element)->OnUIRender();
			}
			ImGui::End();
		}
		
		if (profilerWindow) {
			ImGui::Begin("Profiler", &profilerWindow);
			for (void* element : registeredElements) {
				if (reinterpret_cast<UIElement*>(element)->GetType() == UI_PROFILER)
					reinterpret_cast<UIElement*>(element)->OnUIRender();
			}
			ImGui::End();
		}
		
		if (rendererWindow) {
			ImGui::Begin("Renderer", &rendererWindow);
			for (void* element : registeredElements) {
				if (reinterpret_cast<UIElement*>(element)->GetType() == UI_RENDERER)
					reinterpret_cast<UIElement*>(element)->OnUIRender();
			}
			ImGui::End();
		}

		if (demoWindow) {
			ImGui::ShowDemoWindow(&demoWindow);
		}
	

		//Render and handle muli window bs
		ImGui::Render();
		static int display_w, display_h;
		glfwGetFramebufferSize(pCurrentWindow, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		static GLFWwindow* backup_current_context; backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);

		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glViewport(0, 0, (GLsizei)vportSize.x, (GLsizei)vportSize.y);
	}

	void Free() {
		//Free all resources
		glDeleteTextures(1, &frameBufferTexture);
		glDeleteFramebuffers(1, &frameBuffer);

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}
