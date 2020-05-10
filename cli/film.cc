#include "film.h"
#include "colorspace.h"
#include "lut.h"
#include "matrix.h"
#include "transfer.h"
#include <cmath>
#include <iostream>
#include <unistd.h>
using namespace std;

static const colorspace *find_colorspace(const char *name) {
	const int name_len = strlen(name);
	char name_lower[name_len + 1];
	for (int i = 0; i < name_len; ++i) {
		name_lower[i] = tolower(name[i]);
	}
	name_lower[name_len] = 0;

	if (!strcmp("ciergb", name_lower)) {
		return &CIERGB_colorspace;
	}

	if (!strcmp("prophoto", name_lower)) {
		return &ProPhoto_colorspace;
	}

	if (!strcmp("srgb", name_lower)) {
		return &sRGB_colorspace;
	}

	cout << "# Warning: invalid colorspace " << name << " requested; using CIERGB\n";
	return &CIERGB_colorspace;
}

struct options {
	const char *title;
	float gamma;
	float correct_color_for_gamma;
	float fog;
	float print_contrast;
	const colorspace *colorspace;

	options()
	    : title("film"), gamma(1.473f), correct_color_for_gamma(1.473f),
	      fog((0.75f / 255.f) / 12.92f), print_contrast(-1), colorspace(&CIERGB_colorspace) {}
};

static options parse_options(int argc, char *argv[]) {
	options result;
	int ch;
	while ((ch = getopt(argc, argv, "t:g:c:f:p:s:")) != -1) {
		switch (ch) {
		case 't':
			result.title = optarg;
			break;
		case 'g':
			result.gamma = strtod(optarg, NULL);
			break;
		case 'c':
			result.correct_color_for_gamma = strtod(optarg, NULL);
			break;
		case 'f':
			result.fog = 1. / pow(2., strtod(optarg, NULL));
			break;
		case 'p':
			result.print_contrast = strtod(optarg, NULL);
			break;
		case 's':
			result.colorspace = find_colorspace(optarg);
			break;
		}
	}
	return result;
}

int main(int argc, char *argv[]) {
	const options options = parse_options(argc, argv);

	if (options.print_contrast > 0) {
		float stage_gamma = sqrt(options.gamma);
		float stage_correct_color_for_gamma = sqrt(options.correct_color_for_gamma);
		const film film(*options.colorspace, stage_gamma, stage_correct_color_for_gamma,
		                options.fog);
		pixel white =
		    options.colorspace->transfer_function().decode(film(film({1.f, 1.f, 1.f})));
		float white_level = (white.subpixels[0] + white.subpixels[1] + white.subpixels[2]) / 3;
		const class film print(*options.colorspace, stage_gamma, stage_correct_color_for_gamma,
		                       white_level / options.print_contrast);

		lut_write(cout, options.title, 64, [&](rgb x) { return print(film(x)); });
	} else {
		const film film(*options.colorspace, options.gamma, options.correct_color_for_gamma,
		                options.fog);
		lut_write(cout, options.title, 64, film);
	}
}
