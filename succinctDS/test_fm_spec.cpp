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

csa_wt<wt_huff<rrr_vector<127> >, 512, 1024> fm_index;
//const int G_THREAD_NUM = 4;
const long G_START = 0;
const long G_END = 10000;
const int G_DELTA_200 = 200;
const int G_DELTA_500 = 500;
const int G_DELTA_1000 = 1000;
const int G_DELTA_2000 = 2000;
const int ARRAY_MAX_NUM = 65;

void extract_by_pos(int idx, int delta, long start, long end, std::vector<std::string> &ret)
{
    if (end <= start) {
        return;
    }
//    std::stringstream tid_finish;
//    tid_finish << "in_extract_by_pos_finish " << idx;
     // std::cout << "start[" << start << "]---end[" << end << "]" << std::endl;
//    Timer t;
//    t.reset();
    //long v_start = (end - start) * G_DELTA;
    long v_start = start * delta;
    for (int i = start; i < end; i++) {
        //std::lock_guard<std::mutex> lck(mtx);  // protect statements until end of block agains concurent access
        ret.push_back(extract(fm_index, v_start, v_start+delta-1));
        v_start += delta;
    }
//    t.write_text_to_screen(tid_finish.str());
}


int test_thread(int G_THREAD_NUM, string osm_file)
{
    if (G_THREAD_NUM >= ARRAY_MAX_NUM) {
        cout << " thread num " << G_THREAD_NUM << " is reach limit" << endl;
        return 1;
    }

    string index_suffix = ".fm.index";
    string index_file   = osm_file+index_suffix;

    if (!load_from_file(fm_index, index_file)) {
        ifstream in(osm_file);
        if (!in) {
            cout << "ERROR: File " << osm_file << " does not exist. Exit." << endl;
            return 1;
        }
        cout << "No index "<<index_file<< " located. Building index now." << endl;
        construct(fm_index, osm_file, 1); // generate index
        store_to_file(fm_index, index_file); // save it
    }
    cout << "Index construction complete, index requires " << size_in_mega_bytes(fm_index) << " MiB." << endl;
    cout << "Thread num " << G_THREAD_NUM << " test..." << endl;    

    Timer timer;
    long start = 0;
    std::array<std::vector<std::string>, ARRAY_MAX_NUM> ret_parallel;
    std::vector<std::thread> th_v;
    int total_size = 0;

    for (auto &rp : ret_parallel) {
        rp.clear();
    }
    timer.reset();
    extract_by_pos(G_THREAD_NUM, G_DELTA_200, G_START, G_END, std::ref(ret_parallel[G_THREAD_NUM]));
    timer.write_text_to_screen("test_fm_finish_200");
    cout << "get result total size " << ret_parallel[G_THREAD_NUM].size() << endl;

    for (auto &rp : ret_parallel) {
        rp.clear();
    }
    timer.reset();
    extract_by_pos(G_THREAD_NUM, G_DELTA_500, G_START, G_END, std::ref(ret_parallel[G_THREAD_NUM]));
    timer.write_text_to_screen("test_fm_finish_500");
    cout << "get result total size " << ret_parallel[G_THREAD_NUM].size() << endl;

    for (auto &rp : ret_parallel) {
        rp.clear();
    }
    timer.reset();
    extract_by_pos(G_THREAD_NUM, G_DELTA_1000, G_START, G_END, std::ref(ret_parallel[G_THREAD_NUM]));
    timer.write_text_to_screen("test_fm_finish_1000");
    cout << "get result total size " << ret_parallel[G_THREAD_NUM].size() << endl;

    for (auto &rp : ret_parallel) {
        rp.clear();
    }
    timer.reset();
    extract_by_pos(G_THREAD_NUM, G_DELTA_2000, G_START, G_END, std::ref(ret_parallel[G_THREAD_NUM]));
    timer.write_text_to_screen("test_fm_finish_2000");
    cout << "get result total size " << ret_parallel[G_THREAD_NUM].size() << endl;

    th_v.clear();
    for (auto &rp : ret_parallel) {
        rp.clear();
    }
    timer.reset();
    start = G_START;
    for (long i = 0; i < G_THREAD_NUM; i++) {
        th_v.push_back(std::thread(extract_by_pos, i, G_DELTA_200, start, start + (G_END - G_START)/G_THREAD_NUM, std::ref(ret_parallel[i])));
        start = start + (G_END - G_START)/G_THREAD_NUM;
    }
    for (auto &t : th_v ) {
        t.join();
    }
    extract_by_pos(G_THREAD_NUM, G_DELTA_200, start, G_END, std::ref(ret_parallel[G_THREAD_NUM]));
    timer.write_text_to_screen("test_fm_thread_finish_200");
    total_size = 0;
    for (auto &rp : ret_parallel) {
        total_size += rp.size(); 
    }
    cout << "get result total size " << total_size << endl;

    th_v.clear();
    for (auto &rp : ret_parallel) {
        rp.clear();
    }
    timer.reset();
    start = G_START;
    for (long i = 0; i < G_THREAD_NUM; i++) {
        th_v.push_back(std::thread(extract_by_pos, i, G_DELTA_500, start, start + (G_END - G_START)/G_THREAD_NUM, std::ref(ret_parallel[i])));
        start = start + (G_END - G_START)/G_THREAD_NUM;
    }
    for (auto &t : th_v ) {
        t.join();
    }
    extract_by_pos(G_THREAD_NUM, G_DELTA_500, start, G_END, std::ref(ret_parallel[G_THREAD_NUM]));
    timer.write_text_to_screen("test_fm_thread_finish_500");
    total_size = 0;
    for (auto &rp : ret_parallel) {
        total_size += rp.size(); 
    }
    cout << "get result total size " << total_size << endl;

    th_v.clear();
    for (auto &rp : ret_parallel) {
        rp.clear();
    }
    timer.reset();
    start = G_START;
    for (long i = 0; i < G_THREAD_NUM; i++) {
        th_v.push_back(std::thread(extract_by_pos, i, G_DELTA_1000, start, start + (G_END - G_START)/G_THREAD_NUM, std::ref(ret_parallel[i])));
        start = start + (G_END - G_START)/G_THREAD_NUM;
    }
    for (auto &t : th_v ) {
        t.join();
    }
    extract_by_pos(G_THREAD_NUM, G_DELTA_1000, start, G_END, std::ref(ret_parallel[G_THREAD_NUM]));
    timer.write_text_to_screen("test_fm_thread_finish_1000");
    total_size = 0;
    for (auto &rp : ret_parallel) {
        total_size += rp.size(); 
    }
    cout << "get result total size " << total_size << endl;

    th_v.clear();
    for (auto &rp : ret_parallel) {
        rp.clear();
    }
    timer.reset();
    start = G_START;
    for (long i = 0; i < G_THREAD_NUM; i++) {
        th_v.push_back(std::thread(extract_by_pos, i, G_DELTA_2000, start, start + (G_END - G_START)/G_THREAD_NUM, std::ref(ret_parallel[i])));
        start = start + (G_END - G_START)/G_THREAD_NUM;
    }
    for (auto &t : th_v ) {
        t.join();
    }
    extract_by_pos(G_THREAD_NUM, G_DELTA_2000, start, G_END, std::ref(ret_parallel[G_THREAD_NUM]));
    timer.write_text_to_screen("test_fm_thread_finish_2000");
    total_size = 0;
    for (auto &rp : ret_parallel) {
        total_size += rp.size(); 
    }
    cout << "get result total size " << total_size << endl;
    return 0;
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

    test_thread(4, string(argv[1]));
    test_thread(8, string(argv[1]));
    test_thread(16, string(argv[1]));
    test_thread(32, string(argv[1]));
    return 0;
}

