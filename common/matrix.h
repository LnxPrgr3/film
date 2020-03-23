#ifndef FILM_COMMON_MATRIX_H
#define FILM_COMMON_MATRIX_H

class matrix {
private:
	const float _data[3][3];

public:
	class initializer_row {
	private:
		const float _data[3];

	public:
		constexpr initializer_row(const float d0, const float d1, const float d2)
		    : _data{d0, d1, d2} {}

		constexpr float operator[](size_t x) const { return _data[x]; }
	};

	class row {
	private:
		const float *const _data;

		constexpr row(const float *const data) : _data(data) {}

	public:
		constexpr float operator[](size_t x) const { return _data[x]; }

		friend class matrix;
	};

	constexpr matrix(const initializer_row r0, const initializer_row r1, const initializer_row r2)
	    : _data{{r0[0], r0[1], r0[2]}, {r1[0], r1[1], r1[2]}, {r2[0], r2[1], r2[2]}} {}

	constexpr matrix(const float data[3][3])
	    : _data{{data[0][0], data[0][1], data[0][2]},
	            {data[1][0], data[1][1], data[1][2]},
	            {data[2][0], data[2][1], data[2][2]}} {}

	constexpr row operator[](size_t x) const { return _data[x]; }
};

#endif
