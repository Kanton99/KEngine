#include "Transform.h"

Transform2D::Transform2D()
{
}

void Transform2D::rotate(float degrees)
{
	Matrix4 rotationMat;
	float theta = degrees * (M_PI / 180);
	rotationMat.data[0] = cos(theta);
	rotationMat.data[1] = -sin(theta);
	rotationMat.data[4] = sin(theta);
	rotationMat.data[5] = cos(theta);

	this->transform = rotationMat * this->transform;
}

void Transform2D::translate(float x, float y)
{
	transform.data[3] += x;
	transform.data[7] += y;
}

void Transform2D::translate(Vector2 t)
{
	transform.data[2] += t.x;
	transform.data[5] += t.y;
}

Transform::Transform():transform(Matrix4())
{
	type = TRANSFORM;
	this->transform.data[0] = 1;
	this->transform.data[5] = 1;
	this->transform.data[10] = 1;
	this->transform.data[15] = 1;
}

void Transform::translate(Vector3 t)
{
	translate(t.x, t.y, t.z); 
}

void Transform::translate(float x, float y, float z)
{
	this->transform.data[3] += x;
	this->transform.data[7] += y;
	this->transform.data[11] += z;

}
