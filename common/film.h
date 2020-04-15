#ifndef FILM_COMMON_FILM_H
#define FILM_COMMON_FILM_H
#include "colorspace.h"

class film {
private:
	float _fog;
	float _gamma;
	float _offset;
	const colorspace &_source_colorspace;
	const colorspace _working_colorspace;

	colorspace working_colorspace() const;
	float response(float x) const;

public:
	film(const colorspace &colorspace, float gamma, float fog);
	rgb operator()(rgb x) const;
};

#endif
