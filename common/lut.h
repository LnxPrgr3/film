#ifndef FILM_COMMON_LUT_H
#define FILM_COMMON_LUT_H
#include "types.h"
#include <functional>
#include <ostream>

void lut_write(std::ostream &stream, const char *const title, const int size,
               const std::function<rgb(rgb)> filter);

#endif
