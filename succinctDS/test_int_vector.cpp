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

using namespace std;
using namespace sdsl;

const int G_THREAD_NUM = 4;
const long G_START = 0;
const long G_END = 10000;

std::vector<long> TestThreadEnc(int idx, long start, long end, enc_vector<> &ev) {
    
    std::vector<long> ret;
    for (std::size_t i = start; i < end; ++i) {
        ret.push_back(ev[i]);
    }
    return ret;
}

int main(int argc, char* argv[])
{
    int_vector<> v;
    v.resize(10000000L);
    for (long i = 0; i < v.size(); i++) {
        v[i] = i*10;
    }
    int_vector<> vlc_v;
    vlc_v.resize(10000000L);
    for (long i = 0; i < vlc_v.size(); i++) {
        vlc_v[i] = i*10;
    }
    std::cout << "sizeof(vector<long>) = " << (10000000L*8)/1024/1024 << "M" << std::endl;
    std::cout << "sizeof(int_vector<>) = " << size_in_mega_bytes(v) << "M" << std::endl;
    Timer timer;  
    //cout << "v.size()" << v.size() << std::endl;
    for (int i = 100; i < 1000000; i++) {
        timer.reset();
        long tmp = v[i];
        timer.write_text_to_log_file("test_int_vector", "test_int_vector");
        //cout<<"v["<<i<<"]="<<v[i]<<endl;
    }

    util::bit_compress(v);
    for (int i = 100; i < 1000000; i++) {
	    timer.reset();
	    long tmp = v[i];
	    timer.write_text_to_log_file("test_bitcompress_vector", "test_bitcompress_vector");
	    //cout<<"v["<<i<<"]="<<v[i]<<endl;
    }   
    std::cout << "sizeof(int_bitcompress_vector<>) = " << size_in_mega_bytes(v) << "M" << std::endl;

    Timer enc_t;
    enc_t.reset();
    enc_vector<> ev(v);
	for (int i = G_START; i < G_END; i++) {
        //timer.reset();
        long tmp = ev[i];
        //timer.write_text_to_log_file("test_enc_vector", "test_enc_vector");
        //cout<<"v["<<i<<"]="<<v[i]<<endl;
    }
    enc_t.write_text_to_screen("test_enc_vector");
    std::cout << "sizeof(enc_vector<>) = " << size_in_mega_bytes(ev) << "M" << std::endl;

    std::vector<std::future<std::vector<long>> > vfuture(G_THREAD_NUM);
    enc_t.reset();
    long start = G_START;
    for (long i = 0; i < G_THREAD_NUM; i++) {
        vfuture[i] = std::async(std::launch::async, TestThreadEnc, i, start, start + (G_END - G_START)/G_THREAD_NUM, std::ref(ev));
        start = start + (G_END - G_START)/G_THREAD_NUM;
    }
    for (auto &vf : vfuture) {
        vf.get();
    }
    TestThreadEnc(G_THREAD_NUM, start + (G_END - G_START)/G_THREAD_NUM, G_END, std::ref(ev));
    enc_t.write_text_to_screen("test_enc_vector_parallel_async");


    util::bit_compress(vlc_v);
    vlc_vector<> vv(vlc_v);
    for (int i = 100; i < 1000000; i++) {
        timer.reset();
        long tmp = vv[i];
        timer.write_text_to_log_file("test_vlc_vector", "test_vlc_vector");
        //cout<<"v["<<i<<"]="<<v[i]<<endl;
    }
    std::cout << "sizeof(vlc_vector<>) = " << size_in_mega_bytes(vv) << "M" << std::endl;
    
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

