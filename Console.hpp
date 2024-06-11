#pragma once

#include <vector>
#include <string>
#include <format>

enum CONSOLE_MESSAGE_TAG {
	CONSOLE_MESSAGE_NOTIFICATION,
	CONSOLE_MESSAGE_DEBUG,
	CONSOLE_MESSAGE_ERROR,
	CONSOLE_MESSAGE_SUCCESS,
};

struct ConsoleMessage {
	std::string message;
	CONSOLE_MESSAGE_TAG messageTag;
};

namespace Console {
	bool isEnabled = true;
	const char* name = "Console";

	static std::vector<ConsoleMessage> m_consoleBufferData;
	static char inputBuffer[256];

	void ConsoleUI() {
		ImGui::Begin("Console", &isEnabled);
		ImGui::InputText("##", inputBuffer, IM_ARRAYSIZE(inputBuffer)); ImGui::SameLine();

		if (ImGui::Button("Execute")) { 
			m_consoleBufferData.push_back(ConsoleMessage(inputBuffer, CONSOLE_MESSAGE_DEBUG));
			for (unsigned int i{ 0 }; i < 256; i++) {
				inputBuffer[i] = NULL;
			}
		} ImGui::SameLine();

		if (ImGui::Button("Clear")) {
			m_consoleBufferData.erase(m_consoleBufferData.begin(), m_consoleBufferData.end());
		}

		ImGui::BeginChild("Output", ImGui::GetContentRegionAvail(), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);

		for (unsigned int i{ 0 }; i < m_consoleBufferData.size(); i++) {
			switch (m_consoleBufferData[i].messageTag) {
			case CONSOLE_MESSAGE_NOTIFICATION:
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), m_consoleBufferData[i].message.c_str());
				break;
			case CONSOLE_MESSAGE_DEBUG:
				ImGui::TextColored(ImVec4(0.3f, 0.3f, 0.3f, 1.0f), m_consoleBufferData[i].message.c_str());
				break;
			case CONSOLE_MESSAGE_ERROR:
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), m_consoleBufferData[i].message.c_str());
				break;
			case CONSOLE_MESSAGE_SUCCESS:
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), m_consoleBufferData[i].message.c_str());
				break;
			}
		}
		ImGui::EndChild();

		ImGui::End();
	}

	void SendLine(std::string var, CONSOLE_MESSAGE_TAG tag) {
		m_consoleBufferData.push_back(ConsoleMessage(var, tag));
	}
	
	void SendLine(std::string var, std::string var2, CONSOLE_MESSAGE_TAG tag) {
		m_consoleBufferData.push_back(ConsoleMessage(var + var2, tag));
	}
	
	void SendLine(std::string var, std::string var2, std::string var3, CONSOLE_MESSAGE_TAG tag) {
		m_consoleBufferData.push_back(ConsoleMessage(var + var2 + var3, tag));
	}
}