#include "colorspace.h"
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

template <typename T> class film_model {
private:
	const T _gamma;
	const T _fog;
	const T _saturation;
	const colorspace _src_colorspace;
	const colorspace _dst_colorspace;

	static T cast(T x) { return x; }

	T saturation(T gamma, T fog) const {
		const T max = response(0.18);
		const T min = response(0.09);
		const T saturation = (max - min) / max;
		return 0.5 / saturation;
	}

	colorspace output_colorspace() const {
		T high = response(0.18);
		T low = response(0);

		const xy red = _src_colorspace.toXYZ(rgb(high, low, low)).chromacity();
		const xy green = _src_colorspace.toXYZ(rgb(low, high, low)).chromacity();
		const xy blue = _src_colorspace.toXYZ(rgb(low, low, high)).chromacity();

		return colorspace(red, green, blue, {1 / 3.f, 1 / 3.f}, linear_transfer);
	}

	T response(T x) const { return 1 - pow(cast(0.5), 2 * x * _gamma + 2 * _fog); }

public:
	film_model(T gamma, T fog)
	    : _gamma(gamma), _fog(fog), _saturation(saturation(gamma, fog)),
	      _src_colorspace(CIERGB_linear_colorspace), _dst_colorspace(output_colorspace()) {}

	void operator()(T *dst, const T *src) const {
		rgb output = _dst_colorspace.toRGB(
		    _src_colorspace.toXYZ({response(src[0]), response(src[1]), response(src[2])}));
		dst[0] = clip(output.r);
		dst[1] = clip(output.g);
		dst[2] = clip(output.b);
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
