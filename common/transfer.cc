#include "transfer.h"
#include <cmath>

transfer_function::~transfer_function() {}

float gamma::decode(const float input) const { return pow(input, _gamma); }

float gamma::encode(const float input) const { return pow(input, _inv_gamma); }

const class gamma gamma_2_2_transfer(2.2);
