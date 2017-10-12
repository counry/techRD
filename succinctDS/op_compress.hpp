
#include <string>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <memory.h>
#include <iterator>

#include <lz4.h>
#include <brotli/encode.h>
#include <brotli/decode.h>


inline void write_to_file(std::string ofile, std::string &msg)
{
    std::ofstream file;
    file.open(ofile.c_str(), std::ios::out|std::ios::app|std::ios::binary);
    if (file.is_open()) {
        file << msg;
        file.close();
    }
}

inline void compress_lz4(std::string file, std::string str)
{
    //std::cout << "start lz4......" << std::endl;
    char *compressed_data = NULL;
    char *regen_buffer = NULL;
    const int src_size = str.size();
    int max_dst_size = LZ4_compressBound(src_size);
    int decompressed_size = 0;
    int compressed_data_size = 0;
    //std::cout << "compress string " << str.size() << std::endl;

    //compression
    compressed_data = (char *)malloc(max_dst_size);
    if (compressed_data == NULL) {
        std::cout << "Failed to allocate memory for *compressed_data." << std::endl;
        goto _ERROR_RETURN;
    }
    memset(compressed_data, 0x0, sizeof(max_dst_size));
    compressed_data_size = LZ4_compress_default(&str[0], compressed_data, src_size, max_dst_size);
    if (compressed_data_size < 0) {
        std::cout << "A negative result from LZ4_compress_default indicates a failure trying to compress the data.  See exit code (echo $?) for value returned." << std::endl;
        goto _ERROR_RETURN;
    }
    if (compressed_data_size == 0) {
        std::cout << "A result of 0 means compression worked, but was stopped because the destination buffer couldn't hold all the information." << std::endl;
        goto _ERROR_RETURN;
    }
    //std::cout << "compressed_data_size " << compressed_data_size << std::endl;
    {
    //std::string data(std::begin(compressed_data), std::begin(compressed_data)+compressed_data_size);
    std::string data(compressed_data, compressed_data_size);
    //std::cout << "get data size " << data.size() << std::endl;
    write_to_file(file, data);
}

    //Decompression
    regen_buffer = (char *)malloc(src_size);
    if (regen_buffer == NULL) {
        std::cout << "Failed to allocate memory for *regen_buffer." << std::endl;
        goto _ERROR_RETURN;
    }
    memset(regen_buffer, 0x0, sizeof(src_size));
    decompressed_size = LZ4_decompress_safe(compressed_data, regen_buffer, compressed_data_size, src_size);
    if (decompressed_size < 0) {
        std::cout << "A negative result from LZ4_decompress_safe indicates a failure trying to decompress the data.  See exit code (echo $?) for value returned." << std::endl;
        goto _ERROR_RETURN;
    }
    if (decompressed_size == 0) {
        std::cout << "I'm not sure this function can ever return 0.  Documentation in lz4.h doesn't indicate so." << std::endl;
        goto _ERROR_RETURN;
    }
    if (decompressed_size > 0) {
        //std::cout << "We successfully decompressed some data! decompressed data size " << decompressed_size << " data: ";
        //std::cout.write(regen_buffer, decompressed_size);
        //std::cout << std::endl;
    }

    //std::cout << "end lz4......" << std::endl << std::endl;

    _ERROR_RETURN:
    if (compressed_data)
        free(compressed_data);
    if (regen_buffer)
        free(regen_buffer);
    return;
}

inline void compress_Brotli(std::string file, std::string str)
{
    //std::cout << "start brotli......" << std::endl;
    char *compressed_data = NULL;
    char *regen_buffer = NULL;

    //std::cout << "compress size " << str.size() << std::endl;
    size_t src_size = str.size();
    size_t max_dst_size = BrotliEncoderMaxCompressedSize(src_size);
    int compressed_data_size = 0;
    BROTLI_BOOL compressed_data_ret = BROTLI_FALSE;

    //compression
    compressed_data = (char *)malloc(max_dst_size);
    if (compressed_data == NULL) {
        std::cout << "Failed to allocate memory for *compressed_data." << std::endl;
        goto _ERROR_RETURN;
    }
    memset(compressed_data, 0x0, sizeof(max_dst_size));
    compressed_data_ret = BrotliEncoderCompress(BROTLI_DEFAULT_QUALITY, BROTLI_DEFAULT_WINDOW, BROTLI_DEFAULT_MODE,
                                                        src_size, &str[0], &max_dst_size, compressed_data);

    if (compressed_data_ret == BROTLI_FALSE) {
        std::cout << "A negative result from BrotliEncoderCompress indicates a failure trying to compress the data.  See exit code (echo $?) for value returned." << std::endl;
        goto _ERROR_RETURN;
    } else {
        //std::cout << "compress_size " << max_dst_size << std::endl;
    }
 	{
    std::string data(compressed_data, max_dst_size);
    //std::string data(std::begin(compressed_data), std::begin(compressed_data)+max_dst_size);
    //std::cout << "get data size " << data.size() << std::endl;
    write_to_file(file, data);
	}
    compressed_data_size = max_dst_size;

    //Decompression
    regen_buffer = (char *)malloc(src_size);
    if (regen_buffer == NULL) {
        std::cout << "Failed to allocate memory for *regen_buffer." << std::endl;
        goto _ERROR_RETURN;
    }
    memset(regen_buffer, 0x0, sizeof(src_size));
    BrotliDecoderResult decompressed_ret;
    decompressed_ret = BrotliDecoderDecompress(compressed_data_size, compressed_data, &src_size, regen_buffer);
    if (decompressed_ret != BROTLI_DECODER_RESULT_SUCCESS) {
        std::cout << "A negative result from BrotliDecoderDecompress indicates a failure trying to decompress the data. return " << decompressed_ret << std::endl;
        goto _ERROR_RETURN;
    }
    //std::cout << "We successfully decompressed some data! decompressed data size " << src_size << " data: ";
    //std::cout.write(regen_buffer, src_size);
    //std::cout << std::endl;

    //std::cout << "end brotli......" << std::endl << std::endl;
    _ERROR_RETURN:
    if (compressed_data)
        free(compressed_data);
    if (regen_buffer)
        free(regen_buffer);
    return;
}

