#include "Math.h"
#include <sstream>
#include <iomanip>

// Vector2 methods
Vector2::Vector2() : x(0.0f), y(0.0f) {}
Vector2::Vector2(float x, float y) : x(x), y(y) {}

Vector2 Vector2::operator+(const Vector2& other) const {
    return Vector2(x + other.x, y + other.y);
}

Vector2 Vector2::operator-(const Vector2& other) const {
    return Vector2(x - other.x, y - other.y);
}

Vector2 Vector2::operator*(float scalar) const {
    return Vector2(x * scalar, y * scalar);
}

float Vector2::dot(const Vector2& other) const {
    return x * other.x + y * other.y;
}

float Vector2::magnitude() const {
    return std::sqrt(x * x + y * y);
}

Vector2 Vector2::normalize() const {
    float mag = magnitude();
    if (mag != 0) {
        return Vector2(x / mag, y / mag);
    }
    return *this;
}

// Vector3 methods
Vector3::Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

Vector3 Vector3::operator+(const Vector3& other) const {
    return Vector3(x + other.x, y + other.y, z + other.z);
}

Vector3 Vector3::operator-(const Vector3& other) const {
    return Vector3(x - other.x, y - other.y, z - other.z);
}

Vector3 Vector3::operator*(float scalar) const {
    return Vector3(x * scalar, y * scalar, z * scalar);
}

float Vector3::dot(const Vector3& other) const {
    return x * other.x + y * other.y + z * other.z;
}

Vector3 Vector3::cross(const Vector3& other) const {
    return Vector3(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    );
}

float Vector3::magnitude() const {
    return std::sqrt(x * x + y * y + z * z);
}

Vector3 Vector3::normalize() const {
    float mag = magnitude();
    if (mag != 0) {
        return Vector3(x / mag, y / mag, z / mag);
    }
    return *this;
}

// Matrix3 methods
Matrix3::Matrix3() : data(9, 0.0f) {}

Matrix3 Matrix3::operator*(const Matrix3& other) const {
    Matrix3 result;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 3; ++k) {
                result.data[i * 3 + j] += data[i * 3 + k] * other.data[k * 3 + j];
            }
        }
    }
    return result;
}

Matrix3 Matrix3::inverse()
{
    Matrix3 result;

    float det = data[0] * (data[4] * data[8] - data[7] * data[5]) -
        data[1] * (data[3] * data[8] - data[6] * data[5]) +
        data[2] * (data[3] * data[7] - data[6] * data[4]);

    if (det != 0.0f) {
        float invDet = 1.0f / det;

        result.data[0] = (data[4] * data[8] - data[7] * data[5]) * invDet;
        result.data[1] = -(data[1] * data[8] - data[2] * data[7]) * invDet;
        result.data[2] = (data[1] * data[5] - data[2] * data[4]) * invDet;

        result.data[3] = -(data[3] * data[8] - data[5] * data[6]) * invDet;
        result.data[4] = (data[0] * data[8] - data[2] * data[6]) * invDet;
        result.data[5] = -(data[0] * data[5] - data[2] * data[3]) * invDet;

        result.data[6] = (data[3] * data[7] - data[4] * data[6]) * invDet;
        result.data[7] = -(data[0] * data[7] - data[1] * data[6]) * invDet;
        result.data[8] = (data[0] * data[4] - data[1] * data[3]) * invDet;
    }

    return result;
}

std::string Matrix3::toString() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            ss << std::setw(8) << data[i * 3 + j] << " ";
        }
        ss << "\n";
    }
    ss.unsetf(std::ios_base::floatfield);
    return ss.str();
}

// Matrix4 methods
Matrix4::Matrix4() : data(16, 0.0f) {}

Matrix4 Matrix4::operator*(const Matrix4& other) const {
    Matrix4 result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 4; ++k) {
                result.data[i * 4 + j] += data[i * 4 + k] * other.data[k * 4 + j];
            }
        }
    }
    return result;
}

