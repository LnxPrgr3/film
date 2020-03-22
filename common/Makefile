.PHONY: all clean
OBJECTS = lut.o

all: lut_common.a

lut_common.a: ${OBJECTS}
	ar -r -cs $@ $?


clean:
	rm -f ${OBJECTS} lut_common.a

lut.o: lut.cc lut.h types.h