#define NUM_5k 5120
#define NUM_10k 10240
#define NUM_64k 65536
#define NUM_128k 131072
#define NUM_512k 524288
#define NUM_1M 1048576

inline void op_compress(std::string file, std::string str)
{
    static std::stringstream str_5k;
    static uint16_t str_5k_size = 0;
    static std::stringstream str_10k;
    static uint16_t str_10k_size = 0;
    static std::stringstream str_64k;
    static uint16_t str_64k_size = 0;
    static std::stringstream str_128k;
    static uint16_t str_128k_size = 0;
    static std::stringstream str_512k;
    static uint16_t str_512k_size = 0;
    static std::stringstream str_1M;
    static uint16_t str_1M_size = 0;

    std::string file_lz4       = file + ".lz4";
    std::string file_lz4_5k    = file + ".lz4.5k";
    std::string file_lz4_10k   = file + ".lz4.10k";
    std::string file_lz4_64k   = file + ".lz4.64k";
    std::string file_lz4_128k  = file + ".lz4.128k";
    std::string file_lz4_512k  = file + ".lz4.512k";
    std::string file_lz4_1M    = file + ".lz4.1M";

    std::string file_brotli       = file + ".brotli";
    std::string file_brotli_5k    = file + ".brotli.5k";
    std::string file_brotli_10k   = file + ".brotli.10k";
    std::string file_brotli_64k   = file + ".brotli.64k";
    std::string file_brotli_128k  = file + ".brotli.128k";
    std::string file_brotli_512k  = file + ".brotli.512k";
    std::string file_brotli_1M    = file + ".brotli.1M";

    if (str_5k_size == 0) {
        str_5k.str("");
    }
    if (str_10k_size == 0) {
        str_10k.str("");
    }
    if (str_64k_size == 0) {
        str_64k.str("");
    }
    if (str_128k_size == 0) {
        str_128k.str("");
    }
    if (str_512k_size == 0) {
        str_512k.str("");
    }
    if (str_1M_size == 0) {
        str_1M.str("");
    }

    compress_lz4(file_lz4, str);
    compress_Brotli(file_brotli, str);

    str_5k << str;
    str_5k_size += str.size();
    if (str_5k_size >= NUM_5k) {
        compress_lz4(file_lz4_5k, str_5k.str());
        compress_Brotli(file_brotli_5k, str_5k.str());
        str_5k_size = 0;
        str_5k.str("");
    }

    str_10k << str;
    str_10k_size += str.size();
    if (str_10k_size >= NUM_10k) {
        compress_lz4(file_lz4_10k, str_10k.str());
        compress_Brotli(file_brotli_10k, str_10k.str());
        str_10k_size = 0;
        str_10k.str("");
    }

    str_64k << str;
    str_64k_size += str.size();
    if (str_64k_size >= NUM_64k) {
        compress_lz4(file_lz4_64k, str_64k.str());
        compress_Brotli(file_brotli_64k, str_64k.str());
        str_64k_size = 0;
        str_64k.str("");
    }

    str_128k << str;
    str_128k_size += str.size();
    if (str_128k_size >= NUM_128k) {
        compress_lz4(file_lz4_128k, str_128k.str());
        compress_Brotli(file_brotli_128k, str_128k.str());
        str_128k_size = 0;
        str_128k.str("");
    }

    str_512k << str;
    str_512k_size += str.size();
    if (str_512k_size >= NUM_512k) {
        compress_lz4(file_lz4_512k, str_512k.str());
        compress_Brotli(file_brotli_512k, str_512k.str());
        str_512k_size = 0;
        str_512k.str("");
    }

    str_1M << str;
    str_1M_size += str.size();
    if (str_1M_size >= NUM_1M) {
        compress_lz4(file_lz4_1M, str_1M.str());
        compress_Brotli(file_brotli_1M, str_1M.str());
        str_1M_size = 0;
        str_1M.str("");
    }
}

