
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
#define TRIE_NUMBER 1000000

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

    if (std::ifstream fin{file, std::ios::binary | std::ios::ate}) {
        auto size = fin.tellg()+1;
        cout << "file " << file << " size " << size << endl;
        std::string str(size, '\0'); // construct string to stream size
        fin.seekg(0);
        if (!fin.read(&str[0], size-1)) {
            cout << "ERROR: File " << file << " read error" << endl;
        }
        const int src_size = str.size();
        int max_dst_size = BrotliEncoderMaxCompressedSize(src_size);
        cout << "read str size " << src_size << " lz4 max_dst_size " << max_dst_size << endl;

        //compression
        char* compressed_data = malloc(max_dst_size);
        if (compressed_data == NULL) {
            cout << "Failed to allocate memory for *compressed_data." << endl;
        }
        memset(compressed_data, 0x0, sizeof(compressed_data));
        BROTLI_BOOL compressed_data_ret = BrotliEncoderCompress(::BROTLI_DEFAULT_QUALITY, ::BROTLI_DEFAULT_WINDOW, ::BROTLI_DEFAULT_MODE,
                                                            src_size, &str[0], max_dst_size, compressed_data);

        if (compressed_data_ret == ::BROTLI_FALSE) {
            cout << "A negative result from BrotliEncoderCompress indicates a failure trying to compress the data.  See exit code (echo $?) for value returned." << endl;
            return;
        }

        const int compressed_data_size = strlen(compressed_data);
        cout << "compressed_data size " << compressed_data_size << std::endl;

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
        BrotliDecoderResult decompressed_ret;
        Timer timer;
        for(int i = 0; i < EXTRACT_NUMBER; i++) {
            decompressed_ret = BrotliDecoderDecompress(compressed_data_size, compressed_data, src_size, regen_buffer);
            if (decompressed_ret != BROTLI_DECODER_RESULT_SUCCESS) {
                cout << "A negative result from BrotliDecoderDecompress indicates a failure trying to decompress the data.  See exit code (echo $?) for value returned." << endl;
                return;
            }
        }
        timer.write_text_to_screen(identify);
        free(compressed_data);

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


//char ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ=";
char ALPHABET[] = "BBBBBB";
void MakeKeyset(std::size_t num_keys, marisa::TailMode tail_mode, marisa::Keyset *keyset) {
    char key_buf[200];
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
        //std::cout << "gtest : " << gtest << " gtest_size : " << strlen(gtest) << std::endl;
    }
    keyset->push_back(gtest, strlen(gtest));
}

void test_marisa_trie(std::string identify="marisa_trie") {
    marisa::Trie gtrie;
    marisa::Keyset gkeyset;

    MakeKeyset(TRIE_NUMBER, MARISA_DEFAULT_TAIL, &gkeyset);
    gtrie.build(gkeyset, MARISA_DEFAULT_NUM_TRIES|MARISA_TINY_CACHE);
    std::cout << "finish build trie..." << std::endl;
    std::cout << "gkeyset key number:[" <<  gkeyset.size()  << "]total length in byte:[" << gkeyset.total_length() << "]" << std::endl;
    std::cout << "gtrie key number:[" <<  gtrie.size()  << "]total length in byte:[" << gtrie.io_size() << "]" << std::endl;


    marisa::Agent agent;
    agent.set_query(gtest, strlen(gtest));
    gtrie.lookup(agent);
    //std::cout.write(agent.key().ptr(), agent.key().length());
    //std::cout << ": " << agent.key().id() << std::endl;
    Timer timer;
    for(int i = 0; i < EXTRACT_NUMBER; i++) {
        marisa::Agent _agent;
        _agent.set_query(agent.key().id());
        gtrie.reverse_lookup(_agent);
        //std::cout << "rlookup " << agent.key().id() << " result : " << _agent.key().ptr() << std::endl;
    }
    timer.write_text_to_screen(identify);
}

void test_compress(char *file)
{
    std::cout << "search size: " << strlen(gtest) << std::endl;
    std::srand((unsigned int)std::time(NULL));
    test_marisa_trie();
    std::cout << endl;

    std::ofstream(file, std::ios::binary) << gtest;

    test_fm<csa_wt<wt_huff<rrr_vector<127> >, 512, 1024>>(file, "csa_wt<wt_huff<rrr_vector<127> >, 512, 1024>");
    std::cout << endl;

    test_lz4(file);
    std::cout << endl;

    test_Brotli(file);
    std::cout << endl;

}

