override CXXFLAGS += -I../common

.PHONY: all clean

all: film

clean:
	rm -f film.o film

film: film.o ../common/film_common.a
	${CXX} ${CXXFLAGS} -o $@ $^

film.o: film.cc ../common/lut.h ../common/transfer.h ../common/types.h ../common/matrix.h ../common/colorspace.h ../common/film.h
