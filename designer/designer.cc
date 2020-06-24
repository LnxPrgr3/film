#include "types.h"
#include <cmath>
#include <iostream>
#include <vector>

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
		_subpixels[3] = 13 * _subpixels[0] * (_subpixels[1] - 4. / 19);
		_subpixels[4] = 13 * _subpixels[0] * (_subpixels[2] - 9. / 19);
	}

	double L() const { return _subpixels[0]; }
	double up() const { return _subpixels[1]; }
	double vp() const { return _subpixels[2]; }
	double u() const { return _subpixels[3]; }
	double v() const { return _subpixels[4]; }

private:
	double _subpixels[5];

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

int sign(double x) { return x >= 0 ? 1 : -1; }

double degrees(double radians) {
	static const double pi2 = 8 * atan(1.);
	return (radians >= 0 ? radians : (pi2 + radians)) * (360 / pi2);
}

double radians(double degrees) {
	static const double pi45 = atan(1.) / 45;
	return degrees * pi45;
}

template <typename F> double find_zero(F &&f, double a, double b) {
	double c = (a + b) / 2;
	double r = f(c);
	while (r != 0 && c != a && c != b) {
		if (sign(r) == sign(f(a)))
			a = c;
		else
			b = c;
		c = (a + b) / 2;
		r = f(c);
	}
	return c;
}

template <typename F> double derivative(F &&f, double x) {
	double h = 0.001;
	return (-f(x + 2 * h) + 8 * f(x + h) - 8 * f(x - h) + f(x - 2 * h)) / (12 * h);
}

class graph {
public:
	void add(double wavelength) { _wavelengths.push_back(wavelength); }
	void distance(int angle, double distance) { _distance[angle] = distance; }
	double distance(int angle) const { return _distance[angle]; }

	template <typename CB>
	void for_each(CB &&cb) const {
		for(double wavelength: _wavelengths) {
			cb(wavelength);
		}
	}

private:
	std::vector<double> _wavelengths;
	double _distance[3600];
};

void sweep_target(graph &target) {
	CIELUV p0{observer_x(432), observer_y(432), observer_z(432)};
	CIELUV p1{observer_x(432.1), observer_y(432.1), observer_z(432.1)};
	double distance =
	    sqrt(square(p1.L() - p0.L()) + square(p1.u() - p0.u()) + square(p1.v() - p0.v()));
	double wavelength = 432;
	do {
		debug << wavelength << '\n';
		target.add(wavelength);
		CIELUV p{observer_x(wavelength), observer_y(wavelength), observer_z(wavelength)};
		wavelength = find_zero(
		    [=](double w) {
			    CIELUV pn{observer_x(w), observer_y(w), observer_z(w)};
			    return sqrt(square(pn.L() - p.L()) + square(pn.u() - p.u()) +
			                square(pn.v() - p.v())) -
			           distance;
		    },
		    wavelength, wavelength + 0.2);
	} while (wavelength <= 643);
	CIELUV p2{observer_x(643), observer_y(643), observer_z(643)};
	double angle_start = degrees(atan2(p0.vp() - 9. / 19, p0.up() - 4. / 19));
	double angle_end = degrees(atan2(p2.vp() - 9. / 19, p2.up() - 4. / 19));
	debug << angle_start << " degrees @ 432nm\n";
	debug << angle_end << " degrees @ 643nm\n";
	wavelength = 432;
	for (int angle = angle_start * 10 + 1; angle > angle_end * 10; --angle) {
		wavelength = find_zero(
		    [=](double w) {
			    CIELUV pn{observer_x(w), observer_y(w), observer_z(w)};
			    return degrees(atan2(pn.vp() - 9. / 19, pn.up() - 4. / 19)) -
			           (double(angle) / 10);
		    },
		    wavelength, wavelength + 1);
		CIELUV p{observer_x(wavelength), observer_y(wavelength), observer_z(wavelength)};
		double distance = sqrt(square(p.up() - 4. / 19) + square(p.vp() - 9. / 19));
		debug << angle << ',' << wavelength << ',' << distance << '\n';
		target.distance(angle, distance);
	}
	double a = (p2.vp() - p0.vp()) / (p2.up() - p0.up());
	double b = p2.vp() - a * p2.up();
	for (int angle = angle_end * 10; angle != int(angle_start * 10 + 1); --angle) {
		if (angle < 0) {
			angle = 3600 + angle;
		}
		double t = radians(double(angle) / 10);
		double c = sin(t) / cos(t);
		double d = 9. / 19 - c * 4. / 19;
		double u = -((d - b) / (c - a));
		double v = a * u + b;
		double distance = sqrt(square(u - 4. / 19) + square(v - 9. / 19));
		debug << angle << ',' << distance << '\n';
		target.distance(angle, distance);
	}
}

static double error(const graph &target, const matrix &M) {
	double error = 0;
	int count = 0;
	CIELUV white{1, 1, 1};
	target.for_each([&](double w) {
		double Xt = observer_x(w);
		double Yt = observer_y(w);
		double Zt = observer_z(w);
		CIELUV luvt{Xt, Yt, Zt};
		double ra = sensor_r(w);
		double ga = sensor_g(w);
		double ba = sensor_b(w);
		vector XYZa = M * vector(ra, ga, ba);
		CIELUV luva{XYZa[0], XYZa[1], XYZa[2]};
		double angle = degrees(atan2(luva.vp() - white.vp(), luva.up() - white.up()));
		double measured_distance =
		    sqrt(square(luva.up() - white.up()) + square(luva.vp() - white.vp()));
		double gamut_distance = target.distance(angle * 10);
		double gamut_error = measured_distance <= gamut_distance
		                         ? 0
		                         : square(1300 * (measured_distance - gamut_distance));
		error += square(luva.L() - luvt.L()) + square(luva.u() - luvt.u()) +
		         square(luva.v() - luvt.v()) + gamut_error;
		++count;
	});
	return error / count;
}

int main(int argc, char *argv[]) {
	graph target;
	sweep_target(target);
	std::cout << "error = " << error(target, matrix{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}) << '\n';
}
