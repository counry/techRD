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
#include <sdsl/wavelet_trees.hpp>
#include <ctime>


using namespace std;
using namespace sdsl;

const long ITERM_NUM_10W =   100000L;
const long ITERM_NUM_100W =  1000000L;
const long ITERM_NUM_1000W = 10000000L;
const long ITERM_NUM_1Y =    100000000L;
const long ITERM_NUM_3Y =    300000000L;
const int TIME_NUM = 1;

int test_bit_vector(long iterm_num)
{
    auto b = bit_vector(iterm_num, 0);
    for (size_t i=0; i < b.size(); i+=100)
        b[i] = 1;
    std::cout << "bit vector iterm number:" << iterm_num << std::endl;
    std::cout << "sizeof(iterm_num bit) = " << iterm_num/1024/1024 << "M" << std::endl;
    cout<< "size of bit_vector in MB: " << size_in_mega_bytes(b) << endl;
    b[10] = 0;
    rrr_vector<63> rrrb(b);
    cout<< "size of rrr_vector in MB: " << size_in_mega_bytes(rrrb) << endl;
    sd_vector<> sdb(b);
    cout<< "size of sd_vector in MB: " << size_in_mega_bytes(sdb) << endl;
    std::cout << std::endl;
}

int test_int_vector(long iterm_num)
{
    int_vector<> v;
    v.resize(iterm_num);
    for (long i = 0L; i < v.size(); i++) {
        v[i] = i*10L;
    }
    int_vector<> vlc_v;
    vlc_v.resize(iterm_num);
    for (long i = 0L; i < vlc_v.size(); i++) {
        vlc_v[i] = i*10L;
    }
    std::cout << "int vector iterm number:" << iterm_num << std::endl;
    std::cout << "sizeof(vector<long>) = " << (iterm_num*8)/1024/1024 << "M" << std::endl;
    std::cout << "sizeof(int_vector<>) = " << size_in_mega_bytes(v) << "M" << std::endl;

    util::bit_compress(v);
    std::cout << "sizeof(int_bitcompress_vector<>) = " << size_in_mega_bytes(v) << "M" << std::endl;

    enc_vector<> ev(v);
    std::cout << "sizeof(enc_vector<>) = " << size_in_mega_bytes(ev) << "M" << std::endl;

    util::bit_compress(vlc_v);
    vlc_vector<> vv(vlc_v);
    std::cout << "sizeof(vlc_vector<>) = " << size_in_mega_bytes(vv) << "M" << std::endl;
    std::cout << std::endl;
    return 0;
}

int test_int_vector_address(long iterm_num, long set_num)
{
    std::srand((unsigned int)std::time(NULL));
    Timer timer;
    int_vector<> v;
    v.resize(iterm_num);
    long idx = 0L;
    if (set_num == -1) {
	    for (long i = 0L; i < v.size(); i++) {
		    auto b = bit_vector(1000, 0);
		    v[i] = &b;
	    }
    } else if (set_num < v.size()){
	    for (long i = 0L; i < set_num; i++) {
                    int idx = i + (std::rand() % 1000);
		    auto b = bit_vector(1000, 0);
  		    if (idx < v.size())
		      v[i+idx] = &b;
                    else
                      v[i] = &b;
	    }
    }
    std::cout << "address int vector iterm number:" << iterm_num << " set number:" << set_num << " v[0]=" << v[0]<< std::endl;
    std::cout << "sizeof(vector<long>) = " << (iterm_num*8)/1024/1024 << "M" << std::endl;
    std::cout << "sizeof(int_vector<>) = " << size_in_mega_bytes(v) << "M" << std::endl;
    idx = 0L; 
    for (int i = 0; i < TIME_NUM; i++) {
        idx += 10000L;  
        timer.reset();
        long tmp = v[idx];
        timer.write_text_to_screen("test_int_vector");
    }

    util::bit_compress(v);
    std::cout << "sizeof(int_bitcompress_vector<>) = " << size_in_mega_bytes(v) << "M" << std::endl;
    idx = 0L;
    for (int i = 0; i < TIME_NUM; i++) {
        idx += 10000L;  
        timer.reset();
        long tmp = v[idx];
        timer.write_text_to_screen("test_bitcompress_int_vector");
    }

    enc_vector<> ev(v);
    std::cout << "sizeof(enc_vector<>) = " << size_in_mega_bytes(ev) << "M" << std::endl;
    idx = 0L;
    for (int i = 0; i < TIME_NUM; i++) {
        idx += 10000L;  
        timer.reset();
        long tmp = ev[idx];
        timer.write_text_to_screen("test_enc_vector");
    }

    vlc_vector<> vv(v);
    std::cout << "sizeof(vlc_vector<>) = " << size_in_mega_bytes(vv) << "M" << std::endl;
    idx = 0L;
    for (int i = 0; i < TIME_NUM; i++) {
        idx += 10000L;  
        timer.reset();
        long tmp = vv[idx];
        timer.write_text_to_screen("test_vlc_vector");
    }    
    std::cout << std::endl;
    return 0;
}


