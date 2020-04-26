#include "colorspace.h"

const XYZ colorspace::toXYZ(const rgb &rgb) const { return _color_matrix * _transfer.decode(rgb); }

const rgb colorspace::toRGB(const XYZ &XYZ) const {
	return rgb(_transfer.encode(_inverse_matrix * XYZ));
}

static constexpr matrix ciergb_matrix{
    {0.49000, 0.31000, 0.20000}, {0.17697, 0.81240, 0.01063}, {0.00000, 0.01000, 0.99000}};

constexpr const colorspace CIERGB_colorspace(ciergb_matrix, gamma_2_2_transfer);

constexpr const colorspace CIERGB_linear_colorspace(ciergb_matrix, linear_transfer);
