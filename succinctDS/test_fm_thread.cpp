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
const int G_THREAD_NUM = 2;
const long G_START = 0;
const long G_END = 4;
const int G_DELTA = 10000;

void extract_by_pos(int idx, long start, long end, std::vector<std::string> &ret)
{
    if (end <= start) {
        return;
    }
    std::stringstream tid_before;
    std::stringstream tid_after;
    std::stringstream tid_finish;
    tid_before << "in_extract_by_pos_extract_before " << idx;
    tid_after << "in_extract_by_pos_extract_after " << idx;
    tid_finish << "in_extract_by_pos_finish " << idx;
    std::cout << "start[" << start << "]---end[" << end << "]" << std::endl;
    Timer t;
    t.reset();
    //long v_start = (end - start) * G_DELTA;
    long v_start = start * G_DELTA;
    for (int i = start; i < end; i++) {
	//std::lock_guard<std::mutex> lck(mtx);  // protect statements until end of block agains concurent access
       	//ret.push_back(extract(fm_index, v_start, v_start+G_DELTA-1));
        //t.write_text_to_screen(tid_before.str());
        std::cout << "vstart[" << v_start << "]---vend[" << v_start+G_DELTA-1 << "]" << std::endl;
	t.reset();
       	extract(fm_index, v_start, v_start+G_DELTA-1);
    	t.write_text_to_screen(tid_after.str());
       	//std::cout << "start[" << v_start << "]---end[" << v_start+G_DELTA-1 << "]" << std::endl;
	v_start += G_DELTA;
    }
    //t.write_text_to_screen(tid_finish.str());
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
    
    Timer timer;
    long start = 0;

    std::array<std::vector<std::string>, G_THREAD_NUM+1> ret_parallel;
    std::vector<std::thread> th_v;
    timer.reset();
    start = G_START;
    for (long i = 0; i < G_THREAD_NUM; i++) {
        th_v.push_back(std::thread(extract_by_pos, i, start, start + (G_END - G_START)/G_THREAD_NUM, std::ref(ret_parallel[i])));
        start = start + (G_END - G_START)/G_THREAD_NUM;
    }
    timer.write_text_to_screen("before_thread_join");
    for (auto &t : th_v ) {
        t.join();
    }
    extract_by_pos(G_THREAD_NUM, start + (G_END - G_START)/G_THREAD_NUM, G_END, std::ref(ret_parallel[G_THREAD_NUM]));
    timer.write_text_to_screen("test_fm_thread_finish");

    timer.reset();
    extract_by_pos(G_THREAD_NUM, G_START, G_END, std::ref(ret_parallel[G_THREAD_NUM]));
    timer.write_text_to_screen("test_fm_finish");
    }


