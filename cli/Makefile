CXXFLAGS += -I../common

.PHONY: all clean

all: film

clean:
	rm -f film.o film

film: film.o ../common/lut_common.a
	${CXX} ${CXXFLAGS} -o $@ $^

film.o: film.cc ../common/lut.h
