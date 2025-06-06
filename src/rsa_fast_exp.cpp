#include <iostream>
#include <cstdint>

uint64_t mod_pow(uint64_t base, uint64_t exp, uint64_t mod){
    uint64_t res = 1 % mod;
    base %= mod;
    while(exp){
        if(exp & 1) res = (__uint128_t)res * base % mod; // 防溢位
        base = (__uint128_t)base * base % mod;
        exp >>= 1;                                      // 依 e 的二進位位元
    }
    return res;
}

int main(int argc,char* argv[]){
    if(argc!=4){ std::cerr<<"Usage: base e mod\n"; return 1;}
    uint64_t b = std::stoull(argv[1]),
             e = std::stoull(argv[2]),
             m = std::stoull(argv[3]);
    std::cout<<mod_pow(b,e,m)<<"\n";
    return 0;
}
