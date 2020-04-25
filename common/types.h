#ifndef FILM_COMMON_TYPES_H
#define FILM_COMMON_TYPES_H

struct pixel {
	const float subpixels[3];

	constexpr pixel(const float subpixels[3])
	    : subpixels{subpixels[0], subpixels[1], subpixels[2]} {}
	constexpr pixel(const float a, const float b, const float c) : subpixels{a, b, c} {}
};

struct xy {
	const float x, y;

	constexpr xy(float x, float y) : x(x), y(y) {}
};

struct XYZ : public pixel {
	constexpr XYZ(float X, float Y, float Z) : pixel(X, Y, Z) {}

	constexpr float X() const { return subpixels[0]; }
	constexpr float Y() const { return subpixels[1]; }
	constexpr float Z() const { return subpixels[2]; }

	constexpr xy chromacity() const { return {X() / (X() + Y() + Z()), Y() / (X() + Y() + Z())}; }
};

struct rgb {
	const float r, g, b;

	constexpr rgb(float r, float g, float b) : r(r), g(g), b(b) {}
};

struct xyz {
	const float x, y, z;

	constexpr xyz(float x, float y, float z) : x(x), y(y), z(z) {}
	constexpr xyz(const xy xy) : x(xy.x), y(xy.y), z(1 - xy.x - xy.y) {}

	constexpr xyz normalized() const { return xyz(x / y, 1, z / y); }
};

#endif
