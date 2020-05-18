#ifndef FILM_COMMON_COLORSPACE_H
#define FILM_COMMON_COLORSPACE_H
#include "matrix.h"
#include "transfer.h"
#include "types.h"

constexpr xy daylight_illuminant(int id) {
	const float temperature = id * 100.f;
	const float x = temperature > 7000.f
	                    ? (0.23704f + 0.24748f * (1000.f / temperature) +
	                       1.9018f * (1000000.f / (temperature * temperature)) -
	                       2.0064f * (1000000000.f / (temperature * temperature * temperature)))
	                    : (0.244063f + 0.09911f * (1000.f / temperature) +
	                       2.9678f * (1000000.f / (temperature * temperature)) -
	                       4.607f * (1000000000.f / (temperature * temperature * temperature)));
	return {x, -3.f * x * x + 2.87f * x - 0.275f};
}

class colorspace {
private:
	const matrix _color_matrix;
	const matrix _inverse_matrix;
	const transfer_function &_transfer;

	static constexpr matrix cat02 = {
	    {0.7328f, 0.4296f, -0.1624f}, {-0.7036f, 1.6975f, 0.0061f}, {0.0030f, 0.0136f, 0.9834f}};
	static constexpr xyz d50 = xyz(daylight_illuminant(50)).normalized();

	static constexpr matrix adapt_matrix(const matrix x, const xyz white) {
		return x * invert(cat02) *
		       matrix{
		           {d50.x / white.x, 0, 0}, {0, d50.y / white.y, 0}, {0, 0, d50.z / white.z}} *
		       cat02;
	};

	static constexpr matrix color_matrix(const xy r, const xy g, const xy b, const xy w) {
		const xyz red = xyz(r).normalized();
		const xyz green = xyz(g).normalized();
		const xyz blue = xyz(b).normalized();
		const xyz white = xyz(w).normalized();
		const vector white_vector(white.x, white.y, white.z);
		const matrix tristimulus_matrix = {
		    {red.x, green.x, blue.x}, {red.y, green.y, blue.y}, {red.z, green.z, blue.z}};
		const vector scalars = invert(tristimulus_matrix) * white_vector;
		const matrix result =
		    tristimulus_matrix *
		    matrix{{scalars[0], 0, 0}, {0, scalars[1], 0}, {0, 0, scalars[2]}};
		return adapt_matrix(result, white);
	}

public:
	constexpr colorspace(const xy r, const xy g, const xy b, const xy w,
	                     const transfer_function &transfer)
	    : _transfer(transfer), _color_matrix(color_matrix(r, g, b, w)),
	      _inverse_matrix(invert(_color_matrix)) {}

	constexpr colorspace(const matrix color_matrix, const xy w, const transfer_function &transfer)
	    : _transfer(transfer), _color_matrix(adapt_matrix(color_matrix, xyz(w).normalized())),
	      _inverse_matrix(invert(_color_matrix)) {}

	const transfer_function &transfer() const { return _transfer; }

	const XYZ toXYZ(const rgb &rgb) const;
	const rgb toRGB(const XYZ &XYZ) const;
};

extern const colorspace CIERGB_colorspace;
extern const colorspace CIERGB_linear_colorspace;
extern const colorspace ProPhoto_colorspace;
extern const colorspace sRGB_colorspace;
extern const colorspace bt709_colorspace;

#endif
