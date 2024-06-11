#pragma once

#include <filesystem>
#include <string>

#include "Profiler.hpp"
#include "Console.hpp"
#include "Window.hpp"
#include "Render.hpp"
#include "UI.hpp"

class App: UIManager::UIElement {
public:
	App() {
		UIManager::InitImGui();

		m_renderer.CreateObject<Renderer::Shader>("Posterizing shader", "Resources/Shaders/Basic");
		m_renderer.CreateObject<Renderer::Camera>("Main", 90.0f, true);
		m_renderer.CreateObject<Renderer::Model>("Monke", "Resources/Models/Monke.ply", "Resources/Textures/planks.png");

		UIManager::RegisterElement(this, "App", true);
	};

	~App() = default;

	App(const App&) = delete;
	App operator=(const App&) = delete;

	void Run();

private:
	unsigned int WIDTH = (unsigned int)(1920 * 0.9);
	unsigned int HEIGHT = (unsigned int)(1080 * 0.9);
	Window window{ WIDTH, HEIGHT };

	Renderer::Renderer m_renderer;
	Profiler::Profiler m_profiler;

	std::filesystem::path currentFilePath = std::filesystem::current_path();

	std::string modelPath{ " " };
	std::string diffuseTexturePath{ " " };

	ImVec2 viewportResolusion{ (float)WIDTH, (float)HEIGHT };

	virtual void OnUIRender() override {
		static char name[32];
		ImGui::InputText("Object name", name, IM_ARRAYSIZE(name));

		if (ImGui::Button("Choose model")) {
			ImGui::OpenPopup("model_path");
		}

		ImGui::Text(std::format("Model path: \n{}", modelPath).c_str());

		if (ImGui::BeginPopup("model_path")) {
			for (const auto& entry : std::filesystem::directory_iterator(currentFilePath.generic_string() + "/Resources/Models/")) {
				if (ImGui::Button(entry.path().generic_string().c_str())) {
					modelPath = entry.path().generic_string();
					Console::SendLine("Model path set to", modelPath, CONSOLE_MESSAGE_DEBUG);
				}
			}
			ImGui::EndPopup();
		}

		if (ImGui::Button("Choose diffuse texture")) {
			ImGui::OpenPopup("diffuse_path");
		}

		ImGui::Text(std::format("Diffuse path: \n{}", diffuseTexturePath).c_str());

		if (ImGui::BeginPopup("diffuse_path")) {
			for (const auto& entry : std::filesystem::directory_iterator(currentFilePath.generic_string() + "/Resources/Textures/")) {
				if (ImGui::Button(entry.path().generic_string().c_str())) {
					diffuseTexturePath = entry.path().generic_string();
					Console::SendLine("Diffuse path set to", diffuseTexturePath, CONSOLE_MESSAGE_DEBUG);
				}
			}
			ImGui::EndPopup();
		}

		if (ImGui::Button("Create new")) {
			if (name[0] == NULL) {
				ImGui::OpenPopup("Object has no name");
				// Always center this window when appearing
				ImVec2 center = ImGui::GetMainViewport()->GetCenter();
				ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
				Console::SendLine("Objec name is NULL", CONSOLE_MESSAGE_ERROR);
			}
			else {
				m_renderer.CreateObject<Renderer::Model>(name, modelPath, diffuseTexturePath);
			}
		}

		if (ImGui::BeginPopupModal("Object has no name", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Please enter object name");
			ImGui::InputText("Object name", name, IM_ARRAYSIZE(name));

			ImGui::Separator();
			if (name[0] != NULL) {
				if (ImGui::Button("OK", ImVec2(120, 0))) {
					ImGui::CloseCurrentPopup();
					m_renderer.CreateObject<Renderer::Model>(name, modelPath, diffuseTexturePath);
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			}
			else {
				if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			}
			ImGui::EndPopup();
		}
	};
};

void App::Run() {
	while (window.ShouldRunNextFrame()) {
		//Render all geometry with OpenGL
		m_renderer.RenderFrame();

		//Run UI
		UIManager::RenderUI();
	}
}