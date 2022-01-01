CXX = g++

seq-src = $(wildcard img-seq/*.cpp) \
       $(wildcard img-seq/BmpUtils/*.cpp)
par-src = $(wildcard img-par/*.cpp) \
       $(wildcard img-par/BmpUtils/*.cpp)

seq-obj = $(seq-src:.cpp=.o)
par-obj = $(par-src:.cpp=.o)

CXXFLAGS += -fopenmp -O3 -DNDEBUG

CFLAGS = -fopenmp -O3 -DNDEBUG -std=c++17 -Wall -Wextra -Wno-deprecated -Werror -pedantic -pedantic-errors


exec-seq: $(seq-obj)
	$(CXX) $(CFLAGS) $^ -o $@

exec-par: $(par-obj)
	$(CXX) $(CFLAGS) $(PARFLAGS) $^ -o $@

all: exec-par exec-seq

.PHONY: clean
clean:
	rm -f $(par-obj) $(seq-obj) exec-seq exec-par
