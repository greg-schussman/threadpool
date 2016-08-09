.SUFFIXES:
.PHONY: default clean
.SILENT: clean

SOURCES := $(sort $(wildcard ls *.cc))
OBJS := $(SOURCES:%.cc=%.o)
LIBS = libgls.a

%.o : %.cc
	g++ -fPIC -c $<

default : $(LIBS)


libgls.a: $(OBJS)
	ar rs $@ $(OBJS)

clean:
	-rm -f *.o *.a

