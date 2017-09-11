
#include <cstdlib>
#include <iostream>
#include <time_util.hpp>
#include <ctime>
#include <parallel/algorithm>
#include <vector>

int main()
{
    unsigned int vec_size = 5000000;

    std::srand(std::time(0)); // use current time as seed for random generator

    std::vector<int> random_vector1;
    for (unsigned int i = 0; i < vec_size; i++) {
        int random_variable = std::rand();
        random_vector1.push_back(random_variable);
    }

    std::vector<int> random_vector2;
    for (unsigned int i = 0; i < vec_size; i++) {
        int random_variable = std::rand();
        random_vector2.push_back(random_variable);
    }

    std::vector<int> random_vector_dest1;
    std::vector<int> random_vector_dest2;

    Timer timer;

    random_vector_dest1.clear();
    timer.reset();
    std::set_union(random_vector1.begin(), random_vector1.end(),
                   random_vector2.begin(), random_vector2.end(),
                   std::back_inserter(random_vector_dest1));
    timer.write_text_to_screen("set_union");

    random_vector_dest2.clear();
    timer.reset();
    __gnu_parallel::set_union(random_vector1.begin(), random_vector1.end(),
                   random_vector2.begin(), random_vector2.end(),
                   std::back_inserter(random_vector_dest2));
    timer.write_text_to_screen("set_union_parallel");

    if (random_vector_dest1 == random_vector_dest2) {
        std::cout << "set_union ok" << std::endl;
    } else {
        std::cout << "set_union error" << std::endl;
    }

    return 0;

}

