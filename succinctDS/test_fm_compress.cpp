
#include <sdsl/suffix_arrays.hpp>
#include <sdsl/suffix_trees.hpp>
#include <string>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <unistd.h>
#include <thread>
#include <future>
#include <mutex>

#include <time_util.hpp>
#include "lz4.h"

using namespace sdsl;
using namespace std;

#define EXTRACT_NUMBER 10

template<class t_fm>
void test_fm(char* file, string identify)
{
    t_fm fm_index;
    string index_suffix = ".fm."+ identify + ".index";
    string index_file   = string(file)+index_suffix;

    if (!load_from_file(fm_index, index_file)) {
        ifstream in(file);
        if (!in) {
            cout << "ERROR: File " << file << " does not exist. Exit." << endl;
            return 1;
        }
        cout << "No index "<<index_file<< " located. Building index now." << endl;
        construct(fm_index, file, 1); // generate index
        store_to_file(fm_index, index_file); // save it
    }
    cout << "Index construction complete, " << identify << " index requires " << size_in_mega_bytes(fm_index) << " MiB." << endl;

    Timer timer;
    for(int i = 0; i < EXTRACT_NUMBER; i++) {
        extract(fm_index, 0, fm_index.size());
    }
    timer.write_text_to_screen(identify);
}


void test_lz4(char* file)
{
    string index_suffix = ".fm."+ identify + ".index";
    string index_file   = string(file)+index_suffix;

    std::ifstream fin;
    fin.open(file, std::ifstream::in);
    if (fin.is_open()) {
        auto size = fin.tellg();
        cout << "file " << file << " size " << size << endl;
        std::string str(size+1, '\0'); // construct string to stream size
        is.seekg(0);
        if (!is.read(&str[0], size)) {
            cout << "ERROR: File " << file << " read error" << endl;
        }
        char dst[1024*1024*10];
        memset(dst, 0x0, sizeof(dst));
        int rv = LZ4_compress_default(str.c_str(), dst, str.size(), sizeof(dst)-1);
        cout << "LZ4_compress_default return " << rv << endl;
        if (rv < 1) {
            cout << "Couldn't run LZ4_compress_default()... error code received is in exit code. return " << rv << endl;
        }

        char dst_decompress[1024*1024*30];
        memset(dst_decompress, 0x0, sizeof(dst_decompress));
        Timer timer;
        rv = LZ4_decompress_safe(dst, dst_decompress, sizeof(dst)-1, sizeof(dst_decompress)-1);
        timer.write_text_to_screen(identify);
        cout << "LZ4_decompress_safe return " << rv << endl;
        if (rv < 1) {
            cout << "Couldn't run LZ4_decompress_safe()... error code received is in exit code. return " << rv << endl;
        }
    } else {
        cout << "ERROR: File " << file << " does not exist. Exit." << endl;
        return 1;
    }

}

#if 0
void test_fm_csa_wt2(char* file, string identify="csa_wt2")
{
    csa_wt<> fm_index;
    string index_suffix = ".fm."+ identify + ".index";
    string index_file   = string(file)+index_suffix;

    if (!load_from_file(fm_index, index_file)) {
        ifstream in(file);
        if (!in) {
            cout << "ERROR: File " << file << " does not exist. Exit." << endl;
            return 1;
        }
        cout << "No index "<<index_file<< " located. Building index now." << endl;
        construct(fm_index, file, 1); // generate index
        store_to_file(fm_index, index_file); // save it
    }
    cout << "Index construction complete, " << identify << " index requires " << size_in_mega_bytes(fm_index) << " MiB." << endl;

    Timer timer;
    for(int i = 0; i < EXTRACT_NUMBER; i++) {
        extract(fm_index, 0, fm_index.size());
    }
    timer.write_text_to_screen(identify);
}

void test_fm_csa_wt3(char* file, string identify="csa_wt3")
{
    csa_wt<wt_huff<rrr_vector<63>>,4,8> fm_index;
    string index_suffix = ".fm."+ identify + ".index";
    string index_file   = string(file)+index_suffix;

    if (!load_from_file(fm_index, index_file)) {
        ifstream in(file);
        if (!in) {
            cout << "ERROR: File " << file << " does not exist. Exit." << endl;
            return 1;
        }
        cout << "No index "<<index_file<< " located. Building index now." << endl;
        construct(fm_index, file, 1); // generate index
        store_to_file(fm_index, index_file); // save it
    }
    cout << "Index construction complete, " << identify << " index requires " << size_in_mega_bytes(fm_index) << " MiB." << endl;

    Timer timer;
    for(int i = 0; i < EXTRACT_NUMBER; i++) {
        extract(fm_index, 0, fm_index.size());
    }
    timer.write_text_to_screen(identify);
}

