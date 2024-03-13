#include "Model.h"
unsigned int Model::free_id = 0;
void Model::newCustomPipeline(std::string shaderID, std::string vertShader, std::string fragShader)
{
	return;
}

Model::Model() :
	transform(glm::mat4(1.f)),
	indices_size(0)
{
	id = free_id++;
}