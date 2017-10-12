
#include <string>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <unistd.h>

#include <lz4.h>
#include <brotli/encode.h>
#include <brotli/decode.h>


char *gtest = NULL;

void write_to_file(std::string ofile, std::string &msg)
{
    std::ofstream file;
    file.open(ofile.c_str(), std::ios::out|std::ios::app|std::ios::binary);
    if (file.is_open()) {
        file << msg;
        file.close();
    }
}

void compress_lz4(std::string file, std::string str)
{
    char *compressed_data = NULL;
    char *regen_buffer = NULL;
    const int src_size = str.size();
    int max_dst_size = LZ4_compressBound(src_size);

    std::cout << "compress string " << str << std::endl;

    //compression
    compressed_data = (char *)malloc(max_dst_size);
    if (compressed_data == NULL) {
        std::cout << "Failed to allocate memory for *compressed_data." << std::endl;
        goto _ERROR_RETURN;
    }
    memset(compressed_data, 0x0, sizeof(max_dst_size));
    const int compressed_data_size = LZ4_compress_default(&str[0], compressed_data, src_size, max_dst_size);
    if (compressed_data_size < 0) {
        std::cout << "A negative result from LZ4_compress_default indicates a failure trying to compress the data.  See exit code (echo $?) for value returned." << std::endl;
        goto _ERROR_RETURN;
    }
    if (compressed_data_size == 0) {
        std::cout << "A result of 0 means compression worked, but was stopped because the destination buffer couldn't hold all the information." << std::endl;
        goto _ERROR_RETURN;
    }
    std::cout << "compressed_data_size " << compressed_data_size << std::endl;
    std::string data(std::begin(compressed_data), std::begin(compressed_data)+compressed_data_size);
    std::cout << "get data size " << data.size() << std::endl;
    write_to_file(file, data);


    //Decompression
    regen_buffer = (char *)malloc(src_size);
    if (regen_buffer == NULL) {
        std::cout << "Failed to allocate memory for *regen_buffer." << std::endl;
        goto _ERROR_RETURN;
    }
    memset(regen_buffer, 0x0, sizeof(src_size));
    const int decompressed_size = LZ4_decompress_safe(compressed_data, regen_buffer, compressed_data_size, src_size);
    if (decompressed_size < 0) {
        std::cout << "A negative result from LZ4_decompress_safe indicates a failure trying to decompress the data.  See exit code (echo $?) for value returned." << std::endl;
        goto _ERROR_RETURN;
    }
    if (decompressed_size == 0) {
        std::cout << "I'm not sure this function can ever return 0.  Documentation in lz4.h doesn't indicate so." << std::endl;
        goto _ERROR_RETURN;
    }
    if (decompressed_size > 0) {
        std::cout << "We successfully decompressed some data! decompressed data size " << decompressed_size << " data: ";
        std::cout.write(regen_buffer, decompressed_size);
        std::cout << std::endl;
    }

    _ERROR_RETURN:
    if (compressed_data)
        free(compressed_data);
    if (regen_buffer)
        free(regen_buffer);
    return;
}

void compress_Brotli(std::string file, std::string str)
{
    char *compressed_data = NULL;
    char *regen_buffer = NULL;

    size_t src_size = str.size();
    size_t max_dst_size = BrotliEncoderMaxCompressedSize(src_size);

    //compression
    compressed_data = (char *)malloc(max_dst_size);
    if (compressed_data == NULL) {
        std::cout << "Failed to allocate memory for *compressed_data." << std::endl;
        goto _ERROR_RETURN;
    }
    memset(compressed_data, 0x0, sizeof(max_dst_size));
    BROTLI_BOOL compressed_data_ret = BrotliEncoderCompress(BROTLI_DEFAULT_QUALITY, BROTLI_DEFAULT_WINDOW, BROTLI_DEFAULT_MODE,
                                                        src_size, &str[0], &max_dst_size, compressed_data);

    if (compressed_data_ret == BROTLI_FALSE) {
        std::cout << "A negative result from BrotliEncoderCompress indicates a failure trying to compress the data.  See exit code (echo $?) for value returned." << std::endl;
        goto _ERROR_RETURN;
    } else {
        std::cout << "compress_size " << max_dst_size << std::endl;
    }
    std::string data(std::begin(compressed_data), std::begin(compressed_data)+max_dst_size);
    std::cout << "get data size " << data.size() << std::endl;
    write_to_file(file, data);

    const int compressed_data_size = max_dst_size;

    //Decompression
    regen_buffer = (char *)malloc(src_size);
    if (regen_buffer == NULL) {
        std::cout << "Failed to allocate memory for *regen_buffer." << std::endl;
        goto _ERROR_RETURN;
    }
    memset(compressed_data, 0x0, sizeof(src_size));
    BrotliDecoderResult decompressed_ret;
    decompressed_ret = BrotliDecoderDecompress(compressed_data_size, compressed_data, &src_size, regen_buffer);
    if (decompressed_ret != BROTLI_DECODER_RESULT_SUCCESS) {
        cout << "A negative result from BrotliDecoderDecompress indicates a failure trying to decompress the data.  See exit code (echo $?) for value returned." << endl;
        goto _ERROR_RETURN;
    }
    std::cout << "We successfully decompressed some data! decompressed data size " << src_size << " data: ";
    std::cout.write(regen_buffer, src_size);
    std::cout << std::endl;

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

void op_compress(std::string file, std::string &str)
{
    static std::stringstream str_5k;
    static std::stringstream str_5k_size = 0;
    static std::stringstream str_10k;
    static std::stringstream str_10k_size = 0;
    static std::stringstream str_64k;
    static std::stringstream str_64k_size = 0;
    static std::stringstream str_128k;
    static std::stringstream str_128k_size = 0;
    static std::stringstream str_512k;
    static std::stringstream str_512k_size = 0;
    static std::stringstream str_1M;
    static std::stringstream str_1M_size = 0;

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

int main(int argc, char** argv)
{
    std::string file = "op_compress_data";

    gtest = "<node id=\"531645739\" lat=\"33.301707\" lon=\"117.098392\"/>";
    op_compress(file, std::string(gtest));

    gtest = "<node id=\"76456803\" visible=\"true\" version=\"24\" changeset=\"12034482\" timestamp=\"2012-06-27T08:57:53Z\" user=\"R438\" uid=\"376715\" lat=\"39.9876974\" lon=\"116.4819236\">\n"
    "  <tag k=\"highway\" v=\"traffic_signals\"/>\n"
    " </node>";
    op_compress(file, std::string(gtest));

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
    op_compress(file, std::string(gtest));

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
    op_compress(file, std::string(gtest));

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
    op_compress(file, std::string(gtest));


    return 0;
}


