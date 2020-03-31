#include "colorspace.h"
#include "lut.h"
#include "matrix.h"
#include "transfer.h"
#include <cmath>
#include <iostream>
#include <xmmintrin.h>
using namespace std;

float clip(float x) {
	if (x < 0)
		x = 0;
	else if (x > 1)
		x = 1;
	return x;
}

template <typename T> class film_model {
private:
	const T _gamma;
	const T _fog;
	const colorspace _src_colorspace;
	const colorspace _dst_colorspace;

	static T cast(T x) { return x; }

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
	    : _gamma(gamma), _fog(fog), _src_colorspace(CIERGB_linear_colorspace),
	      _dst_colorspace(output_colorspace()) {}

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
