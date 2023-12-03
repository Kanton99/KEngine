#pragma once
#include "Vertex.h"
#include <string>
class Model
{
private:
	static unsigned int free_id;
public:
	int id;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::string pipeline;
	size_t indices_size;
	glm::mat4 transform;

public:
	void newCustomPipeline(std::string shaderID, std::string vertShader = "", std::string fragShader = "");

	Model();
};

