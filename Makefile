#-----------------------------------------------------------------------------
# To build:
# a) make
#-----------------------------------------------------------------------------

USER_SRCS=$(wildcard tests/*.cpp)

USER_EXE = create_tdb

.PHONY: all depend clean distclean

OBJS = $(USER_SRCS:.cc=.o)

BINDIR = ./bin
CFLAGS = -I./include -I/usr/local/include
CXXFLAGS = $(CFLAGS) -std=c++11 -O2
LIBS = -L/usr/local/lib -ltraildb

all: $(USER_EXE)

$(USER_EXE) : $(OBJS)
		$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS) $(LIBS)

depend: Make-depend

install: 
		cp include/*.h /usr/local/include

Make-depend: $(USER_SRCS)
		$(CXX) $(CXXFLAGS) -MM $^ > $@

clean:
		- rm -f *.o tests/*.o tdbtest.tdb create_tdb Make-depend

