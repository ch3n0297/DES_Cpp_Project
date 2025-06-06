#include <iostream>
#include <vector>
#include <random>
#include <bitset>
#include <unordered_map>

static std::vector<uint8_t> gen_permutation(int n){
    size_t m = 1u<<n;                       // 2^n
    std::vector<uint8_t> p(m);
    std::iota(p.begin(), p.end(), 0);       // [0,1,2,...]
    std::mt19937 rng(std::random_device{}());
    std::shuffle(p.begin(), p.end(), rng);  // 可改成自訂 mapping
    return p;
}
static std::unordered_map<uint8_t,uint8_t>
    invert_perm(const std::vector<uint8_t>& p){
        std::unordered_map<uint8_t,uint8_t> inv;
        for(size_t i=0;i<p.size();++i) inv[p[i]]=i;
        return inv;
}

int main(int argc,char* argv[]){
    if(argc!=3){ std::cerr<<"Usage: enc|dec n  < infile > outfile\n";return 1;}
    bool encrypt = std::string(argv[1])=="enc";
    int n = std::stoi(argv[2]);                         // n-bit
    if(n<=0||n>8){ std::cerr<<"n must be 1-8\n";return 1;}

    auto key = gen_permutation(n);                      // 產生 key
    auto inv = invert_perm(key);
    std::bitset<8> block;
    int bitCnt=0, val=0;
    int ch;
    while((ch=std::cin.get())!=EOF){
        block <<=1;
        block.set(0, ch!='0');                          // 讀入 '0'/'1'
        if(++bitCnt==n){
            uint8_t out = encrypt ? key[block.to_ulong()]
                                   : inv[block.to_ulong()];
            // 輸出 n 個 bit
            for(int i=n-1;i>=0;--i) std::cout<<( (out>>i)&1 );
            block.reset(); bitCnt=0;
        }
    }
    if(bitCnt) std::cerr<<"\n[WARN] Incomplete block ignored\n";
    return 0;
}
