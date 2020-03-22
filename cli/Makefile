.PHONY: all clean

all: film

clean:
	rm -f film.o film

film: film.o
	${CXX} ${CXXFLAGS} -o $@ $^

film.o: film.cc
