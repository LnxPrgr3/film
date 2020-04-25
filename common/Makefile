.PHONY: all clean
OBJECTS = lut.o transfer.o colorspace.o film.o

all: film_common.a

film_common.a: ${OBJECTS}
	ar -r -cs $@ $?


clean:
	rm -f ${OBJECTS} film_common.a

lut.o: lut.cc lut.h types.h
transfer.o: transfer.cc transfer.h types.h
colorspace.o: colorspace.cc colorspace.h matrix.h transfer.h types.h
film.o: film.cc film.h colorspace.h matrix.h transfer.h types.h
