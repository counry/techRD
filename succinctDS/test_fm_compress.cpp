
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


void test_lz4(char* file, string identify = "lz4")
{
    string index_suffix = ".fm."+ identify + ".index";
    string index_file   = string(file)+index_suffix;

    std::ifstream fin;
    fin.open(file, std::ifstream::in);
    if (fin.is_open()) {
        auto size = fin.tellg()+1;
        cout << "file " << file << " size " << size << endl;
        std::string str(size, '\0'); // construct string to stream size
        fin.seekg(0);
        if (!fin.read(&str[0], size-1)) {
            cout << "ERROR: File " << file << " read error" << endl;
        }
        const int src_size = str.size();
        int max_dst_size = LZ4_compressBound(src_size);
        cout << "read str size " << src_size << "lz4 max_dst_size " << max_dst_size << endl;

        //compression
        char* compressed_data = malloc(max_dst_size);
        if (compressed_data == NULL) {
            cout << "Failed to allocate memory for *compressed_data." << endl;
        }
        const int compressed_data_size = LZ4_compress_default(&str[0], compressed_data, src_size, max_dst_size);
        if (compressed_data_size < 0) {
            cout << "A negative result from LZ4_compress_default indicates a failure trying to compress the data.  See exit code (echo $?) for value returned." << endl;
        }
        if (compressed_data_size == 0) {
            cout << "A result of 0 means compression worked, but was stopped because the destination buffer couldn't hold all the information." << endl;
        }
        if (compressed_data_size > 0) {
            cout << "We successfully compressed some data! src_size = " << src_size << " compressed_data_size = " << compressed_data_size << endl;
        }
        compressed_data = (char *)realloc(compressed_data, compressed_data_size);
        if (compressed_data == NULL) {
            cout << "Failed to re-alloc memory for compressed_data." << endl;
        }



        //Decompression
        char* const regen_buffer = malloc(src_size);
        if (regen_buffer == NULL) {
            cout << "Failed to allocate memory for *regen_buffer." << endl;
        }
        Timer timer;
        const int decompressed_size = LZ4_decompress_safe(compressed_data, regen_buffer, compressed_data_size, src_size);
        timer.write_text_to_screen(identify);
        free(compressed_data);
        if (decompressed_size < 0) {
            cout << "A negative result from LZ4_decompress_safe indicates a failure trying to decompress the data.  See exit code (echo $?) for value returned." << endl;
        }
        if (decompressed_size == 0) {
            cout << "I'm not sure this function can ever return 0.  Documentation in lz4.h doesn't indicate so." << endl;
        }
        if (decompressed_size > 0) {
            cout << "We successfully decompressed some data! decompressed_size " << decompressed_size << endl;
        }

    } else {
        cout << "ERROR: File " << file << " does not exist. Exit." << endl;
        return 1;
    }
}

void test_lz4_simple()
{
    //compression
	const char* const src = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.";
	const int src_size = (int)(strlen(src) + 1);
	const int max_dst_size = LZ4_compressBound(src_size);
	cout << "get max_dst_size " << max_dst_size << endl;
	char* compressed_data = malloc(max_dst_size);
	if (compressed_data == NULL) {
		cout << "Failed to allocate memory for *compressed_data." << endl;
	}
	const int compressed_data_size = LZ4_compress_default(src, compressed_data, src_size, max_dst_size);
	if (compressed_data_size < 0) {
		cout << "A negative result from LZ4_compress_default indicates a failure trying to compress the data.  See exit code (echo $?) for value returned." << endl;
	}
	if (compressed_data_size == 0) {
		cout << "A result of 0 means compression worked, but was stopped because the destination buffer couldn't hold all the information." << endl;
	}
	if (compressed_data_size > 0) {
		cout << "We successfully compressed some data! src_size = " << src_size << " compressed_data_size = " << compressed_data_size << endl;
	}
	compressed_data = (char *)realloc(compressed_data, compressed_data_size);
	if (compressed_data == NULL) {
    		cout << "Failed to re-alloc memory for compressed_data." << endl;
	}	

	//Decompression
	char* const regen_buffer = malloc(src_size);
	if (regen_buffer == NULL) {
		cout << "Failed to allocate memory for *regen_buffer." << endl;
	}
	const int decompressed_size = LZ4_decompress_safe(compressed_data, regen_buffer, compressed_data_size, src_size);
	free(compressed_data);
	if (decompressed_size < 0) {
		cout << "A negative result from LZ4_decompress_safe indicates a failure trying to decompress the data.  See exit code (echo $?) for value returned." << endl;
	}
	if (decompressed_size == 0) {
		cout << "I'm not sure this function can ever return 0.  Documentation in lz4.h doesn't indicate so." << endl;
	}
	if (decompressed_size > 0) {
		cout << "We successfully decompressed some data! decompressed_size " << decompressed_size << endl;
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
#if 0
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
#endif
    std::cout << endl;
    //test_lz4(argv[1]);
    test_lz4_simple();

    return 0;
}


