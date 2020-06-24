#include "colorspace.h"

const XYZ colorspace::toXYZ(const rgb &rgb) const { return _color_matrix * _transfer.decode(rgb); }

const rgb colorspace::toRGB(const XYZ &XYZ) const {
	return rgb(_transfer.encode(_inverse_matrix * XYZ));
}

constexpr matrix colorspace::cat02;
constexpr xyz colorspace::d50;

static constexpr matrix ciergb_matrix{
    {0.49000, 0.31000, 0.20000}, {0.17697, 0.81240, 0.01063}, {0.00000, 0.01000, 0.99000}};

constexpr const colorspace CIERGB_colorspace(ciergb_matrix, {1 / 3.f, 1 / 3.f}, gamma_2_2_transfer);

constexpr const colorspace CIERGB_linear_colorspace(ciergb_matrix, {1 / 3.f, 1 / 3.f}, linear_transfer);

constexpr const colorspace film_colorspace({1.218566503589899, 0.3584150197062992},
                                           {-0.05122537562604323, 0.9912120200333888},
                                           {-0.2688638782934198, -0.4798893559292432},
                                           {1 / 3.f, 1 / 3.f}, linear_transfer);

constexpr const colorspace ProPhoto_colorspace({0.734699, 0.265301}, {0.159597, 0.840403},
                                               {0.036598, 0.000105}, {0.345704, 0.358540},
                                               prophoto_transfer);

constexpr const colorspace sRGB_colorspace({0.64, 0.33}, {0.30, 0.60}, {0.05, 0.06}, {0.3127, 0.3290},
                                           srgb_transfer);

constexpr const colorspace sRGB_2_2_colorspace({0.64, 0.33}, {0.30, 0.60}, {0.05, 0.06},
                                               {0.3127, 0.3290}, gamma_2_2_transfer);

constexpr const colorspace bt709_colorspace({0.64, 0.33}, {0.30, 0.60}, {0.05, 0.06}, {0.3127, 0.3290},
                                            bt709_transfer);
