#include "lut.h"
#include "matrix.h"
#include "transfer.h"
#include <cmath>
#include <iostream>
#include <xmmintrin.h>
using namespace std;

template <typename V> V gamma(V x) {
	const auto q = ((V)4) * x * x;
	return (q - ((V)1)) / q;
}

template <typename V> V f(V x) {
	const V o = ((V)6) / 29;
	return x > o * o * o ? cbrt(x) : x / (3 * o * o) + ((V)4) / 29;
}

template <typename V> V L(V x, V max) { return ((V)116) * f(x / max) - ((V)16); }

template <typename V> V response(V x, V g, V fog) {
	const V one = 1;
	return one - pow(one - 0.5, 2 * x * g + 2 * fog);
}

float clip(float x) {
	if (x < 0)
		x = 0;
	else if (x > 1)
		x = 1;
	return x;
}

template <typename T> void matrix_invert(T dst[3][3], const matrix src) {
	T inverse[3][3];
	inverse[0][0] = src[1][1] * src[2][2] - src[1][2] * src[2][1];
	inverse[0][1] = -(src[1][0] * src[2][2] - src[1][2] * src[2][0]);
	inverse[0][2] = src[1][0] * src[2][1] - src[1][1] * src[2][0];

	inverse[1][0] = -(src[0][1] * src[2][2] - src[0][2] * src[2][1]);
	inverse[1][1] = src[0][0] * src[2][2] - src[0][2] * src[2][0];
	inverse[1][2] = -(src[0][0] * src[2][1] - src[0][1] * src[2][0]);

	inverse[2][0] = src[0][1] * src[1][2] - src[0][2] * src[1][1];
	inverse[2][1] = -(src[0][0] * src[1][2] - src[0][2] * src[1][0]);
	inverse[2][2] = src[0][0] * src[1][1] - src[0][1] * src[1][0];

	T determinant =
	    src[0][0] * inverse[0][0] + src[0][1] * inverse[0][1] + src[0][2] * inverse[0][2];

	dst[0][0] = inverse[0][0] / determinant;
	dst[0][1] = inverse[1][0] / determinant;
	dst[0][2] = inverse[2][0] / determinant;

	dst[1][0] = inverse[0][1] / determinant;
	dst[1][1] = inverse[1][1] / determinant;
	dst[1][2] = inverse[2][1] / determinant;

	dst[2][0] = inverse[0][2] / determinant;
	dst[2][1] = inverse[1][2] / determinant;
	dst[2][2] = inverse[2][2] / determinant;
}

template <typename T> class colorspace {
private:
	const matrix _matrix;
	const matrix _inverse;

public:
	constexpr colorspace(matrix matrix) : _matrix(matrix), _inverse(invert(matrix)) {}

	void toXYZ(T *dst, const T *src) const {
		dst[0] = src[0] * _matrix[0][0] + src[1] * _matrix[0][1] + src[2] * _matrix[0][2];
		dst[1] = src[0] * _matrix[1][0] + src[1] * _matrix[1][1] + src[2] * _matrix[1][2];
		dst[2] = src[0] * _matrix[2][0] + src[1] * _matrix[2][1] + src[2] * _matrix[2][2];
	}

	void toRGB(T *dst, const T *src) const {
		dst[0] = src[0] * _inverse[0][0] + src[1] * _inverse[0][1] + src[2] * _inverse[0][2];
		dst[1] = src[0] * _inverse[1][0] + src[1] * _inverse[1][1] + src[2] * _inverse[1][2];
		dst[2] = src[0] * _inverse[2][0] + src[1] * _inverse[2][1] + src[2] * _inverse[2][2];
	}
};

