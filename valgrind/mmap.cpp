
#include <cstddef>
#include <fstream>
#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

std::ifstream::pos_type filesize(const char *filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}

void *mmap_file(std::string filename, std::ios_base::openmode mode)
{
	int fd;
	if (!(mode&std::ios_base::out))
		fd = open(filename.c_str(), O_RDONLY);
    else 
		fd = open(filename.c_str(), O_RDWR);
	void* map = nullptr;
    std::cout << filename << ":" << filesize(filename.c_str()) << std::endl;
	if (!(mode&std::ios_base::out)) 
		map = mmap(NULL,filesize(filename.c_str()),PROT_READ,MAP_SHARED,fd, 0);
	else 
		map = mmap(NULL,filesize(filename.c_str()),PROT_READ | PROT_WRITE,MAP_SHARED,fd, 0);
	if(map == MAP_FAILED) 
		map = nullptr; // unify windows and unix error behaviour
	return map;
}

int main(void)
{
	void *map1 = mmap_file("./part_1_dataop_overpass_data.tar.gz", std::ios_base::out);
	void *map2 = mmap_file("./part_1_dataop_overpass_data.tar.gz", std::ios_base::out);
	void *map3 = mmap_file("./compress_node_fm_index.0", std::ios_base::out);
    if (map1 != nullptr && map2 != nullptr && map3 != nullptr) {
		std::cout << "mmap successfuly!" << std::endl;
	} else {
		std::cout << "mmap failed!" << std::endl;
	}
	sleep(100);
    return 0;
}
