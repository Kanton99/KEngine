#pragma once
#include <cmath>
#include <vector>

class Vector2 {
public:
	float x;
	float y;
public:
	Vector2();
	Vector2(float x, float y);

	//vector operations
	Vector2 operator+(Vector2& other);
	Vector2 operator-(Vector2& other);
};

class Matrix3x3 {
//variables:
public:
	float a1, a2, a3, b1, b2, b3, c1, c2, c3;

public:
	Matrix3x3();
	Matrix3x3(float a1, float a2, float a3, float b1, float b2, float b3, float c1, float c2, float c3);

	Matrix3x3 operator+(Matrix3x3& other);
	Matrix3x3 operator*(Matrix3x3& other);
};