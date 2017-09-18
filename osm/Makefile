
LIB_DIR = ../../lib
INC_DIR = ../../include
#加-pg参数后性能会下降
MY_CXX_FLAGS=-g -Wall -w -O0 -g  -I ./ -DMUDUO_STD_STRING -pthread -DHAVE_NETINET_IN_H -fpermissive -fvisibility=hidden -fPIC -std=c++11 -march=native -DNDEBUG $(CODE_COVER)
MY_CXX_OPT_FLAGS=-ffast-math -funroll-loops -D__extern_always_inline="extern __always_inline"  -msse4.2



CXX_FLAGS=$(MY_CXX_FLAGS) $(MY_CXX_OPT_FLAGS) -I$(INC_DIR) -L$(LIB_DIR)
CCLIB=-fopenmp -Wl,-rpath=../../lib
#CCLIB=-D_GLIBCXX_PARALLEL -Wl,-rpath=../../lib
SOURCES=$(wildcard *.cpp)
EXECS=$(SOURCES:.cpp=.x)

all: $(EXECS)

build-test: $(EXECS)
	        
%.x:%.cpp 
	$(CXX) $(CXX_FLAGS) -o $@ $< $(CCLIB)

clean:
	rm -f $(EXECS)
	rm -rf *.dSYM

