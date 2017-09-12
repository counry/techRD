
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
    auto iter = std::unique(random_vector_sort.begin(),random_vector_sort.end());
    if(iter != random_vector_sort.end()) {
        random_vector_sort.erase(iter,random_vector_sort.end());
    }
    timer.write_text_to_screen("unique random vector");
    std::cout << "random_vector_sort_size:" << random_vector_sort.size() << std::endl;
    
    std::vector<int> random_vector_parallel_sort(random_vector.begin(), random_vector.end());
    timer.reset();
    __gnu_parallel::sort(random_vector_parallel_sort.begin(), random_vector_parallel_sort.end());
    auto iter_par = std::unique(random_vector_parallel_sort.begin(),random_vector_parallel_sort.end());
    if(iter_par != random_vector_parallel_sort.end()) {
        random_vector_parallel_sort.erase(iter_par,random_vector_parallel_sort.end());
    }
    timer.write_text_to_screen("parallel_unique random vector");
    std::cout << "random_vector_parallel_sort_size:" << random_vector_parallel_sort.size() << std::endl;

    std::vector<int> random_vector_parallel_copy_sort(random_vector.begin(), random_vector.end());
    std::vector<int> random_vector_parallel_copy_dst;
    timer.reset();
    __gnu_parallel::sort(random_vector_parallel_copy_sort.begin(), random_vector_parallel_copy_sort.end());
    __gnu_parallel::unique_copy(random_vector_parallel_copy_sort.begin(), random_vector_parallel_copy_sort.end(), 
				std::back_inserter(random_vector_parallel_copy_dst));
    timer.write_text_to_screen("parallel_unique_copy random vector");
    std::cout << "random_vector_parallel_copy_dst_size:" << random_vector_parallel_copy_dst.size() << std::endl;

    if (random_vector_sort.size() == random_vector_parallel_sort.size()) {
       std::cout << "unique ok" << std::endl;
    } else {
	std::cout << "unique error" << std::endl;
    }

    if (random_vector_sort.size() == random_vector_parallel_sort.size() && random_vector_sort.size() == random_vector_parallel_copy_dst.size()) {
	std::cout << "unique_copy ok" << std::endl;
    } else {
	std::cout << "unique_copy error" << std::endl;
    }
    return 0;


}

