#include "colorspace.h"

const XYZ colorspace::toXYZ(const rgb &rgb) const { return _color_matrix * _transfer.decode(rgb); }

const rgb colorspace::toRGB(const XYZ &XYZ) const {
	return rgb(_transfer.encode(_inverse_matrix * XYZ));
}

static constexpr matrix ciergb_matrix{
    {0.49000, 0.31000, 0.20000}, {0.17697, 0.81240, 0.01063}, {0.00000, 0.01000, 0.99000}};

constexpr const colorspace CIERGB_colorspace(ciergb_matrix, gamma_2_2_transfer);

constexpr const colorspace CIERGB_linear_colorspace(ciergb_matrix, linear_transfer);

constexpr const colorspace ProPhoto_colorspace({0.734699, 0.265301}, {0.159597, 0.840403},
                                               {0.036598, 0.000105}, {0.345704, 0.358540},
                                               prophoto_transfer);

constexpr const colorspace sRGB_colorspace({0.64, 0.33}, {0.30, 0.60}, {0.05, 0.06}, {0.3127, 0.3290},
                                           srgb_transfer);
