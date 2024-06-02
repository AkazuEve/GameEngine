#pragma once

//Standar lib
#include <streambuf>
#include <exception>
#include <fstream>
#include <sstream>
#include <sstream>
#include <vector>
#include <string>

//Structs to pack data to
#include "RenderingStructs.hpp"

//Math cause numbers hard
#include <glm/glm.hpp>

//Glad so I can use OpenGL specific variables (GLuint)
#include <glad/glad.h>

MeshData LoadModelFromPLYFile(std::string filePath) {
	TIMEDFUNC("Parsing file: " << filePath, 
		std::ifstream file{ filePath };
		if (!file.is_open())
			throw std::exception("Failed to open file");

		MeshData data;
		Vertex veretx;
		unsigned int tmpInt;

		std::string line;
		std::string word;
		std::stringstream stream;

		unsigned int vertexCount = 0;
		unsigned int triangleCount = 0;


		while (std::getline(file, line)) {
			stream.clear();
			stream.str(line);

			stream >> word;

			//Here I read how many vertices and indices the model has
			//For now I don't care for the layout just want it to work
			if (word == "element") {

				stream >> word;

				if (word == "vertex") {
					stream >> vertexCount;
					data.vertices.reserve(vertexCount);
				}

				if (word == "face") {
					stream >> triangleCount;
					data.indices.reserve(3 * triangleCount);
				}
			}

			//End of header time for data parsing
			if (word == "end_header") {
				//This just contains vertex data
				for (unsigned int i{ 0 }; i < vertexCount; i++) {
					std::getline(file, line);
					stream.clear();
					stream.str(line);

					stream >> veretx.pos.x >> veretx.pos.y >> veretx.pos.z;
					stream >> veretx.nor.x >> veretx.nor.y >> veretx.nor.z;
					stream >> veretx.tex.x >> veretx.tex.y;

					data.vertices.emplace_back(veretx);

				}

				//This line will have a int for vectex amount per face then data >:|
				for (unsigned int i{ 0 }; i < triangleCount; i++) {
					std::getline(file, line);
					stream.clear();
					stream.str(line);

					stream >> tmpInt;

					for (unsigned int x{ 0 }; x < 3; x++) {
						stream >> tmpInt;
						data.indices.emplace_back(tmpInt);
					}

				}
			}
		}
	)
/*  
	for (unsigned int i{ 0 }; i < data.vertices.size(); i++) {
		std::cout << i << ": Px:" << data.vertices[i].pos.x << " Py:" << data.vertices[i].pos.y << " Pz:" << data.vertices[i].pos.z <<
			" Nx:" << data.vertices[i].nor.x << " Ny:" << data.vertices[i].nor.y << " Nz:" << data.vertices[i].nor.z <<
			" Tx:" << data.vertices[i].tex.x << " Ty:" << data.vertices[i].tex.y << std::endl;
	}

	for (unsigned int i{ 0 }; i < data.indices.size(); i += 3) {
		std::cout << i / 3 << " :" << data.indices[i] << ", " << data.indices[i + 1] << ", " << data.indices[i + 2] << std::endl;
	}
*/
	return data;
}

std::string LoadShaderFile(std::string file) {
	std::string shaderString;
	std::ifstream inFile(file.c_str(), std::ios::ate);

	//Just open text and dunk into string
	if (inFile.is_open()) {
		shaderString.reserve(inFile.tellg());
		inFile.seekg(0, std::ios::beg);
		shaderString.assign((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
	}

	return shaderString;
}