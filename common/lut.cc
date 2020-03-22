#include "lut.h"

void lut_write(std::ostream &stream, const char *title, int size, std::function<rgb(rgb)> filter) {
	stream << "TITLE \"" << title << "\"\n\n";
	stream << "LUT_3D_SIZE " << size << "\n\n";
	for (int b = 0; b < size; ++b) {
		const float input_b = (float)b / (size - 1);
		for (int g = 0; g < size; ++g) {
			const float input_g = (float)g / (size - 1);
			for (int r = 0; r < size; ++r) {
				const float input_r = (float)r / (size - 1);
				const rgb output = filter(rgb(input_r, input_g, input_b));
				stream << output.r << ' ' << output.g << ' ' << output.b << '\n';
			}
		}
	}
}