void test_wt_int()
{
    wt_int<> wt;
    //0 1 2 3 4 5 6 7 8 9 0 1 2 3 4  5 6 7 8 9 0  1 2
    construct_im(wt, int_vector<> {1,2,2,4,5,3,2,4,5,3,2,4,7,4,2,52,7,4,2,1,5,74,3});

    cout << "wt = " << wt << endl;
    cout << "wt[0,3] intersect with wt[7,10]" << endl;
    auto res = intersect(wt, {{0,3},{7,10}});
    cout << "element : sum of occurrences in ranges" << endl;
    for (auto x : res) {
        cout << x.first << " : " << x.second <<endl;
    }
}

int main(int argc, char* argv[])
{
    test_int_vector(ITERM_NUM_10W);
    test_int_vector(ITERM_NUM_100W);
    test_int_vector(ITERM_NUM_1000W);
    test_int_vector(ITERM_NUM_1Y);
    test_int_vector(ITERM_NUM_3Y);
    test_bit_vector(ITERM_NUM_10W);
    test_bit_vector(ITERM_NUM_100W);
    test_bit_vector(ITERM_NUM_1000W);
    test_bit_vector(ITERM_NUM_1Y);
    test_bit_vector(ITERM_NUM_3Y);
    test_int_vector_address(ITERM_NUM_10W, -1);
    test_int_vector_address(ITERM_NUM_100W, -1);
    test_int_vector_address(ITERM_NUM_1000W, -1);
    test_int_vector_address(ITERM_NUM_1Y, -1);
    test_int_vector_address(ITERM_NUM_3Y, -1);
    test_int_vector_address(ITERM_NUM_1Y, 100000);
    test_int_vector_address(ITERM_NUM_3Y,  100000);
    test_wt_int();
    
}

int test(int argc, char* argv[])
{
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " file [lb] [rb]" << endl;
        cout << " Reads a serialized int_vector<> from disk." <<endl;
        cout << " Outputs elements in the range [lb..rb], if specified." << endl;
        cout << " Otherwise, reads indexes from stdin and outputs values. "<<endl;
    }
    int_vector<> v;
    load_from_file(v, argv[1]);
    cout<<"loaded int_vector<> containing "<<v.size()<<" "<<
        (int)v.width()<<"-bit integers"<<endl;
    if (argc>3) {
        size_t a=stoull(argv[2]);;
        size_t b=stoull(argv[3]);
        if (b >= v.size())
            b = v.size()-1;
        if (a > b)
            a = b;
        for (size_t i=a; i<=b; ++i) {
            cout<<"v["<<i<<"]="<<v[i]<<endl;
        }
    } else {
        cout << "Interactive mode." << endl;
        size_t i;
        while (cin>>i) {
            cout<<"v["<<i<<"]="<<v[i]<<endl;
        }
    }
}

