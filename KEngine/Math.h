#ifndef LINEAR_ALGEBRA_H
#define LINEAR_ALGEBRA_H

#include <vector>
#include <cmath>
#include <string>

// Vector class for 2D vectors
class Vector2 {
public:
    float x, y;

    Vector2();
    Vector2(float x, float y);

    Vector2 operator+(const Vector2& other) const;
    Vector2 operator-(const Vector2& other) const;
    Vector2 operator*(float scalar) const;
    float dot(const Vector2& other) const;
    float magnitude() const;
    Vector2 normalize() const;
};

// Vector class for 3D vectors
class Vector3 {
public:
    float x, y, z;

    Vector3();
    Vector3(float x, float y, float z);

    Vector3 operator+(const Vector3& other) const;
    Vector3 operator-(const Vector3& other) const;
    Vector3 operator*(float scalar) const;
    float dot(const Vector3& other) const;
    Vector3 cross(const Vector3& other) const;
    float magnitude() const;
    Vector3 normalize() const;
};

// Matrix class for 3x3 matrices
class Matrix3 {
public:
    std::vector<float> data;

    Matrix3();

    Matrix3 operator*(const Matrix3& other) const;
    Matrix3 inverse();
    std::string toString() const;
};

class Matrix4 {
public:
    std::vector<float> data;

    Matrix4();

    Matrix4 operator*(const Matrix4& other) const;
    Matrix4 inverse() const;
};


#endif // LINEAR_ALGEBRA_H
