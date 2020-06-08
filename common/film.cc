#include "film.h"
#include <cmath>

static const float bungholes_number = 0.640089356302193f;

static float lux(float density) { return 10.f / 9.f - pow(10.f, 1.f - density) / 9.f; }

colorspace film::working_colorspace() const {
	float subtract = 0.18f - pow(0.18f, _correct_color_for_gamma);
	float high = 1.f + _fog;
	float low = _fog - subtract / 2.f;

	const xy red = film_colorspace.toXYZ(rgb(high, low, low)).chromacity();
	const xy green = film_colorspace.toXYZ(rgb(low, high, low)).chromacity();
	const xy blue = film_colorspace.toXYZ(rgb(low, low, high)).chromacity();

	return colorspace(red, green, blue, {1 / 3.f, 1 / 3.f}, linear_transfer);
}

float film::response(float x) const {
	return x > 0.f ? lux(1.f - pow(0.5f, pow(_offset * x, _gamma))) + _fog : _fog;
}

film::film(const colorspace &colorspace, float gamma, float correct_color_for_gamma, float fog)
    : _fog(fog), _gamma(gamma), _correct_color_for_gamma(correct_color_for_gamma),
      _offset(bungholes_number * pow(9.f, 1.f - gamma) * pow(50.f, (gamma - 1.f))),
      _source_colorspace(colorspace), _working_colorspace(working_colorspace()) {}

rgb film::operator()(rgb x) const {
	rgb working = film_colorspace.toRGB(_source_colorspace.toXYZ(x));
	return _source_colorspace.toRGB(
	    film_colorspace.toXYZ(_working_colorspace.toRGB(film_colorspace.toXYZ(
	        {response(working.r()), response(working.g()), response(working.b())}))));
}
