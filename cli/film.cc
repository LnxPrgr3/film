#include "film.h"
#include "colorspace.h"
#include "lut.h"
#include "matrix.h"
#include "transfer.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {
	const film film(CIERGB_colorspace, 1.5f, 1.f / 512.f);

	lut_write(cout, "title", 64, film);
}
