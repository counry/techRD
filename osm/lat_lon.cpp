
#include <cstdlib>
#include <iostream>
#include <time_util.hpp>
#include <ctime>
#include <algorithm>
#include <parallel/algorithm>
#include <vector>

uint32 ilat(double lat)
{
    return (lat + 91.0)*10000000+0.5;
}

int32 ilon(double lon)
{
    return lon*10000000 + (lon > 0 ? 0.5 : -0.5);
}

double lon(int32 ilon)
{
    return ((double)ilon)/10000000;
}

double lat(uint32 ilat)
{
    return ((double)ilat)/10000000 - 91.0;
}


int main()
{
    double lat = 48.1245160;
    double lon = -1.6093711;
    std::cout << "transform lat " << lat << " to " << lat(ilat(lat)) << std::endl;
    std::cout << "transform lon " << lon << " to " << lon(ilon(lon)) << std::endl;
    return 0;

}

