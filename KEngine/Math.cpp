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

Matrix3x3::Matrix3x3()
{
}

Matrix3x3::Matrix3x3(float a1, float a2, float a3, float b1, float b2, float b3, float c1, float c2, float c3) :
	values{{a1,a2,a3},{b1,b2,b3},{c1,c2,c3}}
{
}

Matrix3x3 Matrix3x3::operator+(Matrix3x3& other)
{
	return Matrix3x3(this->values[0][0] + other.values[0][0], this->values[0][1] + other.values[0][1], this->values[0][2] + other.values[0][2], 
					 this->values[1][0] + other.values[1][0], this->values[1][1] + other.values[1][1], this->values[2][2] + other.values[2][2],
					 this->values[2][0] + other.values[2][0], this->values[2][1] + other.values[2][1], this->values[2][2] + other.values[2][2]);
}

Matrix3x3 Matrix3x3::operator*(Matrix3x3& other)
{
	float m3[3][3];

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			float val = 0;
			for (int k = 0; k < 3; k++) {
				val += this->values[i][k] * other.values[k][j];
			}
			m3[i][j] = val;
		}
	}

	return Matrix3x3(m3[0][0], m3[0][1], m3[0][2], m3[1][0], m3[1][1], m3[1][2], m3[2][0], m3[2][1], m3[2][2]);
}

Matrix3x3 Matrix3x3::operator*(float& other) {
	return Matrix3x3(other * values[0][0], other * values[0][1], other * values[0][2], other * values[1][0], other * values[1][1], other * values[1][2], other * values[2][0], other * values[2][1], other * values[2][2]);
}

Matrix3x3 Matrix3x3::operator/(float& other) {
	return Matrix3x3(values[0][0] / other, values[0][1] / other,  values[0][2] / other, values[1][0] / other, values[1][1] / other, values[1][2] / other, values[2][0] / other, values[2][1] / other,values[2][2] / other);
}

float Matrix3x3::deteminant() {
	return values[0][0] * (values[1][1] * values[2][2] - values[2][1] * values[1][2]) - values[0][1] * (values[1][0] * values[2][2] - values[2][0] * values[1][2]) + values[0][2] * (values[1][0] * values[2][1] - values[1][1] * values[2][0]);
}

void getCofactor(Matrix3x3 A, Matrix3x3 temp, int p, int q)
{
	int i = 0, j = 0;

	// Looping for each element of the matrix
	for (int row = 0; row < 3; row++) {
		for (int col = 0; col < 3; col++) {
			//  Copying into temporary matrix only those
			//  element which are not in given row and
			//  column
			if (row != p && col != q) {
				temp.values[i][j++] = A.values[row][col];

				// Row is filled, so increase row index and
				// reset col index
				if (j == 2) {
					j = 0;
					i++;
				}
			}
		}
	}
}

Matrix3x3 adjoint(Matrix3x3 A)
{
	Matrix3x3 adj;
	// temp is used to store cofactors of A[][]
	int sign = 1;
	Matrix3x3 temp;

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			// Get cofactor of A[i][j]
			getCofactor(A, temp, i, j);

			// sign of adj[j][i] positive if sum of row
			// and column indexes is even.
			sign = ((i + j) % 2 == 0) ? 1 : -1;

			// Interchanging rows and columns to get the
			// transpose of the cofactor matrix
			adj.values[j][i] = (sign) * (temp.deteminant());
		}
	}

	return adj;
}

Matrix3x3 Matrix3x3::inverse() {
	float det = this->deteminant();
	if (det == 0.f) {
		throw std::exception("Determinant is 0\n");
		exit(1);
	}
	Matrix3x3 adj = adjoint(*this);

	return adj / det;
}


std::string Matrix3x3::toString() {
	std::string res = std::format("{} {} {}\n{} {} {}\n{} {} {}\n", values[0][0], values[0][1], values[0][2], values[1][0], values[1][1], values[1][2], values[2][0], values[2][1], values[2][2]);
	return res;
}
