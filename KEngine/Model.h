#pragma once
#include "Vertex.h"
#include <string>
class Model
{
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::string pipeline;
	size_t indices_size;
	glm::mat4 transform;

	void newCustomPipeline(std::string shaderID, std::string vertShader = "", std::string fragShader = "");

	Model();
};

