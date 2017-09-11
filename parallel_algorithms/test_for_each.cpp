
#include <cstdlib>
#include <iostream>
#include <time_util.hpp>
#include <ctime>
#include <parallel/algorithm>
#include <vector>

int main()
{
    std::srand(std::time(0)); // use current time as seed for random generator

    std::vector<int> random_vector;
    unsigned int vec_size = 50000000;
    for (unsigned int i = 0; i < vec_size; i++) {
        int random_variable = std::rand();
        random_vector.push_back(random_variable);
    }

    Timer timer;

    std::vector<int> random_vector_sort(random_vector.begin(), random_vector.end());
    timer.reset();
    std::sort(random_vector_sort.begin(), random_vector_sort.end());
    timer.write_text_to_screen("sort random vector");

    std::vector<int> random_vector_stable_sort(random_vector.begin(), random_vector.end());
    timer.reset();
    std::stable_sort(random_vector_stable_sort.begin(), random_vector_stable_sort.end());
    timer.write_text_to_screen("stable_sort random vector");
    
    std::vector<int> random_vector_parallel_sort(random_vector.begin(), random_vector.end());
    timer.reset();
    __gnu_parallel::sort(random_vector_parallel_sort.begin(), random_vector_parallel_sort.end());
    timer.write_text_to_screen("parallel_sort random vector");

    std::vector<int> random_vector_parallel_stable_sort(random_vector.begin(), random_vector.end());
    timer.reset();
    __gnu_parallel::stable_sort(random_vector_parallel_stable_sort.begin(), random_vector_parallel_stable_sort.end());
    timer.write_text_to_screen("parallel_stable_sort random vector");
    return 0;

}

