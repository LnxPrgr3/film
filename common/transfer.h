#ifndef FILM_COMMON_TRANSFER_H
#define FILM_COMMON_TRANSFER_H

struct transfer_function {
	virtual ~transfer_function();
	virtual float decode(const float) const = 0;
	virtual float encode(const float) const = 0;
};

class gamma : public transfer_function {
private:
	const float _gamma;
	const float _inv_gamma;

public:
	constexpr gamma(float gamma) : _gamma(gamma), _inv_gamma(1 / gamma) {}
	float decode(const float) const;
	float encode(const float) const;
};

extern const class gamma gamma_2_2_transfer;

#endif
