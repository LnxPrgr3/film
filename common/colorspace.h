#ifndef FILM_COMMON_COLORSPACE_H
#define FILM_COMMON_COLORSPACE_H
#include "matrix.h"
#include "transfer.h"
#include "types.h"

class colorspace {
private:
	const matrix _color_matrix;
	const matrix _inverse_matrix;
	const transfer_function &_transfer;

	static constexpr matrix scale(const matrix matrix) { return matrix / matrix[1][0]; }

	static constexpr matrix color_matrix(const xy r, const xy g, const xy b, const xy w) {
		const xyz red(r), green(g), blue(b);
		const xyz white = xyz(w).normalized();
		const vector white_vector(white.x, white.y, white.z);
		const matrix tristimulus_matrix = {
		    {red.x, green.x, blue.x}, {red.y, green.y, blue.y}, {red.z, green.z, blue.z}};
		const vector scalars = invert(tristimulus_matrix) * white_vector;
		const matrix result =
		    tristimulus_matrix *
		    matrix{{scalars[0], 0, 0}, {0, scalars[1], 0}, {0, 0, scalars[2]}};
		return scale(result);
	}

public:
	constexpr colorspace(const xy r, const xy g, const xy b, const xy w,
	                     const transfer_function &transfer)
	    : _transfer(transfer), _color_matrix(color_matrix(r, g, b, w)),
	      _inverse_matrix(invert(_color_matrix)) {}

	constexpr colorspace(const matrix color_matrix, const transfer_function &transfer)
	    : _transfer(transfer), _color_matrix(scale(color_matrix)),
	      _inverse_matrix(invert(_color_matrix)) {}

	const transfer_function &transfer_function() const { return _transfer; }

	const XYZ toXYZ(const rgb &rgb) const;
	const rgb toRGB(const XYZ &XYZ) const;
};

extern const colorspace CIERGB_colorspace;
extern const colorspace CIERGB_linear_colorspace;
extern const colorspace ProPhoto_colorspace;

#endif
