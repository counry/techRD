
#include <cstdlib>
#include <iostream>
#include <time_util.hpp>
#include <ctime>
#include <parallel/algorithm>
#include <vector>


#define ITERM_NUM 100*10000

class Data {
public:
    Data(unsigned int num) {
        std::srand(std::time(0)); // use current time as seed for random generator
        for (unsigned int i = 0; i < num; i++)
        {
            m_data_vec.push_back(std::rand());
        }
    }

    bool find(int val) {
        return std::find(m_data_vec.begin(), m_data_vec.end(), val) != m_data_vec.end();
    }

private:
    std::vector<int> m_data_vec;
};

class Database {
public:
    static Database& get_database(void)
    {
        static Database database(ITERM_NUM);
        return database;
    }


    bool find(int val) {
        return m_data.find(val);
    }

private:
    Database(unsigned int num):m_data(num) {}
    Data m_data;
};

class Oper {
public:
    static void init() {
        Database::get_database();
    }

    static bool find(int val) {
        return Database::get_database().find(val);
    }
};


int main()
{
    Oper::init();

    std::vector<int> find_vec;
    std::vector<bool> find_result_vec;
    std::vector<bool> find_par_result_vec;
    find_vec.clear();
    std::srand(std::time(0)); // use current time as seed for random generator
    for (unsigned int i = 0; i < 100000; i++)
    {
        find_vec.push_back(std::rand());
    }


    Timer timer;

    find_result_vec.clear();
    timer.reset();
    std::for_each(find_vec.begin(), find_vec.end(), [&find_result_vec](int &n){ find_result_vec.push_back(Oper::find(n));});
    timer.write_text_to_screen("for_each vector");

    find_par_result_vec.clear();
    timer.reset();
    __gnu_parallel::for_each(find_vec.begin(), find_vec.end(), [&find_result_vec](int &n){ find_result_vec.push_back(Oper::find(n));});
    timer.write_text_to_screen("for_each_parallel vector");
    if (find_par_result_vec == find_result_vec) {
        std::cout << "find result is ok" << std::endl;
    } else {
        std::cout << "find result is error" << std::endl;
    }
    return 0;
}

