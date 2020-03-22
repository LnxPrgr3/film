#include "transfer.h"
#include <cmath>

transfer_function::~transfer_function() {}

gamma::gamma(float gamma) : _gamma(gamma), _inv_gamma(1 / gamma) {}

float gamma::decode(float input) const { return pow(input, _gamma); }

float gamma::encode(float input) const { return pow(input, _inv_gamma); }
