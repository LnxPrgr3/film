#include "film.h"
#include "colorspace.h"
#include "lut.h"
#include "matrix.h"
#include "transfer.h"
#include <cmath>
#include <iostream>
#include <unistd.h>
using namespace std;

struct options {
	const char *title;
	float gamma;
	float fog;

	options() : title("film"), gamma(1.5f), fog(1.f / 512.f) {}
};

static options parse_options(int argc, char *argv[]) {
	options result;
	int ch;
	while ((ch = getopt(argc, argv, "t:g:f:")) != -1) {
		switch (ch) {
		case 't':
			result.title = optarg;
			break;
		case 'g':
			result.gamma = strtod(optarg, NULL);
			break;
		case 'f':
			result.fog = 1. / pow(2., strtod(optarg, NULL));
			break;
		}
	}
	return result;
}

int main(int argc, char *argv[]) {
	const options options = parse_options(argc, argv);
	const film film(CIERGB_colorspace, options.gamma, options.fog);

	lut_write(cout, options.title, 64, film);
}
