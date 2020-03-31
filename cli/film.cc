#include "film.h"
#include "colorspace.h"
#include "lut.h"
#include "matrix.h"
#include "transfer.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {
	const film film(CIERGB_colorspace, 1.5f, 0.00390625f);

	lut_write(cout, "title", 64, film);
}
