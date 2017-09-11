

#ifndef testUtil_hpp
#define testUtil_hpp

#include <ctime>
#include <fstream>
#include <iostream>
#include <iomanip>


void write_text_to_log_file( const std::string &text );

class Timer
{
public:
    Timer() { clock_gettime(CLOCK_REALTIME, &beg_); }

    double elapsed() {
        clock_gettime(CLOCK_REALTIME, &end_);
        return (end_.tv_sec - beg_.tv_sec)*1000 +
               (end_.tv_nsec - beg_.tv_nsec) / 1000000.;
    }

    void write_text_to_log_file(const std::string rfile, const std::string &text ){
        clock_gettime(CLOCK_REALTIME, &end_);
        std::string file = "./" + rfile + ".time";
        std::ofstream log_file(file, std::ios_base::out | std::ios_base::app );
        log_file<<"["<<text<<"]"<<" ";
        log_file <<std::fixed << std::setprecision(3)<<beg_.tv_sec*1000 + beg_.tv_nsec/1000000.0 <<" "
                 <<std::setprecision(3)<<end_.tv_sec*1000 + end_.tv_nsec/1000000.0<<" "
                 <<std::setprecision(3)<< (end_.tv_sec - beg_.tv_sec)*1000 + (end_.tv_nsec - beg_.tv_nsec) / 1000000.<< std::endl;
        log_file.close();
    }

    void write_text_to_screen(const std::string &text){
        clock_gettime(CLOCK_REALTIME, &end_);
        std::cout<<"["<<text<<"]"<<" ";
        std::cout<<std::fixed << std::setprecision(3)<<beg_.tv_sec*1000 + beg_.tv_nsec/1000000.0 <<" "
                 <<std::setprecision(3)<<end_.tv_sec*1000 + end_.tv_nsec/1000000.0<<" "
                 <<std::setprecision(3)<< (end_.tv_sec - beg_.tv_sec)*1000 + (end_.tv_nsec - beg_.tv_nsec) / 1000000.<< std::endl;
    }

    void reset() { clock_gettime(CLOCK_REALTIME, &beg_); }

private:
    timespec beg_, end_;
};
#endif /* testUtil_hpp */
