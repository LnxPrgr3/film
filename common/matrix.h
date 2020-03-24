#ifndef FILM_COMMON_MATRIX_H
#define FILM_COMMON_MATRIX_H
#include <cstddef>

class vector {
private:
	const float _data[3];

public:
	constexpr vector(float d0, float d1, float d2) : _data{d0, d1, d2} {}

	constexpr float operator[](size_t x) const { return _data[x]; }
};

class matrix {
private:
	const float _data[3][3];

public:
	class initializer_row {
	private:
		const float _data[3];

	public:
		constexpr initializer_row(const float d0, const float d1, const float d2)
		    : _data{d0, d1, d2} {}

		constexpr float operator[](size_t x) const { return _data[x]; }
	};

	class row {
	private:
		const float *const _data;

		constexpr row(const float *const data) : _data(data) {}

	public:
		constexpr float operator[](size_t x) const { return _data[x]; }

		friend class matrix;
	};

	constexpr matrix(const initializer_row r0, const initializer_row r1, const initializer_row r2)
	    : _data{{r0[0], r0[1], r0[2]}, {r1[0], r1[1], r1[2]}, {r2[0], r2[1], r2[2]}} {}

	constexpr matrix(const float data[3][3])
	    : _data{{data[0][0], data[0][1], data[0][2]},
	            {data[1][0], data[1][1], data[1][2]},
	            {data[2][0], data[2][1], data[2][2]}} {}

	constexpr row operator[](size_t x) const { return _data[x]; }
};

constexpr matrix invert(const matrix &src) {
	const float inverse_0_0 = src[1][1] * src[2][2] - src[1][2] * src[2][1];
	const float inverse_0_1 = -(src[1][0] * src[2][2] - src[1][2] * src[2][0]);
	const float inverse_0_2 = src[1][0] * src[2][1] - src[1][1] * src[2][0];

	const float inverse_1_0 = -(src[0][1] * src[2][2] - src[0][2] * src[2][1]);
	const float inverse_1_1 = src[0][0] * src[2][2] - src[0][2] * src[2][0];
	const float inverse_1_2 = -(src[0][0] * src[2][1] - src[0][1] * src[2][0]);

	const float inverse_2_0 = src[0][1] * src[1][2] - src[0][2] * src[1][1];
	const float inverse_2_1 = -(src[0][0] * src[1][2] - src[0][2] * src[1][0]);
	const float inverse_2_2 = src[0][0] * src[1][1] - src[0][1] * src[1][0];

	const float determinant =
	    src[0][0] * inverse_0_0 + src[0][1] * inverse_0_1 + src[0][2] * inverse_0_2;

	return matrix{
	    {inverse_0_0 / determinant, inverse_1_0 / determinant, inverse_2_0 / determinant},
	    {inverse_0_1 / determinant, inverse_1_1 / determinant, inverse_2_1 / determinant},
	    {inverse_0_2 / determinant, inverse_1_2 / determinant, inverse_2_2 / determinant}};
}

constexpr vector operator*(const matrix &x, const vector &y) {
	return vector(x[0][0] * y[0] + x[0][1] * y[1] + x[0][2] * y[2],
	              x[1][0] * y[0] + x[1][1] * y[1] + x[1][2] * y[2],
	              x[1][0] * y[0] + x[1][1] * y[1] + x[1][2] * y[2]);
}

constexpr vector operator*(const vector &x, const matrix &y) { return y * x; }

constexpr matrix operator*(const matrix &x, const matrix &y) {
	return matrix{{x[0][0] * y[0][0] + x[0][1] * y[1][0] + x[0][2] * y[2][0],
	               x[0][0] * y[0][1] + x[0][1] * y[1][1] + x[0][2] * y[2][1],
	               x[0][0] * y[0][2] + x[0][1] * y[1][2] + x[0][2] * y[2][2]},
	              {x[1][0] * y[0][0] + x[1][1] * y[1][0] + x[1][2] * y[2][0],
	               x[1][0] * y[0][1] + x[1][1] * y[1][1] + x[1][2] * y[2][1],
	               x[1][0] * y[0][2] + x[1][1] * y[1][2] + x[1][2] * y[2][2]},
	              {x[2][0] * y[0][0] + x[2][1] * y[1][0] + x[2][2] * y[2][0],
	               x[2][0] * y[0][1] + x[2][1] * y[1][1] + x[2][2] * y[2][1],
	               x[2][0] * y[0][2] + x[2][1] * y[1][2] + x[2][2] * y[2][2]}};
}

#endif
