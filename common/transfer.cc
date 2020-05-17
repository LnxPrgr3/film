#include "transfer.h"
#include <cmath>

transfer_function::~transfer_function() {}

float gamma::decode(const float input) const {
	return input < 0 ? 0 : input > 1 ? 1 : pow(input, _gamma);
}

float gamma::encode(const float input) const {
	return input < 0 ? 0 : input > 1 ? 1 : pow(input, _inv_gamma);
}

float linear_transfer_function::decode(const float input) const { return input; }
float linear_transfer_function::encode(const float input) const { return input; }

float linear_gamma::decode(const float input) const {
	return input < 0 ? 0
	                 : input < _dec_gamma_threshold
	                       ? _dec_linear_scale * input
	                       : input < 1 ? pow(input + _gamma_offset, _dec_gamma) / _gamma_scale : 1;
}

float linear_gamma::encode(const float input) const {
	return input < 0 ? 0
	                 : input < _gamma_threshold
	                       ? _linear_scale * input
	                       : input < 1 ? _gamma_scale * pow(input, _gamma) - _gamma_offset : 1;
}

const class gamma gamma_2_2_transfer(2.2);
const linear_transfer_function linear_transfer;
const linear_gamma prophoto_transfer(16, 1.f / 512, 1 / 1.8f, 1, 0);
const linear_gamma srgb_transfer(12.92f, 0.0031308f, 1 / 2.4f, 1.055f, 0.055f);
const linear_gamma bt709_transfer(4.5f, 0.0189f, 0.45f, 1.099f, 0.099f);
