#include "colorspace.h"

const XYZ colorspace::toXYZ(const rgb &rgb) const {
	const vector result(_color_matrix * vector(_transfer.decode(rgb.r), _transfer.decode(rgb.g),
	                                           _transfer.decode(rgb.b)));
	return {result[0], result[1], result[2]};
}

const rgb colorspace::toRGB(const XYZ &XYZ) const {
	const vector result(_inverse_matrix * vector(XYZ.X, XYZ.Y, XYZ.Z));
	return {_transfer.encode(result[0]), _transfer.encode(result[1]), _transfer.encode(result[2])};
}

constexpr const colorspace CIERGB_colorspace(xy(0.7347, 0.2653), xy(0.2738, 0.7174), xy(0.1666, 0.0089),
                                   xy(1.f / 3, 1.f / 3), gamma_2_2_transfer);

constexpr const colorspace CIERGB_linear_colorspace(xy(0.7347, 0.2653), xy(0.2738, 0.7174), xy(0.1666, 0.0089),
                                          xy(1.f / 3, 1.f / 3), linear_transfer);
