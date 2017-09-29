



#include "spdlog/spdlog.h"
#include <iostream>
#include <memory>
#include <unistd.h>

namespace spd = spdlog;

void async_example()
{
    size_t q_size = 4096; //queue size must be power of 2
    spdlog::set_async_mode(q_size);
    auto async_file = spd::daily_logger_st("async_file_logger", "logs/async_log");

    for (int i = 0; i < 10; ++i)
        async_file->info("Async message #{}", i);
	async_file->flush();
}



int main(void)
{
	try {
		// Create a file rotating logger with 5mb size max and 3 rotated files
        auto rotating_logger = spd::rotating_logger_mt("some_logger_name", "logs/mylogfile", 1048576 * 5, 3);
        for (int i = 0; i < 10; ++i)
            rotating_logger->info("{} * {} equals {:>10}", i, i, i*i);

		// Create a daily logger - a new file is created every day on 2:30am
        auto daily_logger = spd::daily_logger_mt("daily_logger", "logs/daily", 2, 30);
        // trigger flush if the log severity is error or higher
        daily_logger->flush_on(spd::level::err);
        daily_logger->info(123.44);

	// Customize msg format for all messages
        spd::set_pattern("[Y%-%M-%D %H:%M:%S][thread %t]%v");
        rotating_logger->info("This is another message with custom format");

	// Runtime log levels
        spd::set_level(spd::level::err); //Set global log level to info
        rotating_logger->debug("This message shold not be displayed!");
        rotating_logger->set_level(spd::level::debug); // Set specific logger's log level
        rotating_logger->debug("This message shold be displayed..");


// Asynchronous logging is very fast..
        // Just call spdlog::set_async_mode(q_size) and all created loggers from now on will be asynchronous..
        async_example();

    for (int i = 30; i < 40; ++i)
spd::get("async_file_logger")->error("Async message #{}", i);
	
	spd::get("async_file_logger")->flush();



	sleep(30);

// Release and close all loggers
        spdlog::drop_all();


	} catch (const spd::spdlog_ex& ex)
    {
        std::cout << "Log init failed: " << ex.what() << std::endl;
        return 1;
    }
}
