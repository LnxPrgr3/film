#include "film.h"
#include <cmath>

static const float bungholes_number = 0.640089356302193f;

static float lux(float density) { return 10.f / 9.f - pow(10.f, 1.f - density) / 9.f; }

colorspace film::working_colorspace() const {
	float high = response(0.18);
	float low = response(0);

	const xy red = CIERGB_linear_colorspace.toXYZ(rgb(high, low, low)).chromacity();
	const xy green = CIERGB_linear_colorspace.toXYZ(rgb(low, high, low)).chromacity();
	const xy blue = CIERGB_linear_colorspace.toXYZ(rgb(low, low, high)).chromacity();

	return colorspace(red, green, blue, {1 / 3.f, 1 / 3.f}, linear_transfer);
}

float film::response(float x) const {
	return x > 0.f ? lux(1.f - pow(0.5f, pow(_offset * x, _gamma))) + _fog : _fog;
}

film::film(const colorspace &colorspace, float gamma, float fog)
    : _fog(fog), _gamma(gamma),
      _offset(bungholes_number * pow(9.f, 1.f - gamma) * pow(50.f, (gamma - 1.f))),
      _source_colorspace(colorspace), _working_colorspace(working_colorspace()) {}

rgb film::operator()(rgb x) const {
	rgb working = CIERGB_linear_colorspace.toRGB(_source_colorspace.toXYZ(x));
	return _source_colorspace.toRGB(
	    CIERGB_linear_colorspace.toXYZ(_working_colorspace.toRGB(CIERGB_linear_colorspace.toXYZ(
	        {response(working.r), response(working.g), response(working.b)}))));
}
