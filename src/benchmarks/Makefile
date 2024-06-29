SRCS := $(shell find ../parsing ../misc ../algorithms -name '*.cpp')
OBJS := $(SRCS:.cpp=.o)
INCLUDES := -I../parsing -I../misc -I../app -I../algorithms

CXXFLAGS = -O3 -DNDEBUG -std=c++23
LDFLAGS = -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread

all: mybenchmark

mybenchmark: mybenchmark.o $(OBJS)
	g++ mybenchmark.o $(OBJS) $(LDFLAGS) -o mybenchmark

%.o: %.cpp
	g++ $(CXXFLAGS) $(INCLUDES) -c $< -o $@

debug: CXXFLAGS += -g
debug: mybenchmark

clean:
	rm -f $(OBJS) mybenchmark.o mybenchmark
