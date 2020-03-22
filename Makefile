CFLAGS=-O3 -pipe
CXXFLAGS=-std=c++11 ${CFLAGS}

.PHONY: all clean

all clean:
	make $@ -C common CFLAGS="${CFLAGS}" CXXFLAGS="${CXXFLAGS}"
	make $@ -C cli CFLAGS="${CFLAGS}" CXXFLAGS="${CXXFLAGS}"
