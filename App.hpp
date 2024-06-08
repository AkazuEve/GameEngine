#pragma once

#include <filesystem>
#include <string>

#include "Window.hpp"
#include "Render.hpp"

#include "ResourceManager.hpp"
#include "Profiler.hpp"
#include "UI.hpp"

class App: UIManager::UIElement {
public:
	App() {
		UIManager::InitImGui(window.GetWindowPtr(), &viewportResolusion);
		m_renderer.CreateObject<Renderer::Shader>("Posterizing shader", "Resources/Shaders/Basic");
		m_renderer.CreateObject<Renderer::Camera>("Main", 90.0f, true);

		auto cube1 = m_renderer.CreateObject<Renderer::Model>("Cube1", "Resources/Models/Cube.ply", "Resources/Textures/pop_cat.png");
		cube1->position = glm::vec3(0.0f, 0.0f, -1.5f);

		auto cube = m_renderer.CreateObject<Renderer::Model>("Cube0", "Resources/Models/Cube.ply", "Resources/Textures/pop_cat.png");
		cube->position = glm::vec3(0.0f, 0.0f, 12.0f);
		cube->rotation = glm::vec3(70.0f, 0.0f, 135.0f);
		cube->scale = glm::vec3(3.0f);


		UIManager::RegisterElement(GetUIElementPtr(), "App", true);
	};

	~App() = default;

	App(const App&) = delete;
	App operator=(const App&) = delete;

	void Run();

private:
	unsigned int WIDTH = (unsigned int)(1920 * 0.9);
	unsigned int HEIGHT = (unsigned int)(1080 * 0.9);
	Window window{ WIDTH, HEIGHT };

	Renderer::Renderer m_renderer{ &viewportResolusion };
	Profiler::Profiler m_profiler;

	std::filesystem::path currentFilePath = std::filesystem::current_path();

	std::string modelPath{ " " };
	std::string diffuseTexturePath{ " " };

	ImVec2 viewportResolusion{ (float)WIDTH, (float)HEIGHT };

	virtual void OnUIRender() override {
		if (ImGui::TreeNode("App")) {
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
						DEBUGPRINT("Model path set to" << modelPath);
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
						DEBUGPRINT("Diffuse path set to" << diffuseTexturePath);
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
					DEBUGPRINT("Objec name is NULL");
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
			ImGui::TreePop();
		}
	};
};

void App::Run() {
	while (window.ShouldRunNextFrame()) {
		//Render all geometry with OpenGL
		m_renderer.RenderAll();
		//Run UI
		UIManager::BeginFrame();
		UIManager::RenderUI();
		UIManager::EndFrame();
	}
}