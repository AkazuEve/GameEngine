#pragma once

//Standar lib
#include <vector>

//Math
#include <glm/glm.hpp>

//OpeGL variables
#include <glad/glad.h>

struct Vertex {
	glm::vec3 pos;
	glm::vec3 nor;
	glm::vec2 tex;
};

struct MeshData {
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
};