int main(int argc, char** argv)
{
    char *file = "gmap_test1.txt";
    gtest = "<node id=\"531645739\" lat=\"33.301707\" lon=\"117.098392\"/>";
    test_compress(file);

    file = "gmap_test2.txt";
    gtest = "<node id=\"76456803\" visible=\"true\" version=\"24\" changeset=\"12034482\" timestamp=\"2012-06-27T08:57:53Z\" user=\"R438\" uid=\"376715\" lat=\"39.9876974\" lon=\"116.4819236\">\n"
    "  <tag k=\"highway\" v=\"traffic_signals\"/>\n"
    " </node>";
    test_compress(file);

    file = "gmap_test3.txt";
    gtest = "<way id=\"173391694\" visible=\"true\" version=\"6\" changeset=\"46071994\" timestamp=\"2017-02-14T08:34:22Z\" user=\"guoxiao\" uid=\"649715\">\n"
    "  <nd ref=\"1842171144\"/>\n"
    "  <nd ref=\"1842171113\"/>\n"
    "  <nd ref=\"1842171112\"/>\n"
    "  <nd ref=\"1842171116\"/>\n"
    "  <nd ref=\"1842171117\"/>\n"
    "  <nd ref=\"1842171121\"/>\n"
    "  <nd ref=\"1842171130\"/>\n"
    "  <nd ref=\"1844687708\"/>\n"
    "  <nd ref=\"1844687702\"/>\n"
    "  <nd ref=\"1844687700\"/>\n"
    "  <nd ref=\"2388259032\"/>\n"
    "  <nd ref=\"2388263724\"/>\n"
    "  <nd ref=\"1844687703\"/>\n"
    "  <nd ref=\"1844687707\"/>\n"
    "  <nd ref=\"1844687705\"/>\n"
    "  <nd ref=\"1844687701\"/>\n"
    "  <nd ref=\"4684850848\"/>\n"
    "  <nd ref=\"2098270394\"/>\n"
    "  <tag k=\"highway\" v=\"cycleway\"/>";
    test_compress(file);

    file = "gmap_test4.txt";
    gtest = "<way id=\"200111964\" visible=\"true\" version=\"8\" changeset=\"38761473\" timestamp=\"2016-04-21T16:19:55Z\" user=\"NotInNebraska\" uid=\"3854013\">\n"
        "  <nd ref=\"3377341487\"/>\n"
        "  <nd ref=\"3377341488\"/>\n"
        "  <nd ref=\"3377341489\"/>\n"
        "  <nd ref=\"3377342393\"/>\n"
        "  <nd ref=\"3377342394\"/>\n"
        "  <nd ref=\"3377342395\"/>\n"
        "  <nd ref=\"3364207174\"/>\n"
        "  <nd ref=\"3377339789\"/>\n"
        "  <nd ref=\"3364207175\"/>\n"
        "  <nd ref=\"3377339995\"/>\n"
        "  <nd ref=\"3377339996\"/>\n"
        "  <nd ref=\"3377339997\"/>\n"
        "  <nd ref=\"3377340001\"/>\n"
        "  <nd ref=\"3377339994\"/>\n"
        "  <nd ref=\"3377339998\"/>\n"
        "  <nd ref=\"3377339999\"/>\n"
        "  <nd ref=\"3377340006\"/>\n"
        "  <nd ref=\"3364207178\"/>\n"
        "  <nd ref=\"3377340012\"/>\n"
        "  <nd ref=\"3364207179\"/>\n"
        "  <nd ref=\"3377340014\"/>\n"
        "  <nd ref=\"3377340013\"/>\n"
        "  <nd ref=\"3377340000\"/>\n"
        "  <nd ref=\"4138951639\"/>\n"
        "  <nd ref=\"3377339792\"/>\n"
        "  <nd ref=\"3377339788\"/>\n"
        "  <nd ref=\"3377339786\"/>\n"
        "  <nd ref=\"3364207172\"/>\n"
        "  <nd ref=\"3377339784\"/>\n"
        "  <nd ref=\"3364207170\"/>\n"
        "  <nd ref=\"3377339782\"/>\n"
        "  <nd ref=\"3377339780\"/>\n"
        "  <nd ref=\"3377339778\"/>\n"
        "  <nd ref=\"3377339776\"/>\n"
        "  <nd ref=\"3377339774\"/>\n"
        "  <nd ref=\"3377339772\"/>\n"
        "  <nd ref=\"3377339767\"/>\n"
        "  <nd ref=\"3377339764\"/>\n"
        "  <nd ref=\"3377339763\"/>\n"
        "  <nd ref=\"3377339770\"/>\n"
        "  <nd ref=\"1078854904\"/>\n"
        "  <nd ref=\"1078854891\"/>\n"
        "  <nd ref=\"1094006715\"/>\n"
        "  <nd ref=\"1094006728\"/>\n"
        "  <nd ref=\"1078854908\"/>\n"
        "  <nd ref=\"1078854869\"/>\n"
        "  <nd ref=\"2101011238\"/>\n"
        "  <nd ref=\"1078854911\"/>\n"
        "  <nd ref=\"1094006697\"/>\n"
        "  <nd ref=\"1094006701\"/>\n"
        "  <nd ref=\"1094006745\"/>\n"
        "  <nd ref=\"1078854897\"/>\n"
        "  <nd ref=\"1078854872\"/>\n"
        "  <nd ref=\"1078854878\"/>\n"
        "  <nd ref=\"1078854890\"/>\n"
        "  <nd ref=\"1078854906\"/>\n"
        "  <nd ref=\"1078854907\"/>\n"
        "  <nd ref=\"1078854867\"/>\n"
        "  <nd ref=\"1078854874\"/>\n"
        "  <nd ref=\"1352557096\"/>\n"
        "  <nd ref=\"1078854898\"/>\n"
        "  <nd ref=\"1352557102\"/>\n"
        "  <nd ref=\"1352557104\"/>\n"
        "  <nd ref=\"1352557105\"/>\n"
        "  <nd ref=\"1078854889\"/>\n"
        "  <nd ref=\"1078854916\"/>\n"
        "  <nd ref=\"1359120992\"/>\n"
        "  <nd ref=\"1078854910\"/>\n"
        "  <nd ref=\"1359120994\"/>\n"
        "  <nd ref=\"1078854896\"/>\n"
        "  <nd ref=\"1352542054\"/>\n"
        "  <nd ref=\"1359120996\"/>\n"
        "  <nd ref=\"1352542058\"/>\n"
        "  <nd ref=\"1352542062\"/>\n"
        "  <nd ref=\"1352542064\"/>\n"
        "  <nd ref=\"1352542069\"/>\n"
        "  <nd ref=\"1352557111\"/>\n"
        "  <nd ref=\"1352542142\"/>\n"
        "  <nd ref=\"1352557115\"/>\n"
        "  <nd ref=\"1352542127\"/>\n"
        "  <nd ref=\"1352542129\"/>\n"
        "  <nd ref=\"1078854894\"/>\n"
        "  <nd ref=\"1352542134\"/>\n"
        "  <nd ref=\"1078854909\"/>\n"
        "  <nd ref=\"1078854871\"/>\n"
        "  <nd ref=\"1074447680\"/>\n"
        "  <tag k=\"electrified\" v=\"rail\"/>\n"
        "  <tag k=\"frequency\" v=\"0\"/>\n"
        "  <tag k=\"gauge\" v=\"1435\"/>\n"
        "  <tag k=\"layer\" v=\"-4\"/>\n"
        "  <tag k=\"railway\" v=\"subway\"/>\n"
        "  <tag k=\"tunnel\" v=\"yes\"/>\n"
        "  <tag k=\"voltage\" v=\"750\"/>\n"
        " </way>";
    test_compress(file);


    file = "gmap_test5.txt";
    gtest = "<relation id=\"2164911\" visible=\"true\" version=\"35\" changeset=\"47190986\" timestamp=\"2017-03-27T05:09:19Z\" user=\"快乐书香虎\" uid=\"4794671\">\n"
    "  <member type=\"node\" ref=\"2562753084\" role=\"stop\"/>\n"
    "  <member type=\"way\" ref=\"249700317\" role=\"platform\"/>\n"
    "  <member type=\"node\" ref=\"1736313301\" role=\"stop\"/>\n"
    "  <member type=\"node\" ref=\"1736313258\" role=\"platform\"/>\n"
    "  <member type=\"node\" ref=\"1736313295\" role=\"stop\"/>\n"
    "  <member type=\"node\" ref=\"1736313281\" role=\"platform\"/>\n"
    "  <member type=\"node\" ref=\"1736406159\" role=\"stop\"/>\n"
    "  <member type=\"way\" ref=\"161675332\" role=\"platform\"/>\n"
    "  <member type=\"node\" ref=\"1736406148\" role=\"stop\"/>\n"
    "  <member type=\"node\" ref=\"1736405833\" role=\"platform\"/>\n"
    "  <member type=\"node\" ref=\"1736406134\" role=\"stop\"/>\n"
    "  <member type=\"way\" ref=\"161675290\" role=\"platform\"/>\n"
    "  <member type=\"node\" ref=\"1736406151\" role=\"stop\"/>\n"
    "  <member type=\"way\" ref=\"161675300\" role=\"platform\"/>\n"
    "  <member type=\"node\" ref=\"1736406144\" role=\"stop\"/>\n"
    "  <member type=\"node\" ref=\"1736405856\" role=\"platform\"/>\n"
    "  <member type=\"node\" ref=\"1736413255\" role=\"stop\"/>\n"
    "  <member type=\"way\" ref=\"161680505\" role=\"platform\"/>\n"
    "  <member type=\"node\" ref=\"1736446999\" role=\"stop\"/>\n"
    "  <member type=\"way\" ref=\"161680504\" role=\"platform\"/>\n"
    "  <member type=\"node\" ref=\"1736447004\" role=\"stop\"/>\n"
    "  <member type=\"way\" ref=\"161680498\" role=\"platform\"/>\n"
    "  <member type=\"node\" ref=\"1736446991\" role=\"stop\"/>\n"
    "  <member type=\"node\" ref=\"1736446950\" role=\"platform\"/>\n"
    "  <member type=\"node\" ref=\"1736446998\" role=\"stop\"/>\n"
    "  <member type=\"way\" ref=\"161680499\" role=\"platform\"/>\n"
    "  <member type=\"node\" ref=\"1736447000\" role=\"stop\"/>\n"
    "  <member type=\"way\" ref=\"161680507\" role=\"platform\"/>\n"
    "  <member type=\"node\" ref=\"1736446997\" role=\"stop\"/>\n"
    "  <member type=\"way\" ref=\"161680496\" role=\"platform\"/>\n"
    "  <member type=\"node\" ref=\"1667004636\" role=\"stop\"/>\n"
    "  <member type=\"node\" ref=\"1736446837\" role=\"platform\"/>\n"
    "  <member type=\"node\" ref=\"1736461136\" role=\"stop\"/>\n"
    "  <member type=\"node\" ref=\"1736461130\" role=\"platform\"/>\n"
    "  <member type=\"node\" ref=\"1736461139\" role=\"stop\"/>\n"
    "  <member type=\"node\" ref=\"1736461116\" role=\"platform\"/>\n"
    "  <member type=\"node\" ref=\"1736461132\" role=\"stop\"/>\n"
    "  <member type=\"node\" ref=\"1736461098\" role=\"platform\"/>\n"
    "  <member type=\"node\" ref=\"2512692409\" role=\"stop\"/>\n"
    "  <member type=\"node\" ref=\"2512692395\" role=\"platform\"/>\n"
    "  <member type=\"node\" ref=\"1736475388\" role=\"stop\"/>\n"
    "  <member type=\"way\" ref=\"161684137\" role=\"platform\"/>\n"
    "  <member type=\"node\" ref=\"1974915224\" role=\"stop\"/>\n"
    "  <member type=\"way\" ref=\"186748120\" role=\"platform\"/>\n"
    "  <member type=\"node\" ref=\"1736532764\" role=\"stop\"/>\n"
    "  <member type=\"way\" ref=\"161691267\" role=\"platform\"/>\n"
    "  <member type=\"way\" ref=\"42498688\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"48064151\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161649155\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161658493\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161658488\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"128879297\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161675397\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161675395\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161675396\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161675355\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161675398\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"249565253\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"33505890\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161675369\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161675323\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161675272\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"169686340\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"169686342\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161675324\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161675270\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"169686338\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"36453374\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"184477868\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"154106346\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161680511\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161680522\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161680518\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161680497\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161680509\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161680502\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161680519\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"184477852\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"154106336\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"154106349\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"154117676\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"243914204\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"243914203\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"478368008\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"154117646\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"154117672\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"60051360\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161685599\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"169212594\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"244010335\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161685600\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"120495327\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"120495325\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"154255533\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"154255534\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"196117052\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"169425782\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161688908\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"169204613\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"173319059\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161688915\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161688913\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"169442280\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"240812558\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"169439322\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161688911\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161688916\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"240743199\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"199822784\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161688912\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"161688910\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"169123776\" role=\"\"/>\n"
    "  <member type=\"way\" ref=\"24399971\" role=\"\"/>\n"
    "  <tag k=\"from\" v=\"顺义南彩汽车站\"/>\n"
    "  <tag k=\"name\" v=\"Bus 915: 顺义南彩汽车站 =&gt; 东直门外\"/>\n"
    "  <tag k=\"ref\" v=\"915\"/>\n"
    "  <tag k=\"route\" v=\"bus\"/>\n"
    "  <tag k=\"to\" v=\"东直门外\"/>\n"
    "  <tag k=\"type\" v=\"route\"/>\n"
    " </relation>";
    test_compress(file);

    //test_lz4_simple();

    return 0;
}


