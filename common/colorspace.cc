#include "colorspace.h"

const XYZ colorspace::toXYZ(const rgb &rgb) const {
	const vector result(_color_matrix * vector(_transfer.decode(rgb.r()), _transfer.decode(rgb.g()),
	                                           _transfer.decode(rgb.b())));
	return {result[0], result[1], result[2]};
}

const rgb colorspace::toRGB(const XYZ &XYZ) const {
	const vector result(_inverse_matrix * vector(XYZ.X(), XYZ.Y(), XYZ.Z()));
	return {_transfer.encode(result[0]), _transfer.encode(result[1]), _transfer.encode(result[2])};
}

static constexpr matrix ciergb_matrix{
    {0.49000, 0.31000, 0.20000}, {0.17697, 0.81240, 0.01063}, {0.00000, 0.01000, 0.99000}};

constexpr const colorspace CIERGB_colorspace(ciergb_matrix, gamma_2_2_transfer);

constexpr const colorspace CIERGB_linear_colorspace(ciergb_matrix, linear_transfer);
