.PHONY: all clean
OBJECTS = lut.o transfer.o

all: film_common.a

film_common.a: ${OBJECTS}
	ar -r -cs $@ $?


clean:
	rm -f ${OBJECTS} film_common.a

lut.o: lut.cc lut.h types.h
transfer.o: transfer.cc transfer.h
