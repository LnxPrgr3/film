#include "film.h"
#include "colorspace.h"
#include "lut.h"
#include "matrix.h"
#include "transfer.h"
#include <cmath>
#include <cstring>
#include <getopt.h>
#include <iostream>
#include <unistd.h>
using namespace std;

struct version_info {
	short major;
	short minor;
	short patch;
	bool beta;
};

ostream &operator<<(ostream &str, const version_info &version) {
	str << version.major << '.' << version.minor << '.' << version.patch;
	if (version.beta)
		str << "-beta";
	return str;
}

static const version_info version = {2, 0, 0, true};

struct colorspace_entry {
	const char *name;
	const colorspace *entry;
};

static const colorspace_entry colorspaces[] = {{"ciergb", &CIERGB_colorspace},
                                               {"prophoto", &ProPhoto_colorspace},
                                               {"srgb", &sRGB_colorspace},
                                               {"bt709", &bt709_colorspace}};

static const colorspace *find_colorspace(const char *name) {
	const int name_len = strlen(name);
	char name_lower[name_len + 1];
	for (int i = 0; i < name_len; ++i) {
		name_lower[i] = tolower(name[i]);
	}
	name_lower[name_len] = 0;

	for (int i = 0; i < sizeof(colorspaces) / sizeof(colorspaces[0]); ++i) {
		if (!strcmp(colorspaces[i].name, name_lower)) {
			return colorspaces[i].entry;
		}
	}

	return NULL;
}

struct options {
	const char *title;
	float gamma;
	float correct_color_for_gamma;
	float fog;
	float print_contrast;
	const colorspace *io_colorspace;

	options()
	    : title("film"), gamma(1.473f), correct_color_for_gamma(1.473f),
	      fog((0.75f / 255.f) / 12.92f), print_contrast(-1), io_colorspace(&CIERGB_colorspace) {}
};

static void show_help(const char *name) {
	const char *options_text =
	    "Options:\n  title: the title of the generated LUT\n  gamma: the 'gamma', or contrast, of "
	    "the response curve\n  neutral color gamma: the gamma setting at which color saturation is "
	    "affected\n    as little as possible (for medium tones)\n  fog: the darkest possible "
	    "output, in f stops below full output\n  print contrast: cascades a second curve so that "
	    "the output contrast ratio is\n    print contrast:1\n  colorspace: the output LUT will "
	    "expect the input image to be in this\n    colorspace, and the output image will also be "
	    "in this colorspace\n\n";
	char spaces[strlen(name) + 9];
	for (int i = 0; i < sizeof(spaces) - 1; ++i) {
		spaces[i] = ' ';
	}
	spaces[sizeof(spaces) - 1] = '\0';
	cerr << "Usage: " << name << " [--title | -t title]\n"
	     << spaces << "[--gamma | -g gamma]\n"
	     << spaces << "[--neutral-color-gamma | -c neutral-color-gamma]\n"
	     << spaces << "[--fog | -f fog]\n"
	     << spaces << "[--print-contrast | -p print contrast]\n"
	     << spaces << "[--colorspace | -s colorspace]\n"
	     << spaces << "[--version | -v]\n"
	     << spaces << "[--help | -h]\n\n"
	     << options_text << "Available color spaces:\n";
	for (int i = 0; i < sizeof(colorspaces) / sizeof(colorspaces[0]); ++i) {
		cerr << "  " << colorspaces[i].name << '\n';
	}
	cerr << "\nProgram writes the generated LUT as a CUBE file to standard output.\n";
	exit(-1);
}

static double parse_float_optarg(const char *name) {
	char *end;
	double result = strtod(optarg, &end);
	if (*end != '\0') {
		cerr << "Not a valid number: " << optarg << "\n\n";
		show_help(name);
	}
	return result;
}

static options parse_options(int argc, char *argv[]) {
	options result;
	int ch;
	static const option long_opts[] = {{"title", required_argument, NULL, 't'},
	                                   {"gamma", required_argument, NULL, 'g'},
	                                   {"neutral-color-gamma", required_argument, NULL, 'c'},
	                                   {"fog", required_argument, NULL, 'f'},
	                                   {"print-contrast", required_argument, NULL, 'p'},
	                                   {"colorspace", required_argument, NULL, 's'},
	                                   {"version", no_argument, NULL, 'v'},
	                                   {"help", no_argument, NULL, 'h'},
	                                   {0}};
	while ((ch = getopt_long(argc, argv, "t:g:c:f:p:s:hv", long_opts, NULL)) != -1) {
		switch (ch) {
		case 't':
			result.title = optarg;
			break;
		case 'g':
			result.gamma = parse_float_optarg(argv[0]);
			break;
		case 'c':
			result.correct_color_for_gamma = parse_float_optarg(argv[0]);
			break;
		case 'f':
			result.fog = 1. / pow(2., parse_float_optarg(argv[0]));
			break;
		case 'p':
			result.print_contrast = parse_float_optarg(argv[0]);
			break;
		case 's':
			result.io_colorspace = find_colorspace(optarg);
			if (result.io_colorspace == NULL) {
				cerr << "Unrecognized colorspace: " << optarg << "\n\n";
				show_help(argv[0]);
			}
			break;
		case 'v':
			cerr << "film version " << version << "\n";
			exit(-1);
			break;
		case 'h':
		default:
			show_help(argv[0]);
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
		const film film(*options.io_colorspace, stage_gamma, stage_correct_color_for_gamma,
		                options.fog);
		pixel white = options.io_colorspace->transfer().decode(film(film({1.f, 1.f, 1.f})));
		float white_level = (white.subpixels[0] + white.subpixels[1] + white.subpixels[2]) / 3;
		const class film print(*options.io_colorspace, stage_gamma,
		                       stage_correct_color_for_gamma,
		                       white_level / options.print_contrast);

		lut_write(cout, options.title, 64, [&](rgb x) { return print(film(x)); });
	} else {
		const film film(*options.io_colorspace, options.gamma, options.correct_color_for_gamma,
		                options.fog);
		lut_write(cout, options.title, 64, film);
	}
}
