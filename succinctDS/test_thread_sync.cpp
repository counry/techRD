#include <string>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <unistd.h>
#include <thread>
#include <future>
#include <time_util.hpp>

using namespace std;

const int G_THREAD_NUM = 5;
const long long G_START = 0LL;
const long long G_END = 10LL;

void extract_add(long long start, long long end)
{
    Timer t;
    t.reset();
    long long sum = 0LL;
    for (long long i = start; i < end; i++) {
       sleep(2); 
    }
    t.write_text_to_screen("extract_add");
}

int main(int argc, char** argv)
{
    Timer timer;
    long long start = 0LL;
    cout << "test common------" << std::endl; 
    timer.reset();
    extract_add(G_START, G_END);
    timer.write_text_to_screen("test_common");
 
    std::vector<std::thread> th_v;
    cout << "test thread------" << std::endl;
    timer.reset();
    start = G_START;
    for (long i = 0; i < G_THREAD_NUM; i++) {
        th_v.push_back(std::thread(extract_add, start, start + (G_END - G_START)/G_THREAD_NUM));
	start = start + (G_END - G_START)/G_THREAD_NUM;
    }
    extract_add(start + (G_END - G_START)/G_THREAD_NUM, G_END);
    for (auto &t : th_v ) {
        t.join();
    }
    timer.write_text_to_screen("test_thread");

    cout << "test async---" << std::endl;
    std::vector<std::future<void> > vfuture(G_THREAD_NUM);
    timer.reset();
    start = G_START;
    for (long i = 0; i < G_THREAD_NUM; i++) {
        vfuture[i] = std::async(std::launch::async, extract_add, start, start + (G_END - G_START)/G_THREAD_NUM);
        //程序退出不会运行
	//vfuture[i] = std::async(std::launch::deferred, extract_add, start, start + (G_END - G_START)/G_THREAD_NUM);
        //程序退出的时候会以此运行
	//std::async(std::launch::async, extract_add, start, start + (G_END - G_START)/G_THREAD_NUM);
        start = start + (G_END - G_START)/G_THREAD_NUM;
    }
    timer.write_text_to_screen("test_async_1");
    for (auto &vf : vfuture) {
	vf.get();	
    }
    extract_add(start + (G_END - G_START)/G_THREAD_NUM, G_END);
    timer.write_text_to_screen("test_async");
}

