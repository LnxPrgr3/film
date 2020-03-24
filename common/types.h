#ifndef FILM_COMMON_TYPES_H
#define FILM_COMMON_TYPES_H

struct rgb {
	const float r, g, b;

	constexpr rgb(float r, float g, float b) : r(r), g(g), b(b) {}
};

struct xy {
	const float x, y;

	constexpr xy(float x, float y) : x(x), y(y) {}
};

struct xyz {
	const float x, y, z;

	constexpr xyz(float x, float y, float z) : x(x), y(y), z(z) {}
	constexpr xyz(const xy xy) : x(xy.x), y(xy.y), z(1 - xy.x - xy.y) {}

	constexpr xyz normalized() const { return xyz(x / y, 1, z / y); }
};

struct XYZ {
	const float X, Y, Z;

	constexpr XYZ(float X, float Y, float Z) : X(X), Y(Y), Z(Z) {}
};

#endif
