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
const int G_THREAD_NUM = 4;
const long G_START = 0;
const long G_END = 16;
const int G_DELTA = 200;
mutex mtx; 

void extract_by_pos(int idx, long start, long end, std::vector<std::string> &ret)
{
    if (end <= start) {
        return;
    }
    std::stringstream tid_finish;
    tid_finish << "in_extract_by_pos_finish " << idx;
    std::cout << "start[" << start << "]---end[" << end << "]" << std::endl;
    Timer t;
    t.reset();
    //long v_start = (end - start) * G_DELTA;
    long v_start = start * G_DELTA;
    for (int i = start; i < end; i++) {
		//std::lock_guard<std::mutex> lck(mtx);  // protect statements until end of block agains concurent access
       	ret.push_back(extract(fm_index, v_start, v_start+G_DELTA-1));
	    v_start += G_DELTA;
    }
    t.write_text_to_screen(tid_finish.str());
}

std::vector<std::string> extract_by_pos_async(int idx, long start, long end)
{
    std::vector<std::string> ret;
    if (end <= start) {
        return ret;
    }
    std::stringstream tid_finish;
    tid_finish << "in_extract_by_pos_async_finish " << idx;
    std::cout << "start[" << start << "]---end[" << end << "]" << std::endl;
    Timer t;
    t.reset();
    long v_start = start * G_DELTA;
    for (int i = start; i < end; i++) {
        ret.push_back(extract(fm_index, v_start, v_start+G_DELTA-1));
        v_start += G_DELTA;
    }
    t.write_text_to_screen(tid_finish.str());
	return ret;
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
    
#if 0
   for (int i = G_START; i < G_END; i++) {
        timer.reset();
        auto s   = extract(fm_index, start, start+G_DELTA);
        timer.write_text_to_log_file("test_fm", "test_fm");
        start += G_DELTA;
        //cout << "extract s : " << s << std::endl;
	}
#endif
	
	std::vector<std::string> ret;
    timer.reset();
    start = 0;
    for (long i = G_START; i < G_END; i++) {
        ret.push_back(extract(fm_index, start, start+G_DELTA-1));
        start += G_DELTA;
    }
    timer.write_text_to_screen("test_fm_vector");
	std::ofstream log_file("dump.fm.vector", std::ios_base::out);
    for_each(ret.begin(), ret.end(), [&log_file](std::string &st){log_file << st << std::endl;});
    log_file.close();

    std::array<std::vector<std::string>, G_THREAD_NUM+1> ret_parallel;
    std::vector<std::thread> th_v;
    timer.reset();
    start = G_START;
    for (long i = 0; i < G_THREAD_NUM; i++) {
        th_v.push_back(std::thread(extract_by_pos, i, start, start + (G_END - G_START)/G_THREAD_NUM, std::ref(ret_parallel[i])));
        start = start + (G_END - G_START)/G_THREAD_NUM;
    }
    for (auto &t : th_v ) {
        t.join();
    }
    extract_by_pos(G_THREAD_NUM, start + (G_END - G_START)/G_THREAD_NUM, G_END, std::ref(ret_parallel[G_THREAD_NUM]));
    timer.write_text_to_screen("test_fm_parallel");
    std::ofstream log_file_parallel("dump.fm.parallel", std::ios_base::out);
    for_each(ret_parallel.begin(), ret_parallel.end(), [&log_file_parallel](std::vector<std::string> &ret) {
        for_each(ret.begin(), ret.end(),
                 [&log_file_parallel](std::string &st) { log_file_parallel << st << std::endl; });
    });
    log_file.close();


    std::vector<std::vector<std::string>> ret_parallel_async;
    std::vector<std::future<std::vector<std::string>> > vfuture(G_THREAD_NUM);
    timer.reset();
    start = G_START;
    for (long i = 0; i < G_THREAD_NUM; i++) {
        vfuture[i] = std::async(std::launch::async, extract_by_pos_async, i, start, start + (G_END - G_START)/G_THREAD_NUM);
        start = start + (G_END - G_START)/G_THREAD_NUM;
    }
    for (auto &vf : vfuture) {
        ret_parallel_async.push_back(vf.get());
    }
	ret_parallel_async.push_back(extract_by_pos_async(G_THREAD_NUM, start + (G_END - G_START)/G_THREAD_NUM, G_END));
    timer.write_text_to_screen("test_fm_parallel_async");
	std::ofstream log_file_parallel_async("dump.fm.parallel.async", std::ios_base::out);
    for_each(ret_parallel_async.begin(), ret_parallel_async.end(), [&log_file_parallel_async](std::vector<std::string> &ret) {
        for_each(ret.begin(), ret.end(),
                 [&log_file_parallel_async](std::string &st) { log_file_parallel_async << st << std::endl; });
    });
    log_file_parallel_async.close();

        //sleep(10);
#if 0
    cout << "Input search terms and press Ctrl-D to exit." << endl;
    string prompt = "\e[0;32m>\e[0m ";
    cout << prompt;
    string query;
    while (getline(cin, query)) {
        size_t m  = query.size();
        size_t occs = sdsl::count(fm_index, query.begin(), query.end());
        cout << "# of occurrences: " << occs << endl;
        if (occs > 0) {
            cout << "Location and context of first occurrences: " << endl;
            auto locations = locate(fm_index, query.begin(), query.begin()+m);
            sort(locations.begin(), locations.end());
            for (size_t i = 0, pre_extract = pre_context, post_extract = post_context; i < min(occs, max_locations); ++i) {
                cout << setw(8) << locations[i] << ": ";
                if (pre_extract > locations[i]) {
                    pre_extract = locations[i];
                }
                if (locations[i]+m+ post_extract > fm_index.size()) {
                    post_extract = fm_index.size()-locations[i]-m;
                }
                auto s   = extract(fm_index, locations[i]-pre_extract, locations[i]+m+ post_extract-1);
                string pre = s.substr(0, pre_extract);
                s = s.substr(pre_extract);
                if (pre.find_last_of('\n') != string::npos) {
                    pre = pre.substr(pre.find_last_of('\n')+1);
                }
                cout << pre;
                cout << "\e[1;31m";
                cout << s.substr(0, m);
                cout << "\e[0m";
                string context = s.substr(m);
                cout << context.substr(0, context.find_first_of('\n')) << endl;
            }
        }
        cout << prompt;
    }
    cout << endl;
#endif
}

