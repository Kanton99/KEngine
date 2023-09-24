#include "Math.h"

Vector2::Vector2():
	x(0),
	y(0)
{
}

Vector2::Vector2(float x, float y):
	x(x),
	y(y)
{
}

Vector2 Vector2::operator+(Vector2& other)
{
	return Vector2(this->x+other.x,this->y+other.y);
}

Matrix3x3::Matrix3x3():
	a1(1),a2(0),a3(0),
	b1(0),b2(1),b3(0),
	c1(0),c2(0),c3(1)
{
}

Matrix3x3::Matrix3x3(float a1, float a2, float a3, float b1, float b2, float b3, float c1, float c2, float c3) :
	a1(a1), a2(a2), a3(a3),
	b1(b1), b2(b2), b3(b3),
	c1(c1), c2(c2), c3(c3)
{
}

Matrix3x3 Matrix3x3::operator+(Matrix3x3& other)
{
	return Matrix3x3(this->a1 + other.a1, this->a2 + other.a2, this->a3 + other.a3, 
					 this->b1 + other.b1, this->b2 + other.b2, this->c3 + other.c3,
					 this->c1 + other.c1, this->c2 + other.c2, this->c3 + other.c3);
}

Matrix3x3 Matrix3x3::operator*(Matrix3x3& other)
{
	float m1[3][3] = { {this->a1,this->a2,this->a3},{this->b1,this->b2,this->b3},{this->c1,this->c2,this->c3} };
	float m2[3][3] = { {other.a1,other.a2,other.a3},{other.b1,other.b2,other.b3},{other.c1,other.c2,other.c3} };

	float m3[3][3];

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			float val = 0;
			for (int k = 0; k < 3; k++) {
				
			}
		}
	}
}

