
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <parallel/algorithm>
#include <vector>

uint32_t ilat(double lat)
{
    return (lat + 91.0)*10000000+0.5;
}

int32_t ilon(double lon)
{
    return lon*10000000 + (lon > 0 ? 0.5 : -0.5);
}

double lon(int32_t ilon)
{
    return ((double)ilon)/10000000;
}

double lat(uint32_t ilat)
{
    return ((double)ilat)/10000000 - 91.0;
}


int main()
{
    double lat_ = 48.1245160;
    double lon_ = -1.6093711;
    std::cout << "transform lat " << lat_ << " to " << lat(ilat(lat_)) << std::endl;
    std::cout << "transform lon " << lon_ << " to " << lon(ilon(lon_)) << std::endl;
    return 0;
}

