#include "Transform.h"

Transform2D::Transform2D():
	transform(Matrix3())
{
}

void Transform2D::rotate(float degrees)
{
	Matrix3 rotationMat;
	float theta = degrees * (M_PI / 180);
	rotationMat.data[0] = cos(theta);

	this->transform = rotationMat * this->transform;
}

void Transform2D::translate(float x, float y)
{
	transform.data[2] += x;
	transform.data[5] += y;
}

void Transform2D::translate(Vector2 t)
{
	transform.data[2] += t.x;
	transform.data[5] += t.y;
}
