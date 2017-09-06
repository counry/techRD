#include <sdsl/wavelet_trees.hpp>
#include <iostream>

using namespace sdsl;
using namespace std;

void test_comp()
{
    //0 1 2 3 4 5 6 7 8 9 0 1 2 3 4  5 6 7 8 9 0  1 2
    //int_vector<> intv = {11,12,142,54,65,73,12,11,54};
    int_vector<> intv;
    intv.resize(1000000);
for (unsigned long i = 0; i < 1000000; i++) {
  intv[i] = i * 10;
}    
wt_int<rrr_vector<63>> wt;
wt_int<sd_vector<>> wt_sd;
   util::bit_compress(intv);
enc_vector<> enc_intv(intv);
vlc_vector<> vlc_intv(intv);
      //construct_im(wt, int_vector<> {11,12,142,54,65,73,12,11,54});
      construct_im(wt, intv);
      construct_im(wt_sd, intv);
         // cout << "comp wt = " << wt << endl;
   cout << size_in_mega_bytes(intv) << std::endl;
   cout << size_in_mega_bytes(wt) << std::endl;
   cout << size_in_mega_bytes(wt_sd) << std::endl;
   cout << size_in_mega_bytes(enc_intv) << std::endl;
   cout << size_in_mega_bytes(vlc_intv) << std::endl;
    cout << wt.size() << std::endl;
    cout << wt_sd.size() << std::endl;
}

void test_vlc()
{
    wt_int<> wt;
    //0 1 2 3 4 5 6 7 8 9 0 1 2 3 4  5 6 7 8 9 0  1 2
    int_vector<> v = {11,12,142,54,65,73,12,11,54};
    util::bit_compress(v);
enc_vector<> intv(v);
      //construct_im(wt, int_vector<> {11,12,142,54,65,73,12,11,54});
      construct_im(wt, intv);
          cout << "vlc wt = " << wt << endl;
   cout << size_in_mega_bytes(wt) << std::endl;
    cout << wt.size() << std::endl;
 
}

void test_enc()
{
    wt_int<> wt;
    //0 1 2 3 4 5 6 7 8 9 0 1 2 3 4  5 6 7 8 9 0  1 2
    int_vector<> v = {11,12,142,54,65,73,12,11,54};
    util::bit_compress(v);
vlc_vector<> intv(v);
      //construct_im(wt, int_vector<> {11,12,142,54,65,73,12,11,54});
      construct_im(wt, intv);
          cout << "enc wt = " << wt << endl;
   cout << size_in_mega_bytes(wt) << std::endl;
    cout << wt.size() << std::endl;
 
}


int main()
{
    wt_int<> wt;
    //0 1 2 3 4 5 6 7 8 9 0 1 2 3 4  5 6 7 8 9 0  1 2
    int_vector<> intv = {11,12,142,54,65,73,12,11,54};
      //construct_im(wt, int_vector<> {11,12,142,54,65,73,12,11,54});
      construct_im(wt, intv);

    
           cout << "wt = " << wt << endl;
   cout << size_in_mega_bytes(wt) << std::endl;
    cout << wt.size() << std::endl;
  
    cout << "wt[0,3] intersect with wt[7,10]" << endl;
    auto res = intersect(wt, {{0,3},{5,10}});
    cout << "element : sum of occurrences in ranges" << endl;
    for (auto x : res) {
    cout << x.first << " : " << x.second <<endl;
    }
     
     test_comp();
     //test_vlc();
    //test_enc();

   }

