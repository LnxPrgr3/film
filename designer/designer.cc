#include "types.h"
#include <cmath>
#include <iostream>

struct debug {
	template <typename T> debug &operator<<(const T &t) {
		// std::cout << t;
		return *this;
	}
};

static debug debug;

static constexpr float square(float x) { return x * x; }
static constexpr double square(double x) { return x * x; }
static constexpr float cube(float x) { return x * x * x; }

class CIELUV {
public:
	CIELUV(double X, double Y, double Z) {
		_subpixels[0] = Y > cube(6.f / 29) ? 116 * pow(Y, 1.f / 3) - 16 : cube(29.f / 3) * Y;
		_subpixels[1] = up(X, Y, Z);
		_subpixels[2] = vp(X, Y, Z);
	}

	double L() const { return _subpixels[0]; }
	double u() const { return _subpixels[1]; }
	double v() const { return _subpixels[2]; }

private:
	double _subpixels[3];

	static double up(double X, double Y, double Z) { return (4 * X) / (X + 15 * Y + 3 * Z); }
	static double vp(double X, double Y, double Z) { return (9 * Y) / (X + 15 * Y + 3 * Z); }
};

static double observer(double x, double alpha, double mean, double stddev_1, double stddev_2) {
	x *= 10;
	return alpha * exp(square(x - mean) / (-2 * square(x < mean ? stddev_1 : stddev_2)));
}

static double observer_x(double x) {
	return observer(x, 1.056, 5998, 379, 310) + observer(x, 0.362, 4420, 160, 267) +
	       observer(x, -0.065, 5011, 204, 262);
}

static double observer_y(double x) {
	return observer(x, 0.821, 5688, 469, 405) + observer(x, 0.286, 5309, 163, 311);
}

static double observer_z(double x) {
	return observer(x, 1.217, 4370, 118, 360) + observer(x, 0.681, 4590, 260, 138);
}

static const double observer_sum = 426559 * sqrt(4 * atan(1.)) / (625 * pow(2., 7. / 2));

static const double sensor_scale = 3 * observer_sum / 241.8941293481694;

static double sensor(double x, double mean) {
	return sensor_scale * mean * exp(-square((x - mean) / (136. / 3))) / x;
}

static double sensor_r(double x) { return sensor(x, 593); }

static double sensor_g(double x) { return sensor(x, 550); }

static double sensor_b(double x) { return sensor(x, 505); }

class graph_ent {
public:
	graph_ent() : _set(false), _purple(false) {}

	operator bool() const { return _set; }
	operator float() const { return 100 * float(_value) / ((1U << 30) - 1); }
	graph_ent &operator=(float x) {
		_value = round((x / 100) * ((1U << 30) - 1));
		_set = true;
		return *this;
	}
	bool isPurple() const { return _purple; }
	float getWavelength() const { return _wavelength; }
	float getRedness() const { return _redness; }

	graph_ent &setWavelength(float wavelength) {
		_purple = false;
		_wavelength = wavelength;
		return *this;
	}

	graph_ent &setRedness(float redness) {
		_purple = true;
		_redness = redness;
		return *this;
	}

private:
	bool _set : 1;
	bool _purple : 1;
	uint32_t _value : 30;
	union {
		float _wavelength;
		float _redness;
	};
};

class graph {
public:
	graph() : _start_x(-1), _start_y(-1) {}
	int map(float x) { return round(float(x) * _size); }
	float map_back(int x) { return float(x) / _size; }
	int size() const { return _size; }
	graph_ent operator()(int x, int y) const { return _graph[y][x]; }
	graph_ent &operator()(int x, int y) {
		if (_start_x == -1 && _start_y == -1) {
			_start_x = x;
			_start_y = y;
			debug << "Start = " << map_back(x) << ',' << map_back(y) << '\n';
		}
		return _graph[y][x];
	}

	template <typename CB> void for_each(CB &&cb) const {
		int x = _start_x;
		int y = _start_y;
		const graph_ent *ent = &_graph[y][x];
		cb(*ent);
		while (true) {
			int nx = x;
			int ny = y;
			const float owavelength = ent->getWavelength();
			float nwavelength = 700;
			for (int dy = -1; dy <= 1; ++dy) {
				for (int dx = -1; dx <= 1; ++dx) {
					if (dy == 0 && dx == 0)
						continue;
					const graph_ent *nent = &_graph[y + dy][x + dx];
					if (*nent && !nent->isPurple() &&
					    (nent->getWavelength() > owavelength) &&
					    (nent->getWavelength() - owavelength <
					     nwavelength - owavelength)) {
						nx = x + dx;
						ny = y + dy;
						nwavelength = nent->getWavelength();
					}
				}
			}
			if (nx == x && ny == y)
				break;
			x = nx;
			y = ny;
			ent = &_graph[y][x];
			cb(*ent);
		}
	}

private:
	static constexpr int _size = 32000;
	graph_ent _graph[_size][_size];
	int _start_x;
	int _start_y;
};

void sweep_target(graph &target) {
	int x = 0, y = 0, count = 0;
	float value = 0;
	float metadata = 0;
	int size = target.size();
	for (int i = 432 * size; i < 643 * size; ++i) {
		double w = double(i) / size;
		double X = observer_x(w);
		double Y = observer_y(w);
		double Z = observer_z(w);
		CIELUV luv(X, Y, Z);
		int xp = target.map(luv.u());
		int yp = target.map(luv.v());
		if (x == xp && y == yp) {
			value += luv.L();
			metadata += w;
			++count;
		} else {
			if (count) {
				target(x, y).setWavelength(metadata / count) = value / count;
				debug << target.map_back(x) << ',' << target.map_back(y) << ','
				      << float(target(x, y))
				      << ", wavelength = " << target(x, y).getWavelength() << '\n';
			}
			x = xp;
			y = yp;
			value = luv.L();
			metadata = w;
			count = 1;
		}
	}
	if (count) {
		target(x, y).setWavelength(metadata / count) = value / count;
		debug << target.map_back(x) << ',' << target.map_back(y) << ',' << float(target(x, y))
		      << ", wavelength = " << target(x, y).getWavelength() << '\n';
	}
	x = y = count = 0;
	for (int i = 0; i < 100 * size; ++i) {
		double Bp = float(100 * size - i) / (100 * size);
		double Rp = float(i) / (100 * size);
		double X = Bp * observer_x(432) + Rp * observer_x(643);
		double Y = Bp * observer_y(432) + Rp * observer_y(643);
		double Z = Bp * observer_z(432) + Rp * observer_z(643);
		CIELUV luv(X, Y, Z);
		int xp = target.map(luv.u());
		int yp = target.map(luv.v());
		if (x == xp && y == yp) {
			value += luv.L();
			metadata += Rp;
			++count;
		} else {
			if (count && !target(x, y)) {
				target(x, y).setRedness(metadata / count) = value / count;
				debug << target.map_back(x) << ',' << target.map_back(y) << ','
				      << float(target(x, y))
				      << ", redness = " << target(x, y).getRedness() << '\n';
			}
			x = xp;
			y = yp;
			value = luv.L();
			metadata = Rp;
			count = 1;
		}
	}
	if (count) {
		target(x, y).setRedness(metadata / count) = value / count;
		debug << target.map_back(x) << ',' << target.map_back(y) << ',' << float(target(x, y))
		      << ", redness = " << target(x, y).getRedness() << '\n';
	}
}

static graph target;

int main(int argc, char *argv[]) { sweep_target(target); }