void test_fm_csa_wt4(char* file, string identify="csa_wt4")
{
    csa_wt<wt_int<rrr_vector<63>>> fm_index;
    string index_suffix = ".fm."+ identify + ".index";
    string index_file   = string(file)+index_suffix;

    if (!load_from_file(fm_index, index_file)) {
        ifstream in(file);
        if (!in) {
            cout << "ERROR: File " << file << " does not exist. Exit." << endl;
            return 1;
        }
        cout << "No index "<<index_file<< " located. Building index now." << endl;
        construct(fm_index, file, 1); // generate index
        store_to_file(fm_index, index_file); // save it
    }
    cout << "Index construction complete, " << identify << " index requires " << size_in_mega_bytes(fm_index) << " MiB." << endl;

    Timer timer;
    for(int i = 0; i < EXTRACT_NUMBER; i++) {
        extract(fm_index, 0, fm_index.size());
    }
    timer.write_text_to_screen(identify);
}

void test_fm_csa_bitcompressed(char* file, string identify="csa_bitcompressed")
{
    csa_bitcompressed<> fm_index;
    string index_suffix = ".fm."+ identify + ".index";
    string index_file   = string(file)+index_suffix;

    if (!load_from_file(fm_index, index_file)) {
        ifstream in(file);
        if (!in) {
            cout << "ERROR: File " << file << " does not exist. Exit." << endl;
            return 1;
        }
        cout << "No index "<<index_file<< " located. Building index now." << endl;
        construct(fm_index, file, 1); // generate index
        store_to_file(fm_index, index_file); // save it
    }
    cout << "Index construction complete, " << identify << " index requires " << size_in_mega_bytes(fm_index) << " MiB." << endl;

    Timer timer;
    for(int i = 0; i < EXTRACT_NUMBER; i++) {
        extract(fm_index, 0, fm_index.size());
    }
    timer.write_text_to_screen(identify);
}

void test_fm_csa_sada(char* file, string identify="csa_sada")
{
    csa_sada<> fm_index;
    string index_suffix = ".fm."+ identify + ".index";
    string index_file   = string(file)+index_suffix;

    if (!load_from_file(fm_index, index_file)) {
        ifstream in(file);
        if (!in) {
            cout << "ERROR: File " << file << " does not exist. Exit." << endl;
            return 1;
        }
        cout << "No index "<<index_file<< " located. Building index now." << endl;
        construct(fm_index, file, 1); // generate index
        store_to_file(fm_index, index_file); // save it
    }
    cout << "Index construction complete, " << identify << " index requires " << size_in_mega_bytes(fm_index) << " MiB." << endl;

    Timer timer;
    for(int i = 0; i < EXTRACT_NUMBER; i++) {
        extract(fm_index, 0, fm_index.size());
    }
    timer.write_text_to_screen(identify);
}
#endif

int main(int argc, char** argv)
{
    if (argc <  2) {
        cout << "Usage " << argv[0] << " text_file" << endl;
        cout << "    This program constructs a very compact FM-index" << endl;
        cout << "    which supports count, locate, and extract queries." << endl;
        cout << "    text_file      Original text file." << endl;
        return 1;
    }
    test_fm<csa_wt<wt_huff<rrr_vector<127> >, 512, 1024>>(argv[1], "wt_huff<rrr_vector<127>>,512,1024>");
    std::cout << endl;
    test_fm<csa_wt<>>(argv[1], "csa_wt<>");
    std::cout << endl;
    test_fm<csa_wt<wt_huff<rrr_vector<63>>,4,8>>(argv[1], "csa_wt<wt_huff<rrr_vector<63>>,4,8>");
    std::cout << endl;
    test_fm<csa_wt<wt_int<rrr_vector<63>>>>(argv[1], "csa_wt<wt_int<rrr_vector<63>>>");
    std::cout << endl;
    test_fm<csa_bitcompressed<>>(argv[1], "csa_bitcompressed<>");
    std::cout << endl;
    test_fm<csa_sada<>>(argv[1], "csa_sada<>");

    std::cout << endl;
    test_lz4(argv[1]);

    return 0;
}


