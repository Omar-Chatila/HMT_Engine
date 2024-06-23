SRCS := $(shell find ../parsing ../misc ../algorithms -name '*.cpp')
INCLUDES := -I../parsing -I../misc -I../app -I../algorithms

CXXFLAGS = -O3 -std=c++23

all: mybenchmark

mybenchmark: mybenchmark.cpp $(SRCS)
	g++ mybenchmark.cpp $(SRCS) $(INCLUDES) $(CXXFLAGS) -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -o mybenchmark

debug: CXXFLAGS += -g
debug: mybenchmark