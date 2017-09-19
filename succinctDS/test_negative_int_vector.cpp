//! Program for debugging. Load vector v of type int_vector<> from
// file argv[1]. Then the program outputs for each integer i,
// which is read from stdin, the value v[i].
#include <sdsl/int_vector.hpp>
#include <sdsl/vectors.hpp>
#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;
using namespace sdsl;

int main(int argc, char* argv[])
{
    int_vector<> v;
    v.resize(10L);
    int neg = 1;
    for (int i = 0; i < v.size(); i++) {
        neg *= (-1);
	v[i] = i*10*neg;
    }

   for (int i = 0; i < v.size(); i++) {
        cout<<"v["<<i<<"]="<<(int)v[i]<<endl;
    }
  
    util::bit_compress(v);
    vlc_vector<> vv(v);
    for (int i = 0; i < vv.size(); i++) {
        cout<<"vv["<<i<<"]="<<(int)vv[i]<<endl;
    }
    return 0;
}



