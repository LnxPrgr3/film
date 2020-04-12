#include "film.h"
#include <cmath>

static float mapped_fog(float x) { return log(1.f / (1.f - x)) / (2.f * log(2.f)); }

static float mapped_gamma(float gamma, float fog) {
	return gamma / ((log(2.f) * pow(2.f, -2.f * fog - 1.f / 5.f)) /
	                (5.f * (1.f - pow(2.f, -2.f * fog - 1.f / 5.f))));
}

colorspace film::working_colorspace() const {
	float high = response(0.18);
	float low = response(0);

	const xy red = CIERGB_linear_colorspace.toXYZ(rgb(high, low, low)).chromacity();
	const xy green = CIERGB_linear_colorspace.toXYZ(rgb(low, high, low)).chromacity();
	const xy blue = CIERGB_linear_colorspace.toXYZ(rgb(low, low, high)).chromacity();

	return colorspace(red, green, blue, {1 / 3.f, 1 / 3.f}, linear_transfer);
}

float film::response(float x) const { return 1 - pow(0.5f, 2 * x * _gamma + 2 * _fog); }

film::film(const colorspace &colorspace, float gamma, float fog)
    : _fog(mapped_fog(fog)), _gamma(mapped_gamma(gamma, _fog)), _source_colorspace(colorspace),
      _working_colorspace(working_colorspace()) {}

rgb film::operator()(rgb x) const {
	rgb working = CIERGB_linear_colorspace.toRGB(_source_colorspace.toXYZ(x));
	return _source_colorspace.toRGB(
	    CIERGB_linear_colorspace.toXYZ(_working_colorspace.toRGB(CIERGB_linear_colorspace.toXYZ(
	        {response(working.r), response(working.g), response(working.b)}))));
}
