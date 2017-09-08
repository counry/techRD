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


string get_string(long start, long end)
{
    string ret("");
    if (!fm_index.empty()) {
        if (start > fm_index.size() || end > fm_index.empty() || start > end || (end - start) > 1024*1024*2) {
            return 1;
        }
        ret = extract(fm_index, start, end);
    }
    return ret;
}

string get_string_nocheck(long start, long end)
{
    return extract(fm_index, start, end);
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
    cout << "Index construction complete, index requires " << size_in_mega_bytes(fm_index) << " MiB. size:" << fm_index.size() << endl;

    std::cout << get_string(0, 0) << std::endl;
    std::cout << get_string(0, fm_index.size()-10) << std::endl;
    std::cout << get_string(0, fm_index.size()) << std::endl;
    std::cout << get_string(0, fm_index.size()+10) << std::endl;

    std::cout << get_string(fm_index.size()-10, 0) << std::endl;
    std::cout << get_string(fm_index.size()-10, fm_index.size()-10) << std::endl;
    std::cout << get_string(fm_index.size()-10, fm_index.size()) << std::endl;
    std::cout << get_string(fm_index.size()-10, fm_index.size()+10) << std::endl;

    std::cout << get_string(fm_index.size(), 0) << std::endl;
    std::cout << get_string(fm_index.size(), fm_index.size()-10) << std::endl;
    std::cout << get_string(fm_index.size(), fm_index.size()) << std::endl;
    std::cout << get_string(fm_index.size(), fm_index.size()+10) << std::endl;

#if 0
    std::cout << get_string_nocheck(0, 0) << std::endl;
    std::cout << get_string_nocheck(0, fm_index.size()-10) << std::endl;
    std::cout << get_string_nocheck(0, fm_index.size()) << std::endl;
    std::cout << get_string_nocheck(0, fm_index.size()+10) << std::endl;

    std::cout << get_string_nocheck(fm_index.size()-10, 0) << std::endl;
    std::cout << get_string_nocheck(fm_index.size()-10, fm_index.size()-10) << std::endl;
    std::cout << get_string_nocheck(fm_index.size()-10, fm_index.size()) << std::endl;
    std::cout << get_string_nocheck(fm_index.size()-10, fm_index.size()+10) << std::endl;

    std::cout << get_string_nocheck(fm_index.size(), 0) << std::endl;
    std::cout << get_string_nocheck(fm_index.size(), fm_index.size()-10) << std::endl;
    std::cout << get_string_nocheck(fm_index.size(), fm_index.size()) << std::endl;
    std::cout << get_string_nocheck(fm_index.size(), fm_index.size()+10) << std::endl;
#endif

    return 0;
}


