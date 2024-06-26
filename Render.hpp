#pragma once

#include <format>

#include <stb/stbi.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Dependecies/imgui/imgui.h"

#include "RenderingObjects.hpp"
#include "ResourceManager.hpp"
#include "Console.hpp"
#include "UI.hpp"

namespace Renderer {

	static void* pCurrentShader = nullptr;
	static void* pCurrentCamera = nullptr; 

	class Shader {
	public:
		Shader(std::string name, std::string filePath):name(name), m_filePath(filePath) {
			std::string tmp;
			bool tmpBool; //?

			tmp = LoadShaderFile(m_filePath + ".vert");
			GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, tmp.c_str(), tmpBool);
			tmp = LoadShaderFile(m_filePath + ".frag");
			GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, tmp.c_str() , tmpBool);

			m_ID = glCreateProgram();
			glAttachShader(m_ID, vertexShader);
			glAttachShader(m_ID, fragmentShader);
			glLinkProgram(m_ID);

			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			Console::SendLine("Created shader: " ,m_filePath, CONSOLE_MESSAGE_DEBUG);

			this->Bind();
		}
		~Shader() {
			glDeleteProgram(m_ID);
			Console::SendLine("Freed shader: ", m_filePath, CONSOLE_MESSAGE_DEBUG);
		}

		Shader(const Shader&) = delete;
		Shader operator=(const Shader&) = delete;

		void HotReload() {
			Console::SendLine("Hot reloading shader: ", m_filePath, CONSOLE_MESSAGE_NOTIFICATION);

			std::string tmp;
			bool successFlag = true;

			tmp = LoadShaderFile(m_filePath + ".vert");
			GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, tmp.c_str(), successFlag);
			tmp = LoadShaderFile(m_filePath + ".frag");
			GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, tmp.c_str(), successFlag);

			if (successFlag) {
				glDeleteProgram(m_ID);
				m_ID = glCreateProgram();
				glAttachShader(m_ID, vertexShader);
				glAttachShader(m_ID, fragmentShader);
				glLinkProgram(m_ID);

				glDeleteShader(vertexShader);
				glDeleteShader(fragmentShader);

				glUseProgram(m_ID);
				Console::SendLine("Hot reloading success", CONSOLE_MESSAGE_SUCCESS);
			}
		}

		void Bind() { 
			pCurrentShader = static_cast<void*>(this);
			glUseProgram(m_ID); 
		}

		GLuint GetID() { return m_ID; }

	public:
		std::string name{ " " };

	private:
		GLuint m_ID;
		std::string m_filePath{ " " };

		GLuint CompileShader(GLenum shaderType, const char* shaderSource, bool& successFlag) {
			GLuint shader = glCreateShader(shaderType);
			glShaderSource(shader, 1, &shaderSource, NULL);
			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> errorLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

				// Provide the infolog in whatever manor you deem best.
				Console::SendLine(errorLog.data(), CONSOLE_MESSAGE_ERROR);

				// Exit with failure.
				glDeleteShader(shader); // Don't leak the shader.
				switch (shaderType)
				{
				case GL_VERTEX_SHADER:
					Console::SendLine("Vertex shader compilation failure", CONSOLE_MESSAGE_ERROR);
					successFlag = false;
					break;
				case GL_FRAGMENT_SHADER:
					Console::SendLine("Fragment shader compilation failure", CONSOLE_MESSAGE_ERROR);
					successFlag = false;
					break;
				}
			}
			return shader;
		}
	};

	class Mesh {
	public:
		Mesh() {
			glCreateVertexArrays(1, &m_vertexArray);
			glCreateBuffers(1, &m_vertexBuffer);
			glCreateBuffers(1, &m_indexBuffer);
		}
		~Mesh() {
			glDeleteVertexArrays(1, &m_vertexArray);
			glDeleteBuffers(1, &m_vertexBuffer);
			glDeleteBuffers(1, &m_indexBuffer);

			Console::SendLine("Freed mesh data", CONSOLE_MESSAGE_DEBUG);
		}
		
		Mesh(const Mesh&) = delete;
		Mesh operator=(const Mesh&) = delete;

		void SendModelData(MeshData data) {
			m_indexBufferSize = data.indices.size();

			glBindVertexArray(m_vertexArray);

			glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * data.vertices.size(), data.vertices.data(), GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * data.indices.size(), data.indices.data(), GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(GLfloat)));

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			Console::SendLine("Created mesh data", CONSOLE_MESSAGE_DEBUG);
		}

		void Bind() {
			glBindVertexArray(m_vertexArray);
		}

		inline GLuint GetIndexCount() {
			return (GLuint)m_indexBufferSize;
		}

	private:
		GLuint m_vertexArray;
		GLuint m_vertexBuffer;
		GLuint m_indexBuffer;
		rsize_t m_indexBufferSize = 0;
	};

	class Texture2D {
	public:
		Texture2D() {
			glGenTextures(1, &m_ID);
		}
		~Texture2D() {
			glDeleteTextures(1, &m_ID);
			Console::SendLine("Freed texture: ", m_name, CONSOLE_MESSAGE_DEBUG);
		}

		Texture2D(const Texture2D&) = delete;
		Texture2D operator=(const Texture2D&) = delete;

		void SendTextureData(std::string texturePath) {
			stbi_set_flip_vertically_on_load(true);

			int imgWidth, imgHeight, imgColCh;
			unsigned char* bytes = stbi_load(texturePath.c_str(), &imgWidth, &imgHeight, &imgColCh, 0);

			if (!bytes) {
				throw std::runtime_error("Could not load texture");
			}

			Console::SendLine("Created texture: ", texturePath, CONSOLE_MESSAGE_DEBUG);
			m_name = texturePath;

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_ID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);

			stbi_image_free(bytes);
		}

		void Bind() {
			glBindTexture(GL_TEXTURE_2D, m_ID);
		};

	private:
		GLuint m_ID;

		std::string m_name;
	};

	class Camera {
	public:
		Camera(std::string name, float fov, bool isActive): name(name),  Fov(fov) {
			if (isActive) {
				pCurrentCamera = static_cast<void*>(this);
			}
		};
		~Camera() = default;

		void UpdateMatrix(ImVec2& viewportSize) {
			m_projection = glm::perspective(glm::radians(Fov), (float)viewportSize.x / (float)viewportSize.y, 0.1f, 100.0f);
			m_view = glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, 1.0f), Up);
			glUniformMatrix4fv(glGetUniformLocation(static_cast<Shader*>(pCurrentShader)->GetID(), "cameraMatrix"), 1, GL_FALSE, glm::value_ptr(m_projection * m_view));
		};

	private:
		glm::vec3 Up{ 0.0f, 1.0f, 0.0f };
		glm::mat4 m_projection{ 0.0f };
		glm::mat4 m_view{ 0.0f };
	public:
		float Fov = 0.0f;
		std::string name{ " " };
		glm::vec3 position{ 0.0f, 0.0f, 0.0f };
	};

	class Model {
	public:
		Model(std::string name, std::string meshPath, std::string texturePath): m_name(name) {
			m_mesh.SendModelData(LoadModelFromPLYFile(meshPath));
			m_diffuseTexture.SendTextureData(texturePath);

			UpdateMatrix();
		}

		~Model() = default;

		Model(const Model&) = delete;
		Model operator=(const Model&) = delete;

		void Render() {
			if (m_isRendered) {
				m_mesh.Bind();
				m_diffuseTexture.Bind();

				glUniform1i(glGetUniformLocation(static_cast<Shader*>(pCurrentShader)->GetID(), "diffuseTexture"), 0);

				UpdateMatrix();
				glUniformMatrix4fv(glGetUniformLocation(static_cast<Shader*>(pCurrentShader)->GetID(), "model"), 1, GL_FALSE, glm::value_ptr(m_modelMatrix));
				glDrawElements(GL_TRIANGLES, m_mesh.GetIndexCount(), GL_UNSIGNED_INT, 0);
			}
		}

	private:
		Mesh m_mesh;
		Texture2D m_diffuseTexture;

		glm::mat4 m_modelMatrix{ 1.0f };

		glm::mat4 m_translationMatrix{ 1.0f };
		glm::mat4 m_roatationlationMatrix{ 1.0f };
		glm::mat4 m_scaleMatrix{ 1.0f };

		void UpdateMatrix() {
			static glm::mat4 iMatrix{ 1.0f };

			m_translationMatrix = glm::translate(iMatrix, position);

			m_roatationlationMatrix = glm::rotate(iMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			m_roatationlationMatrix = glm::rotate(m_roatationlationMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			m_roatationlationMatrix = glm::rotate(m_roatationlationMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

			m_scaleMatrix = glm::scale(iMatrix, scale);

			m_modelMatrix = m_translationMatrix * m_roatationlationMatrix * m_scaleMatrix;
		}

	public:
		glm::vec3 position{ 0.0f };
		glm::vec3 rotation{ 0.0f };
		glm::vec3 scale   { 1.0f };

		bool m_isRendered{ true };
		std::string m_name;
	};

	class FrameBuffer {
	public:
		FrameBuffer() {
			glGenFramebuffers(1, &m_frameBuffer);
		}
		~FrameBuffer() {
			glDeleteBuffers(1, &m_frameBuffer);

			for (unsigned int i{ 0 }; i < m_frameBufferAttachments.size(); i++) {
				glDeleteTextures(1, &m_frameBufferAttachments[i].texture);
			}
		}

		void AddAttachment(GLenum attachment, GLenum internalFormat, GLenum format, GLenum type) {
			m_frameBufferAttachments.push_back(Attachmenttexture(attachment, internalFormat, format, type));
			size_t index = m_frameBufferAttachments.size() - 1;

			m_attachments.push_back(attachment);

			glGenTextures(1, &m_frameBufferAttachments[index].texture);

			glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
			glBindTexture(GL_TEXTURE_2D, m_frameBufferAttachments[index].texture);

			glTexImage2D(GL_TEXTURE_2D, 0, m_frameBufferAttachments[index].internalFormat, 100, 100, 0, m_frameBufferAttachments[index].format, m_frameBufferAttachments[index].type, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glFramebufferTexture2D(GL_FRAMEBUFFER, m_frameBufferAttachments[index].attachment, GL_TEXTURE_2D, m_frameBufferAttachments[index].texture, 0);
		}

		void FinalizeBuffer() {
			glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
			glDrawBuffers(m_attachments.size() - 1, m_attachments.data());
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void BindAndSetupForRender(GLsizei width, GLsizei height) {
			glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
			glViewport(0, 0, width, height);

			for (unsigned int i{ 0 }; i < m_frameBufferAttachments.size(); i++) {
				glBindTexture(GL_TEXTURE_2D, m_frameBufferAttachments[i].texture);
				glTexImage2D(GL_TEXTURE_2D, 0, m_frameBufferAttachments[i].internalFormat, width, height, 0, m_frameBufferAttachments[i].format, m_frameBufferAttachments[i].type, nullptr);
			}
			
			glBindTexture(GL_TEXTURE_2D, 0);

			glClear(GL_COLOR_BUFFER_BIT);
			glClear(GL_DEPTH_BUFFER_BIT);
		}

		GLuint* GetTexturePtrByAttachment(GLenum attachment) {
			for (unsigned int i{ 0 }; i < m_frameBufferAttachments.size(); i++) {
				if (m_frameBufferAttachments[i].attachment == attachment) {
					return &m_frameBufferAttachments[i].texture;
				}
			}
		}

	private:
		struct Attachmenttexture {
			Attachmenttexture(GLenum attachment, GLenum internalFormat, GLenum format, GLenum type) {
				this->internalFormat = internalFormat;
				this->format= format;
				this->type = type;
				this->attachment = attachment;
			}
			
			GLenum attachment;
			GLuint texture = 0;
			GLenum internalFormat;
			GLenum format;
			GLenum type;
		};

		GLuint m_frameBuffer;
		std::vector<Attachmenttexture> m_frameBufferAttachments;
		std::vector<GLenum> m_attachments;
	};

	class Renderer: UIManager::UIElement {
	public:
		Renderer() {
			glClearColor(m_clearClolor.x, m_clearClolor.y, m_clearClolor.z, 1.0f);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			glEnable(GL_CULL_FACE);

			glCullFace(GL_FRONT);
			glFrontFace(GL_CW);

			m_gBuffer.AddAttachment(GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_RGB, GL_FLOAT);
			m_gBuffer.AddAttachment(GL_COLOR_ATTACHMENT1, GL_RGBA16F, GL_RGB, GL_FLOAT);
			m_gBuffer.AddAttachment(GL_COLOR_ATTACHMENT2, GL_RGBA,    GL_RGB, GL_UNSIGNED_BYTE);
			m_gBuffer.AddAttachment(GL_DEPTH_ATTACHMENT,GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT);
			m_gBuffer.FinalizeBuffer();

			UIManager::RegisterElement(this, "Renderer", true);
		}

		~Renderer() {
			for (Model* model : m_pModels) {
				delete model;
			}
			for (Camera* camera : m_pCameras) {
				delete camera;
			}
			for (Shader* shader : m_pShaders) {
				delete shader;
			}
		}

		Renderer(const Renderer&) = delete;
		Renderer operator=(const Renderer&) = delete;

		template<typename Shader>
		Shader* CreateObject(std::string name, std::string filePath) {
			Shader* newShader = new Shader(name, filePath);
			m_pShaders.push_back(newShader);
			return newShader;
		}

		template<typename Camera>
		Camera* CreateObject(std::string name, float fov, bool isActive) {
			Camera* newCamera = new Camera(name, fov, isActive);
			m_pCameras.push_back(newCamera);
			return newCamera;
		}

		template<typename Model>
		Model* CreateObject(std::string name, std::string meshPath, std::string texturePath) {
			Model* newModel = new Model(name, meshPath, texturePath);
			m_pModels.push_back(newModel);
			return newModel;
		}

		void RenderFrame() {
			m_gBuffer.BindAndSetupForRender((GLsizei)m_pViewportSize.x, (GLsizei)m_pViewportSize.y);

			static_cast<Camera*>(pCurrentCamera)->UpdateMatrix(m_pViewportSize);

			for (unsigned int i{ 0 }; i < m_pModels.size(); i++) {
				m_pModels[i]->Render();
			}
		}

	private:
		FrameBuffer m_gBuffer;

		std::vector<Model*> m_pModels;
		std::vector<Camera*> m_pCameras;
		std::vector<Shader*> m_pShaders;

		glm::vec3 m_clearClolor{ 0.5f, 0.5f, 0.5f };
		bool m_cullFaceEnable{ true };
		bool m_depthTestEnable{ true };

		std::string m_cullFace{ "Front" };
		std::string m_frontFace{ "Clockwise" };
		std::string m_depthTest{ "Less" };

		ImVec2 m_pViewportSize{ 100.0f ,100.0f };

		virtual void OnUIRender() override {
			ImGui::Text(std::format("Model count: {}", m_pModels.size()).c_str());

			if (ImGui::Checkbox("Face culling", &m_cullFaceEnable)) {
				switch (m_cullFaceEnable) {
				case true:
					glEnable(GL_CULL_FACE);
					break;
				case false:
					glDisable(GL_CULL_FACE);
					break;
				}
			}
			if (ImGui::Button("Culling options")) {
				ImGui::OpenPopup("culling_options");
			}
			ImGui::SameLine();
			ImGui::Text(std::format("Culling: {} {}", m_cullFace, m_frontFace).c_str());
			if (ImGui::BeginPopup("culling_options")) {

				if (ImGui::Button("Cull face front")) {
					glCullFace(GL_FRONT);
					m_cullFace = "Front";
				}
				if (ImGui::Button("Cull face back")) {
					glCullFace(GL_BACK);
					m_cullFace = "Back";
				}
				if (ImGui::Button("Culling clockwise")) {
					glFrontFace(GL_CW);
					m_frontFace = "Clockwise";
				}
				if (ImGui::Button("Culling counterclockwise")) {
					glFrontFace(GL_CCW);
					m_frontFace = "Counterclockwise";
				}
				ImGui::EndPopup();
			}

			if (ImGui::Checkbox("Depth test", &m_depthTestEnable)) {
				switch (m_cullFaceEnable) {
				case true:
					glEnable(GL_DEPTH_TEST);
					break;
				case false:
					glDisable(GL_DEPTH_TEST);
					break;
				}
			}
			if (ImGui::Button("Depth test options")) {
				ImGui::OpenPopup("depth_test_options");
			}
			ImGui::SameLine();
			ImGui::Text(std::format("Depth test func: {}", m_depthTest).c_str());
			if (ImGui::BeginPopup("depth_test_options")) {

				if (ImGui::Button("Always")) {
					m_depthTest = "Always";
					glDepthFunc(GL_ALWAYS);
				}
				if (ImGui::Button("Never")) {
					m_depthTest = "Never";
					glDepthFunc(GL_NEVER);
				}
				if (ImGui::Button("Less")) {
					m_depthTest = "Less";
					glDepthFunc(GL_LESS);
				}
				if (ImGui::Button("Equal")) {
					m_depthTest = "Equal";
					glDepthFunc(GL_EQUAL);
				}
				if (ImGui::Button("Less or Equal")) {
					m_depthTest = "Less or Equal";
					glDepthFunc(GL_LEQUAL);
				}
				if (ImGui::Button("Greater or Equal")) {
					m_depthTest = "Greater or Equal";
					glDepthFunc(GL_GEQUAL);
				}
				if (ImGui::Button("Greater")) {
					m_depthTest = "Greater";
					glDepthFunc(GL_GREATER);
				}
				if (ImGui::Button("Not Equal")) {
					m_depthTest = "Not Equal";
					glDepthFunc(GL_NOTEQUAL);
				}

				ImGui::EndPopup();
			}

			if (ImGui::TreeNode("Cameras")) {
				for (Camera* camera : m_pCameras) {
					if (ImGui::TreeNode(camera->name.c_str())) {
						ImGui::DragFloat3("Position", &camera->position.x, 0.1f, -100.0f, 100.0f);
						ImGui::DragFloat("FOV", &camera->Fov, 0.1f, 10.0f, 180.0f);

						if (ImGui::Button("Use")) {
							pCurrentCamera = static_cast<void*>(camera);
						}
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Shaders")) {
				for (Shader* shader : m_pShaders) {
					if (ImGui::TreeNode(shader->name.c_str())) {
						ImGui::Text(std::format("ID: {}", shader->GetID()).c_str());
						if (ImGui::Button("Hot reload")) {
							shader->HotReload();
						}
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Shader settings")) {
				if (ImGui::ColorPicker3("Clear color", &m_clearClolor.x)) {
					glClearColor(m_clearClolor.x, m_clearClolor.y, m_clearClolor.z, 1.0f);
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Models")) {
				for (unsigned int i{ 0 }; i < m_pModels.size(); i++) {
					static std::string name;

					name = m_pModels[i]->m_name + std::string("##", i);

					if (ImGui::TreeNode(name.c_str())) {
						ImGui::Checkbox("Is rendered", &m_pModels[i]->m_isRendered);

							ImGui::DragFloat3("Position", &m_pModels[i]->position.x, 0.1f, -100.0f, 100.0f);
							ImGui::DragFloat3("Rotation", &m_pModels[i]->rotation.x, 0.1f, -180.0f, 180.0f);
							ImGui::DragFloat3("Scale", &m_pModels[i]->scale.x, 0.1f, -10.0f, 10.0f);

							if (ImGui::Button("Restet Transform")) {
								m_pModels[i]->position = glm::vec3(0.0f);
								m_pModels[i]->rotation = glm::vec3(0.0f);
								m_pModels[i]->scale = glm::vec3(1.0f);
							}
							if (ImGui::Button("Remove")) {
								delete m_pModels[i];
								m_pModels.erase(m_pModels.begin() + i);
							}
							ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}

			UIManager::RenderViewport("Color Buffer", m_gBuffer.GetTexturePtrByAttachment(GL_COLOR_ATTACHMENT0), m_pViewportSize);
			UIManager::RenderViewport("Position Buffer", m_gBuffer.GetTexturePtrByAttachment(GL_COLOR_ATTACHMENT1));
			UIManager::RenderViewport("Normal Buffer", m_gBuffer.GetTexturePtrByAttachment(GL_COLOR_ATTACHMENT2));
			UIManager::RenderViewport("Depth Buffer", m_gBuffer.GetTexturePtrByAttachment(GL_DEPTH_ATTACHMENT));
		}
	};
}