template <typename T> class film_model {
private:
	static constexpr matrix _src_matrix{{0.49 / 0.17697, 0.31 / 0.17697, 0.2 / 0.17697},
	                                    {1, 0.8124 / 0.17697, 0.01063 / 0.17697},
	                                    {0, 0.01 / 0.17697, 0.99 / 0.17697}};
	const T _gamma;
	const T _fog;
	const T _saturation;
	const colorspace<T> _src_colorspace;
	const colorspace<T> _dst_colorspace;

	static T cast(T x) { return x; }

	T saturation(T gamma, T fog) const {
		const T max = response(0.18);
		const T min = response(0.09);
		const T saturation = (max - min) / max;
		return 0.5 / saturation;
	}

	colorspace<T> output_colorspace() const {
		T rgb[3];
		T xyz[3];
		T high = response(0.18);
		T low = response(0);

		rgb[0] = high;
		rgb[1] = low;
		rgb[2] = low;
		_src_colorspace.toXYZ(xyz, rgb);
		const T x_r = xyz[0] / (xyz[0] + xyz[1] + xyz[2]);
		const T y_r = xyz[1] / (xyz[0] + xyz[1] + xyz[2]);
		const T z_r = 1 - x_r - y_r;

		rgb[0] = low;
		rgb[1] = high;
		rgb[2] = low;
		_src_colorspace.toXYZ(xyz, rgb);
		const T x_g = xyz[0] / (xyz[0] + xyz[1] + xyz[2]);
		const T y_g = xyz[1] / (xyz[0] + xyz[1] + xyz[2]);
		const T z_g = 1 - x_g - y_g;

		rgb[0] = low;
		rgb[1] = low;
		rgb[2] = high;
		_src_colorspace.toXYZ(xyz, rgb);
		const T x_b = xyz[0] / (xyz[0] + xyz[1] + xyz[2]);
		const T y_b = xyz[1] / (xyz[0] + xyz[1] + xyz[2]);
		const T z_b = 1 - x_b - y_b;

		T matrix[3][3] = {{x_r, x_g, x_b}, {y_r, y_g, y_b}, {z_r, z_g, z_b}};
		T inverse[3][3];
		matrix_invert(inverse, matrix);

		const T a_r = inverse[0][0] + inverse[0][1] + inverse[0][2];
		const T a_g = inverse[1][0] + inverse[1][1] + inverse[1][2];
		const T a_b = inverse[2][0] + inverse[2][1] + inverse[2][2];

		matrix[0][0] *= a_r;
		matrix[0][1] *= a_g;
		matrix[0][2] *= a_b;

		matrix[1][0] *= a_r;
		matrix[1][1] *= a_g;
		matrix[1][2] *= a_b;

		matrix[2][0] *= a_r;
		matrix[2][1] *= a_g;
		matrix[2][2] *= a_b;

		matrix[0][0] /= matrix[1][0];
		matrix[0][1] /= matrix[1][0];
		matrix[0][2] /= matrix[1][0];
		matrix[1][1] /= matrix[1][0];
		matrix[1][2] /= matrix[1][0];
		matrix[2][0] /= matrix[1][0];
		matrix[2][1] /= matrix[1][0];
		matrix[2][2] /= matrix[1][0];
		matrix[1][0] /= matrix[1][0];

		return colorspace<T>(matrix);
	}

	T response(T x) const { return 1 - pow(cast(0.5), 2 * x * _gamma + 2 * _fog); }

public:
	film_model(T gamma, T fog)
	    : _gamma(gamma), _fog(fog), _saturation(saturation(gamma, fog)),
	      _src_colorspace(_src_matrix), _dst_colorspace(output_colorspace()) {}

	void operator()(T *dst, const T *src) const {
		T xyz[3];
		dst[0] = response(src[0]);
		dst[1] = response(src[1]);
		dst[2] = response(src[2]);
		_src_colorspace.toXYZ(xyz, dst);
		_dst_colorspace.toRGB(dst, xyz);
		dst[0] = clip(dst[0]);
		dst[1] = clip(dst[1]);
		dst[2] = clip(dst[2]);
	}
};

int main(int argc, char *argv[]) {
	const int num_values = 64;
	const film_model<float> film(1.5, 0.00390625);
	const class gamma transfer(2.2f);
	_MM_SET_EXCEPTION_MASK(_MM_GET_EXCEPTION_MASK() & ~_MM_MASK_INVALID);

	lut_write(cout, "title", 64, [&](rgb in) -> rgb {
		const float input[3] = {transfer.decode(in.r), transfer.decode(in.g),
		                        transfer.decode(in.b)};
		float output[3];
		film(output, input);
		return rgb(transfer.encode(output[0]), transfer.encode(output[1]),
		           transfer.encode(output[2]));
	});
}
