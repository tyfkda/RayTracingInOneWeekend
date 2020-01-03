SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)

CFLAGS=-std=c++11

.PHONY:	all
all:	a.out

.PHONY:	clean
clean:
	rm -rf *.o a.out image.ppm

.PHONY:	run
run:	a.out
	./a.out > image.ppm

a.out:	$(OBJS)
	$(CXX) -o $@ $(CFLAGS) $^

%.o:	%.cpp
	$(CXX) -O2 -c -o $@ $(CFLAGS) $^
