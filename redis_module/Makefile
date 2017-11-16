

CXX = g++
CXXFLAGS = -g -Wall -w -O3 -fpermissive -fPIC -shared -std=c++11 -fPIC -fvisibility=hidden

.SUFFIXES: .cpp .so .xo .o

all: helloworld.so 

.cpp.xo:
	$(CXX) -I. $(CXXFLAGS) -c $< -o $@

helloworld.xo: redismodule.h

helloworld.so: helloworld.xo
	$(CXX) -o $@ $< $(CXXFLAGS) 

clean:
	rm -rf *.xo *.so
