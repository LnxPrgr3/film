#ifndef FILM_COMMON_TRANSFER_H
#define FILM_COMMON_TRANSFER_H

struct transfer_function {
	virtual ~transfer_function();
	virtual float decode(float) const = 0;
	virtual float encode(float) const = 0;
};

class gamma : public transfer_function {
private:
	const float _gamma;
	const float _inv_gamma;

public:
	gamma(float gamma);
	float decode(float) const;
	float encode(float) const;
};

#endif
