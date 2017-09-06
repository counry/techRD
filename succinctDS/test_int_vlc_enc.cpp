//! Program for debugging. Load vector v of type int_vector<> from
// file argv[1]. Then the program outputs for each integer i,
// which is read from stdin, the value v[i].
#include <sdsl/int_vector.hpp>
#include <sdsl/vectors.hpp>
#include <iostream>
#include <string>
#include "time_util.hpp"
#include <unistd.h>
#include <thread>
#include <future>
#include <sdsl/bit_vectors.hpp>
#include <ctime>


using namespace std;
using namespace sdsl;

const long ITERM_NUM_10W =   100000L;
const long ITERM_NUM_100W =  1000000L;
const long ITERM_NUM_1000W = 10000000L;
const long ITERM_NUM_1Y =    100000000L;
const long ITERM_NUM_3Y =    300000000L;
const int TIME_NUM = 1;


int test_int_vector(long iterm_num)
{
    int_vector<> v;
    v.resize(iterm_num);
    std::cout << "uninit v[0]=" << v[0] << std::endl;
    std::cout << "uninit v[100]=" << v[100] << std::endl;
    for (long i = 0L; i < v.size(); i++) {
        v[i] = i*10L;
    }
    std::cout << "int vector iterm number:" << iterm_num << std::endl;
    std::cout << "sizeof(vector<long>) = " << (iterm_num*8)/1024/1024 << "M" << std::endl;
    std::cout << "sizeof(int_vector<>) = " << size_in_mega_bytes(v) << "M" << std::endl;

    util::bit_compress(v);
    std::cout << "sizeof(int_bitcompress_vector<>) = " << size_in_mega_bytes(v) << "M" << std::endl;

    enc_vector<> ev(v);
    std::cout << "sizeof(enc_vector<>) = " << size_in_mega_bytes(ev) << "M" << std::endl;

    vlc_vector<> vv(v);
    std::cout << "sizeof(vlc_vector<>) = " << size_in_mega_bytes(vv) << "M" << std::endl;
    std::cout << std::endl;
    return 0;
}

int main(int argc, char* argv[])
{
    test_int_vector(ITERM_NUM_10W);
}


