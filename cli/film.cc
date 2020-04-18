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
	float print_contrast;

	options() : title("film"), gamma(1.5f), fog(1.f / 512.f), print_contrast(-1) {}
};

static options parse_options(int argc, char *argv[]) {
	options result;
	int ch;
	while ((ch = getopt(argc, argv, "t:g:f:p:")) != -1) {
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
		case 'p':
			result.print_contrast = strtod(optarg, NULL);
			break;
		}
	}
	return result;
}

int main(int argc, char *argv[]) {
	const options options = parse_options(argc, argv);

	if (options.print_contrast > 0) {
		float stage_gamma = sqrt(options.gamma);
		const film film(CIERGB_colorspace, stage_gamma, options.fog);
		rgb white = film(film({1.f, 1.f, 1.f}));
		float white_level =
		    (gamma_2_2_transfer.decode(white.r) + gamma_2_2_transfer.decode(white.g) +
		     gamma_2_2_transfer.decode(white.b)) /
		    3;
		const class film print(CIERGB_colorspace, stage_gamma,
		                       white_level / options.print_contrast);

		lut_write(cout, options.title, 64, [&](rgb x) { return print(film(x)); });
	} else {
		const film film(CIERGB_colorspace, options.gamma, options.fog);
		lut_write(cout, options.title, 64, film);
	}
}
