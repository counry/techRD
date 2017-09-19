
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <parallel/algorithm>
#include <vector>
#include <array>
#include <iomanip>

uint32_t ilatitude(double lat)
{
    return (lat + 91.0)*10000000+0.5;
}

int32_t ilongitude(double lon)
{
    return lon*10000000 + (lon > 0 ? 0.5 : -0.5);
}

double longitude(int32_t ilon)
{
    return ((double)ilon)/10000000;
}

double latitude(uint32_t ilat)
{
    return ((double)ilat)/10000000 - 91.0;
}


int main()
{
    double lat = 0.0;
    double lon = 0.0;
    std::vector<std::array<double, 2>> coords;

    lat = 23.1141643;
    lon = 121.0068784;
    coords.push_back({lat, lon});

    lat = 48.1245160;
    lon = -1.6093711;
    coords.push_back({lat, lon});

    lat = 48.1254138;
    lon = -1.6097754;
    coords.push_back({lat, lon});

    lat = 55.0809291;
    lon = -118.8107007;
    coords.push_back({lat, lon});

    lat = -72.8832451;
    lon = 35.7338765;
    coords.push_back({lat, lon});

    lat = -55.0658177;
    lon = -67.4323684;
    coords.push_back({lat, lon});
    

    for (auto c : coords) {
        double _lat = c.front();
        double _lon = c.back();
	uint64_t u64_lat = ilatitude(_lat);
	uint64_t u64_lon = ilongitude(_lon);
        std::cout << "transform lat " << std::setprecision(10) << _lat << " to " << std::setprecision(10) << latitude(u64_lat) << std::endl;
        std::cout << "transform lon " << std::setprecision(10) << _lon << " to " << std::setprecision(10) << longitude(u64_lon) << std::endl;
    }

    return 0;
}

