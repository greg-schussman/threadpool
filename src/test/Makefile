.SUFFIXES:
.PHONY: default clean
.SILENT: clean

SOURCES = $(sort $(wildcard *.cc))

default: tester
	./tester

GLS_INCLUDE = -I ..
GLS_LIB = -L.. -lgls

GTEST_LIB_DIR = ../../util/gtest/lib64
GTEST_INCLUDE = -I ../../util
GTEST_LIB = -L$(GTEST_LIB_DIR) -lgtest -Wl,-rpath,$(GTEST_LIB_DIR)

PTHREAD_LIB = -lpthread

tester:
	g++ -o $@ $(SOURCES) \
	-D_GLIBCXX_USE_CXX11_ABI=0 \
	$(GLS_INCLUDE) $(GTEST_INCLUDE) \
	$(GLS_LIB) $(GTEST_LIB) \
	$(PTHREAD_LIB)

clean:
	-rm -f *.o tester
