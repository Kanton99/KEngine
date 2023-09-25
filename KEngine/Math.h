#pragma once
#include <cmath>
#include <iostream>
#include <format>
#include <exception>

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
	float values[3][3] = { {1,0,0},{0,1,0},{0,0,1} };

public:
	Matrix3x3();
	Matrix3x3(float a1, float a2, float a3, float b1, float b2, float b3, float c1, float c2, float c3);

	Matrix3x3 operator+(Matrix3x3& other);
	Matrix3x3 operator*(Matrix3x3& other);
	Matrix3x3 operator*(float& other);
	Matrix3x3 operator/(float& other);

	float deteminant();
	Matrix3x3 inverse();
	std::string toString();
};