
#include <fstream>
#include <string>
#include <iostream>


int main(void)
{
	std::string str(10000, '0');
	std::string m_compress_file = "./test_write_fdisk";
	for (int i = 0; i < 100000; i++) {
		std::ofstream file;
		file.open(m_compress_file.c_str(), std::ios::out | std::ios::app | std::ios::binary);
		if (file.is_open()) {
			file << str;
			file.close();
		} 
	}
	return 0;
}
