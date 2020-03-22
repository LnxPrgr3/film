#include "transfer.h"
#include <cmath>

transfer_function::~transfer_function() {}

gamma::gamma(const float gamma) : _gamma(gamma), _inv_gamma(1 / gamma) {}

float gamma::decode(const float input) const { return pow(input, _gamma); }

float gamma::encode(const float input) const { return pow(input, _inv_gamma); }
