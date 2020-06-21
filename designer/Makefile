override CXXFLAGS += -I../common

.PHONY: all clean

all: designer

clean:
	rm -f designer.o designer

designer: designer.o
	${CXX} ${CXXFLAGS} -o $@ $^

designer.o: designer.cc ../common/types.h ../common/matrix.h
