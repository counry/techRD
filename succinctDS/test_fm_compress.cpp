#include <sdsl/suffix_arrays.hpp>
#include <string>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <unistd.h>
#include <thread>
#include <future>
#include <mutex>

#include <time_util.hpp>
using namespace sdsl;
using namespace std;

#define EXTRACT_NUMBER 10


void test_fm_csa_wt1(char* file, string identify="csa_wt1")
{
    string identify = __FUNCTION__;
    csa_wt<wt_huff<rrr_vector<127> >, 512, 1024> fm_index;
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

void test_fm_cst_sada(char* file, string identify="cst_sada")
{
    cst_sada<> fm_index;
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

void test_fm_cst_sct3_a(char* file, string identify="cst_sct3_a")
{
    cst_sct3<> fm_index;
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

void test_fm_cst_sct3_b(char* file, string identify="cst_sct3_b")
{
    cst_sct3<csa_wt<wt_int<rrr_vector<>>>> fm_index;
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

void test_fm_cst_sct3_c(char* file, string identify="cst_sct3_c")
{
    cst_sct3<csa_wt<wt_huff<rrr_vector<>>>, lcp_support_sada<>> fm_index;
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

void test_fm_wt_hutu(char* file, string identify="wt_hutu")
{
    //wt_hutu<> fm_index;
    wt_hutu<rrr_vector<63>> fm_index;
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

void test_fm_wt_int(char* file, string identify="wt_int")
{
    //wt_int<> fm_index;
    wt_int<rrr_vector<63>> fm_index;
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

void test_fm_wt_huff_int(char* file, string identify="wt_huff_int")
{
    //wt_huff_int<> fm_index;
    wt_huff_int<rrr_vector<63>> fm_index;
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


int main(int argc, char** argv)
{
    if (argc <  2) {
        cout << "Usage " << argv[0] << " text_file" << endl;
        cout << "    This program constructs a very compact FM-index" << endl;
        cout << "    which supports count, locate, and extract queries." << endl;
        cout << "    text_file      Original text file." << endl;
        return 1;
    }
    test_fm_csa_wt1(argv[1]);
    std::cout << endl;
    test_fm_csa_wt2(argv[1]);
    std::cout << endl;
    test_fm_csa_wt3(argv[1]);
    std::cout << endl;
    test_fm_csa_wt4(argv[1]);
    std::cout << endl;
    test_fm_csa_bitcompressed(argv[1]);
    std::cout << endl;
    test_fm_csa_sada(argv[1]);
    std::cout << endl;
    test_fm_cst_sada(argv[1]);
    std::cout << endl;
    test_fm_cst_sct3_a(argv[1]);
    std::cout << endl;
    test_fm_cst_sct3_b(argv[1]);
    std::cout << endl;
    test_fm_cst_sct3_c(argv[1]);
    std::cout << endl;
    test_fm_wt_hutu(argv[1]);
    std::cout << endl;
    test_fm_wt_int(argv[1]);
    std::cout << endl;
    test_fm_wt_huff_int(argv[1]);
    return 0;
}


