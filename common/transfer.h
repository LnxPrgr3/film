#ifndef FILM_COMMON_TRANSFER_H
#define FILM_COMMON_TRANSFER_H
#include "types.h"

struct transfer_function {
	virtual ~transfer_function();
	virtual float decode(const float) const = 0;
	virtual float encode(const float) const = 0;

	pixel decode(const pixel &x) const {
		return {decode(x.subpixels[0]), decode(x.subpixels[1]), decode(x.subpixels[2])};
	}

	pixel encode(const pixel &x) const {
		return {
		    encode(x.subpixels[0]),
		    encode(x.subpixels[1]),
		    encode(x.subpixels[2]),
		};
	}
};

class gamma : public transfer_function {
private:
	const float _gamma;
	const float _inv_gamma;

public:
	constexpr gamma(float gamma) : _gamma(gamma), _inv_gamma(1 / gamma) {}
	float decode(const float) const;
	float encode(const float) const;
	using transfer_function::decode;
	using transfer_function::encode;
};

class linear_transfer_function : public transfer_function {
public:
	float decode(const float) const;
	float encode(const float) const;
	using transfer_function::decode;
	using transfer_function::encode;
};

class linear_gamma : public transfer_function {
private:
	const float _linear_scale;
	const float _gamma_threshold;
	const float _gamma;
	const float _gamma_scale;
	const float _gamma_offset;
	const float _dec_linear_scale;
	const float _dec_gamma_threshold;
	const float _dec_gamma;
	const float _dec_gamma_scale;

public:
	constexpr linear_gamma(float linear_scale, float gamma_threshold, float gamma,
	                       float gamma_scale, float gamma_offset)
	    : _linear_scale(linear_scale), _gamma_threshold(gamma_threshold), _gamma(gamma),
	      _gamma_scale(gamma_scale), _gamma_offset(gamma_offset),
	      _dec_linear_scale(1 / linear_scale), _dec_gamma_threshold(gamma_threshold / linear_scale),
	      _dec_gamma(1.f / gamma), _dec_gamma_scale(1.f / gamma_scale) {}
	float decode(const float) const;
	float encode(const float) const;
	using transfer_function::decode;
	using transfer_function::encode;
};

extern const class gamma gamma_2_2_transfer;
extern const class gamma gamma_2_4_transfer;
extern const linear_transfer_function linear_transfer;
extern const linear_gamma prophoto_transfer;
extern const linear_gamma srgb_transfer;

#endif
