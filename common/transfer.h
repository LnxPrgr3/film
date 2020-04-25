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

extern const class gamma gamma_2_2_transfer;
extern const linear_transfer_function linear_transfer;

#endif
