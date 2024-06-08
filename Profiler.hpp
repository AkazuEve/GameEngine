#pragma once

#include <vector>

#include "Dependecies/imgui/imgui.h"

#include "UI.hpp"

namespace Profiler {
	class Profiler: UIManager::UIElement {
	public:
		Profiler() {
			m_samples.reserve(12000);
			for (unsigned int i{ 0 }; i < m_samples.capacity(); i++) {
				m_samples.push_back(100.0f);
			}

			UIManager::RegisterElement(GetUIElementPtr(), "Profiler", false);
		}

		~Profiler() {

		}

	private:
		std::vector<float> m_samples;
		unsigned int m_lastSample = 0;

		virtual void OnUIRender() override {
			static ImGuiIO& io = ImGui::GetIO(); (void)io;
			static float ms;
			static unsigned int sampleCount;

			if (ImGui::TreeNode("Framerate Metrics")) {
				ms = 1000.0f / io.Framerate;
				sampleCount = (int)(1000 / ms);

				ImGui::Text("Dear ImGui %s", IMGUI_VERSION);
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", ms, io.Framerate);
				ImGui::Text("%d vertices, %d indices (%d triangles)", io.MetricsRenderVertices, io.MetricsRenderIndices, io.MetricsRenderIndices / 3);

				if (m_lastSample < sampleCount) {
					m_samples[m_lastSample] = ms;
					m_lastSample++;
				}
				else {
					m_lastSample = 0;
				}

				ImGui::PlotHistogram("Frame Times", m_samples.data(), sampleCount, 0, NULL, 0.0f, 100.0f, ImVec2(0, 80.0f));
				ImGui::TreePop();
			}
		}
	};
}