#if 0

int main_bak(int argc, char** argv)
{
    if (argc <  2) {
        cout << "Usage " << argv[0] << " text_file" << endl;
        cout << "    This program constructs a very compact FM-index" << endl;
        cout << "    which supports count, locate, and extract queries." << endl;
        cout << "    text_file      Original text file." << endl;
        return 1;
    }

    string index_suffix = ".fm.index";
    string index_file   = string(argv[1])+index_suffix;

    if (!load_from_file(fm_index, index_file)) {
        ifstream in(argv[1]);
        if (!in) {
            cout << "ERROR: File " << argv[1] << " does not exist. Exit." << endl;
            return 1;
        }
        cout << "No index "<<index_file<< " located. Building index now." << endl;
        construct(fm_index, argv[1], 1); // generate index
        store_to_file(fm_index, index_file); // save it
    }
    cout << "Index construction complete, index requires " << size_in_mega_bytes(fm_index) << " MiB." << endl;
  
    cout << "Thread num " << G_THREAD_NUM << " test..." << endl;    

    Timer timer;
    long start = 0;
    std::array<std::vector<std::string>, G_THREAD_NUM+1> ret_parallel;
    std::vector<std::thread> th_v;
    int total_size = 0;

    for (auto &rp : ret_parallel) {
        rp.clear();
    }
    timer.reset();
    extract_by_pos(G_THREAD_NUM, G_DELTA_200, G_START, G_END, std::ref(ret_parallel[G_THREAD_NUM]));
    timer.write_text_to_screen("test_fm_finish_200");
    cout << "get result total size " << ret_parallel[G_THREAD_NUM].size() << endl << endl;

    for (auto &rp : ret_parallel) {
        rp.clear();
    }
    timer.reset();
    extract_by_pos(G_THREAD_NUM, G_DELTA_500, G_START, G_END, std::ref(ret_parallel[G_THREAD_NUM]));
    timer.write_text_to_screen("test_fm_finish_500");
    cout << "get result total size " << ret_parallel[G_THREAD_NUM].size() << endl << endl;

    for (auto &rp : ret_parallel) {
        rp.clear();
    }
    timer.reset();
    extract_by_pos(G_THREAD_NUM, G_DELTA_1000, G_START, G_END, std::ref(ret_parallel[G_THREAD_NUM]));
    timer.write_text_to_screen("test_fm_finish_1000");
    cout << "get result total size " << ret_parallel[G_THREAD_NUM].size() << endl << endl;

    for (auto &rp : ret_parallel) {
        rp.clear();
    }
    timer.reset();
    extract_by_pos(G_THREAD_NUM, G_DELTA_2000, G_START, G_END, std::ref(ret_parallel[G_THREAD_NUM]));
    timer.write_text_to_screen("test_fm_finish_2000");
    cout << "get result total size " << ret_parallel[G_THREAD_NUM].size() << endl << endl;

    th_v.clear();
    for (auto &rp : ret_parallel) {
        rp.clear();
    }
    timer.reset();
    start = G_START;
    for (long i = 0; i < G_THREAD_NUM; i++) {
        th_v.push_back(std::thread(extract_by_pos, i, G_DELTA_200, start, start + (G_END - G_START)/G_THREAD_NUM, std::ref(ret_parallel[i])));
        start = start + (G_END - G_START)/G_THREAD_NUM;
    }
    for (auto &t : th_v ) {
        t.join();
    }
    extract_by_pos(G_THREAD_NUM, G_DELTA_200, start + (G_END - G_START)/G_THREAD_NUM, G_END, std::ref(ret_parallel[G_THREAD_NUM]));
    timer.write_text_to_screen("test_fm_thread_finish_200");
    total_size = 0;
    for (auto &rp : ret_parallel) {
        total_size += rp.size(); 
    }
    cout << "get result total size " << total_size << endl << endl;

    th_v.clear();
    for (auto &rp : ret_parallel) {
        rp.clear();
    }
    timer.reset();
    start = G_START;
    for (long i = 0; i < G_THREAD_NUM; i++) {
        th_v.push_back(std::thread(extract_by_pos, i, G_DELTA_500, start, start + (G_END - G_START)/G_THREAD_NUM, std::ref(ret_parallel[i])));
        start = start + (G_END - G_START)/G_THREAD_NUM;
    }
    for (auto &t : th_v ) {
        t.join();
    }
    extract_by_pos(G_THREAD_NUM, G_DELTA_500, start + (G_END - G_START)/G_THREAD_NUM, G_END, std::ref(ret_parallel[G_THREAD_NUM]));
    timer.write_text_to_screen("test_fm_thread_finish_500");
    total_size = 0;
    for (auto &rp : ret_parallel) {
        total_size += rp.size(); 
    }
    cout << "get result total size " << total_size << endl << endl;

    th_v.clear();
    for (auto &rp : ret_parallel) {
        rp.clear();
    }
    timer.reset();
    start = G_START;
    for (long i = 0; i < G_THREAD_NUM; i++) {
        th_v.push_back(std::thread(extract_by_pos, i, G_DELTA_1000, start, start + (G_END - G_START)/G_THREAD_NUM, std::ref(ret_parallel[i])));
        start = start + (G_END - G_START)/G_THREAD_NUM;
    }
    for (auto &t : th_v ) {
        t.join();
    }
    extract_by_pos(G_THREAD_NUM, G_DELTA_1000, start + (G_END - G_START)/G_THREAD_NUM, G_END, std::ref(ret_parallel[G_THREAD_NUM]));
    timer.write_text_to_screen("test_fm_thread_finish_1000");
    total_size = 0;
    for (auto &rp : ret_parallel) {
        total_size += rp.size(); 
    }
    cout << "get result total size " << total_size << endl << endl;

    th_v.clear();
    for (auto &rp : ret_parallel) {
        rp.clear();
    }
    timer.reset();
    start = G_START;
    for (long i = 0; i < G_THREAD_NUM; i++) {
        th_v.push_back(std::thread(extract_by_pos, i, G_DELTA_2000, start, start + (G_END - G_START)/G_THREAD_NUM, std::ref(ret_parallel[i])));
        start = start + (G_END - G_START)/G_THREAD_NUM;
    }
    for (auto &t : th_v ) {
        t.join();
    }
    extract_by_pos(G_THREAD_NUM, G_DELTA_2000, start + (G_END - G_START)/G_THREAD_NUM, G_END, std::ref(ret_parallel[G_THREAD_NUM]));
    timer.write_text_to_screen("test_fm_thread_finish_2000");
    total_size = 0;
    for (auto &rp : ret_parallel) {
        total_size += rp.size(); 
    }
    cout << "get result total size " << total_size << endl << endl;

}
#endif


