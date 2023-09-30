#include "Transform.h"
#include <gtc/matrix_transform.hpp>

Transform2D::Transform2D()
{
}

void Transform2D::rotate(float degrees)
{
	float rad = degrees * M_PI / 180;
	this->transform = glm::rotate(this->transform, rad, glm::vec3(0, 0, 1));
}

void Transform2D::translate(float x, float y)
{
	this->transform[0][3] += x;
	this->transform[1][3] += y;
}

void Transform2D::translate(glm::vec2 t)
{
	translate(t.x, t.y);
}

void Transform2D::scale(glm::vec2 scale)
{
	this->scale(scale.x, scale.y);
}

void Transform2D::scale(float x, float y)
{
	this->transform = glm::scale(this->transform,glm::vec3(x, y, 1));
}

Transform::Transform():transform(glm::mat4())
{
	type = TRANSFORM;
}

void Transform::translate(glm::vec3 t)
{
	translate(t.x, t.y, t.z); 
}

void Transform::translate(float x, float y, float z)
{
	this->transform[0][3] += x;
	this->transform[1][3] += y;
	this->transform[2][3] += z;

}

void Transform::rotate(float angle, glm::vec3 axis)
{
	this->transform = glm::rotate(this->transform, angle, axis);
}

void Transform::scale(glm::vec3 scale)
{
	this->transform = glm::scale(this->transform, scale);
}

void Transform::scale(float x, float y, float z)
{
	scale(glm::vec3(x, y, z));
}