Matrix4 Matrix4::inverse() const {
    Matrix4 result;

    float temp[12];
    float src[16];

    for (int i = 0; i < 4; i++) {
        src[i] = data[i];
        src[i + 4] = data[i + 4];
        src[i + 8] = data[i + 8];
        src[i + 12] = data[i + 12];
    }

    // Calculate pairs for first 8 elements (cofactors)
    temp[0] = src[10] * src[15];
    temp[1] = src[11] * src[14];
    temp[2] = src[9] * src[15];
    temp[3] = src[11] * src[13];
    temp[4] = src[9] * src[14];
    temp[5] = src[10] * src[13];
    temp[6] = src[8] * src[15];
    temp[7] = src[11] * src[12];
    temp[8] = src[8] * src[14];
    temp[9] = src[10] * src[12];
    temp[10] = src[8] * src[13];
    temp[11] = src[9] * src[12];

    // Calculate first 8 elements (cofactors)
    result.data[0] = temp[0] * src[5] + temp[3] * src[6] + temp[4] * src[7];
    result.data[0] -= temp[1] * src[5] + temp[2] * src[6] + temp[5] * src[7];
    result.data[1] = temp[1] * src[4] + temp[6] * src[6] + temp[9] * src[7];
    result.data[1] -= temp[0] * src[4] + temp[7] * src[6] + temp[8] * src[7];
    result.data[2] = temp[2] * src[4] + temp[7] * src[5] + temp[10] * src[7];
    result.data[2] -= temp[3] * src[4] + temp[6] * src[5] + temp[11] * src[7];
    result.data[3] = temp[5] * src[4] + temp[8] * src[5] + temp[11] * src[6];
    result.data[3] -= temp[4] * src[4] + temp[9] * src[5] + temp[10] * src[6];
    result.data[4] = temp[1] * src[1] + temp[2] * src[2] + temp[5] * src[3];
    result.data[4] -= temp[0] * src[1] + temp[3] * src[2] + temp[4] * src[3];
    result.data[5] = temp[0] * src[0] + temp[7] * src[2] + temp[8] * src[3];
    result.data[5] -= temp[1] * src[0] + temp[6] * src[2] + temp[9] * src[3];
    result.data[6] = temp[3] * src[0] + temp[6] * src[1] + temp[11] * src[3];
    result.data[6] -= temp[2] * src[0] + temp[7] * src[1] + temp[10] * src[3];
    result.data[7] = temp[4] * src[0] + temp[9] * src[1] + temp[10] * src[2];
    result.data[7] -= temp[5] * src[0] + temp[8] * src[1] + temp[11] * src[2];

    // Calculate pairs for second 8 elements (cofactors)
    temp[0] = src[2] * src[7];
    temp[1] = src[3] * src[6];
    temp[2] = src[1] * src[7];
    temp[3] = src[3] * src[5];
    temp[4] = src[1] * src[6];
    temp[5] = src[2] * src[5];
    temp[6] = src[0] * src[7];
    temp[7] = src[3] * src[4];
    temp[8] = src[0] * src[6];
    temp[9] = src[2] * src[4];
    temp[10] = src[0] * src[5];
    temp[11] = src[1] * src[4];

    // Calculate second 8 elements (cofactors)
    result.data[8] = temp[0] * src[13] + temp[3] * src[14] + temp[4] * src[15];
    result.data[8] -= temp[1] * src[13] + temp[2] * src[14] + temp[5] * src[15];
    result.data[9] = temp[1] * src[12] + temp[6] * src[14] + temp[9] * src[15];
    result.data[9] -= temp[0] * src[12] + temp[7] * src[14] + temp[8] * src[15];
    result.data[10] = temp[2] * src[12] + temp[7] * src[13] + temp[10] * src[15];
    result.data[10] -= temp[3] * src[12] + temp[6] * src[13] + temp[11] * src[15];
    result.data[11] = temp[5] * src[12] + temp[8] * src[13] + temp[11] * src[14];
    result.data[11] -= temp[4] * src[12] + temp[9] * src[13] + temp[10] * src[14];
    result.data[12] = temp[2] * src[10] + temp[5] * src[11] + temp[1] * src[9];
    result.data[12] -= temp[4] * src[11] + temp[0] * src[9] + temp[3] * src[10];
    result.data[13] = temp[8] * src[11] + temp[0] * src[8] + temp[7] * src[10];
    result.data[13] -= temp[6] * src[10] + temp[9] * src[11] + temp[1] * src[8];
    result.data[14] = temp[6] * src[9] + temp[11] * src[11] + temp[3] * src[8];
    result.data[14] -= temp[10] * src[11] + temp[2] * src[8] + temp[7] * src[9];
    result.data[15] = temp[10] * src[10] + temp[4] * src[8] + temp[9] * src[9];
    result.data[15] -= temp[8] * src[9] + temp[11] * src[10] + temp[5] * src[8];

    float determinant = src[0] * result.data[0] + src[1] * result.data[1] +
        src[2] * result.data[2] + src[3] * result.data[3];

    if (determinant != 0.0f) {
        float invDet = 1.0f / determinant;

        for (int i = 0; i < 16; i++) {
            result.data[i] *= invDet;
        }
    }

    return result;
}