int test(int argc, char** argv)
{
    if (argc <  2) {
        cout << "Usage " << argv[0] << " text_file [max_locations] [post_context] [pre_context]" << endl;
        cout << "    This program constructs a very compact FM-index" << endl;
        cout << "    which supports count, locate, and extract queries." << endl;
        cout << "    text_file      Original text file." << endl;
        cout << "    max_locations  Maximal number of location to report." <<endl;
        cout << "    post_context   Maximal length of the reported post-context." << endl;
        cout << "    pre_context    Maximal length of the pre-context." << endl;
        return 1;
    }
    size_t max_locations = 5;
    size_t post_context = 10;
    size_t pre_context = 10;
    if (argc >= 3) {
        max_locations = atoi(argv[2]);
    }
    if (argc >= 4) {
        post_context = atoi(argv[3]);
    }
    if (argc >= 5) {
        pre_context = atoi(argv[4]);
    }
    string index_suffix = ".fm9";
    string index_file   = string(argv[1])+index_suffix;
    csa_wt<wt_huff<rrr_vector<127> >, 512, 1024> fm_index;

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
    cout << "Input search terms and press Ctrl-D to exit." << endl;
    string prompt = "\e[0;32m>\e[0m ";
    cout << prompt;
    string query;
    while (getline(cin, query)) {
        size_t m  = query.size();
        size_t occs = sdsl::count(fm_index, query.begin(), query.end());
        cout << "# of occurrences: " << occs << endl;
        if (occs > 0) {
            cout << "Location and context of first occurrences: " << endl;
            auto locations = locate(fm_index, query.begin(), query.begin()+m);
            sort(locations.begin(), locations.end());
            for (size_t i = 0, pre_extract = pre_context, post_extract = post_context; i < min(occs, max_locations); ++i) {
                cout << setw(8) << locations[i] << ": ";
                if (pre_extract > locations[i]) {
                    pre_extract = locations[i];
                }
                if (locations[i]+m+ post_extract > fm_index.size()) {
                    post_extract = fm_index.size()-locations[i]-m;
                }
                auto s   = extract(fm_index, locations[i]-pre_extract, locations[i]+m+ post_extract-1);
                string pre = s.substr(0, pre_extract);
                s = s.substr(pre_extract);
                if (pre.find_last_of('\n') != string::npos) {
                    pre = pre.substr(pre.find_last_of('\n')+1);
                }
                cout << pre;
                cout << "\e[1;31m";
                cout << s.substr(0, m);
                cout << "\e[0m";
                string context = s.substr(m);
                cout << context.substr(0, context.find_first_of('\n')) << endl;
            }
        }
        cout << prompt;
    }
    cout << endl;
}

