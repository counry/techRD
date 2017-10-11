
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
#include <lz4.h>
#include <brotli/encode.h>
#include <brotli/decode.h>
#include <marisa.h>

using namespace sdsl;
using namespace std;

#define EXTRACT_NUMBER 10
#define TRIE_NUMBER 100

char *gtest = NULL;

template<class t_fm>
void test_fm(char* file, string identify)
{
    cout << "start " << identify << "......" << endl;
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
    cout << "extract size " << fm_index.size() << endl;
    Timer timer;
    for(int i = 0; i < EXTRACT_NUMBER; i++) {
        extract(fm_index, 0, fm_index.size());
    }
    timer.write_text_to_screen(identify);
    cout << "end " << identify << "......" << endl << endl;
}


void test_lz4(char* file, string identify = "lz4")
{
    cout << "start " << identify << "......"  << endl;
    string index_suffix = ".fm."+ identify + ".index";
    string index_file   = string(file)+index_suffix;

    if (std::ifstream fin{file, std::ios::binary | std::ios::ate}) {
        auto size = fin.tellg()+1;
        cout << "file " << file << " size " << size << endl;
        std::string str(size, '\0'); // construct string to stream size
        fin.seekg(0);
        if (!fin.read(&str[0], size-1)) {
            cout << "ERROR: File " << file << " read error" << endl;
        }
        const int src_size = str.size();
        int max_dst_size = LZ4_compressBound(src_size);
        cout << "read str size " << src_size << " lz4 max_dst_size " << max_dst_size << endl;

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

	    int decompressed_size = 0;
        Timer timer;
    	for(int i = 0; i < EXTRACT_NUMBER; i++) {
        	decompressed_size = LZ4_decompress_safe(compressed_data, regen_buffer, compressed_data_size, src_size);
        }
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
    cout << "end " << identify << "......" << endl << endl;
}

void test_Brotli(char* file, string identify = "Brotli")
{
    cout << "start " << identify << "......"  << endl;
    string index_suffix = ".fm."+ identify + ".index";
    string index_file   = string(file)+index_suffix;

    if (std::ifstream fin{file, std::ios::binary | std::ios::ate}) {
        auto size = fin.tellg()+1;
        cout << "file " << file << " size " << size << endl;
        std::string str(size, '\0'); // construct string to stream size
        fin.seekg(0);
        if (!fin.read(&str[0], size-1)) {
            cout << "ERROR: File " << file << " read error" << endl;
        }
        const int src_size = str.size();
        int max_dst_size = LZ4_compressBound(src_size);
        cout << "read str size " << src_size << " lz4 max_dst_size " << max_dst_size << endl;

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

        int decompressed_size = 0;
        Timer timer;
        for(int i = 0; i < EXTRACT_NUMBER; i++) {
            decompressed_size = LZ4_decompress_safe(compressed_data, regen_buffer, compressed_data_size, src_size);
        }
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
    cout << "end " << identify << "......" << endl << endl;
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


char ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ=";
void MakeKeyset(std::size_t num_keys, marisa::TailMode tail_mode, marisa::Keyset *keyset) {
    char key_buf[64];
    for (std::size_t i = 0; i < num_keys; ++i) {
        std::size_t length = sizeof(key_buf);
        for (std::size_t j = 0; j < length; ++j) {
            key_buf[j] = ALPHABET[(std::rand() % sizeof(ALPHABET)-1)];
        }
        keyset->push_back(key_buf, length);
    }
    if (gtest == NULL) {
        std::cout << "gtest is null" << std::endl;
        return;
    } else {
        std::cout << "gtest : " << gtest << " gtest_size : " << strlen(gtest) << std::endl;
    }
    keyset->push_back(gtest, strlen(gtest));
}

void test_marisa_trie(std::string identify="marisa_trie") {
    marisa::Trie gtrie;
    marisa::Keyset gkeyset;

    MakeKeyset(TRIE_NUMBER, MARISA_DEFAULT_TAIL, &gkeyset);
    gtrie.build(gkeyset);
    std::cout << "finish build trie..." << std::endl;
    std::cout << "gkeyset key number:[" <<  gkeyset.size()  << "]total length in byte:[" << gkeyset.total_length() << "]" << std::endl;
    std::cout << "gtrie key number:[" <<  gtrie.size()  << "]total length in byte:[" << gtrie.io_size() << "]" << std::endl;


    marisa::Agent agent;
    agent.set_query(gtest, strlen(gtest));
    gtrie.lookup(agent);
    std::cout.write(agent.key().ptr(), agent.key().length());
    std::cout << ": " << agent.key().id() << std::endl;
    Timer timer;
    for(int i = 0; i < EXTRACT_NUMBER; i++) {
        marisa::Agent _agent;
        _agent.set_query(agent.key().id());
        gtrie.reverse_lookup(_agent);
        std::cout << "rlookup " << agent.key().id() << " result : " << _agent.key().ptr() << std::endl;
    }
    timer.write_text_to_screen(identify);
}

int main(int argc, char** argv)
{
    char *gtest = "<node id=\"531645739\" lat=\"33.301707\" lon=\"117.098392\"/>";
    std::srand((unsigned int)std::time(NULL));
    test_marisa_trie();
    std::cout << endl;

    std::ofstream("test1.txt", std::ios::binary) << gtest;

    test_fm<csa_wt<wt_huff<rrr_vector<127> >, 512, 1024>>("test1.txt", "csa_wt<wt_huff<rrr_vector<127> >, 512, 1024>");
    std::cout << endl;

    test_lz4("test1.txt");
    std::cout << endl;

    //test_Brotli(argv[1]);
    //test_lz4_simple();

    return 0;
}


