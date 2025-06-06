#include <bits/stdc++.h>
using Mat = std::array<std::array<int,2>,2>;

static int mod=29;

int mod_inv(int a){
    for(int x=1;x<mod;++x) if((a*x)%mod==1) return x;
    throw std::runtime_error("No inverse");
}
Mat mat_mul(const Mat& a,const Mat& b){
    Mat c{}; 
    for(int i=0;i<2;++i)
        for(int j=0;j<2;++j)
            for(int k=0;k<2;++k)
                c[i][j]=(c[i][j]+a[i][k]*b[k][j])%mod;
    return c;
}
std::array<int,2> vec_mul(const Mat& a,const std::array<int,2>& v){
    return { (a[0][0]*v[0]+a[0][1]*v[1])%mod,
             (a[1][0]*v[0]+a[1][1]*v[1])%mod };
}
Mat mat_inv(const Mat& k){
    int det = (k[0][0]*k[1][1]-k[0][1]*k[1][0])%mod;
    if(det<0) det+=mod;
    int invDet = mod_inv(det);
    Mat r{{
        { k[1][1]*invDet%mod, (mod-k[0][1])*invDet%mod },
        { (mod-k[1][0])*invDet%mod, k[0][0]*invDet%mod }
    }};
    return r;
}
int sym2num(char c){
    if(c>='A'&&c<='Z') return c-'A';
    if(c==',') return 26;
    if(c=='.') return 27;
    if(c=='?') return 28;
    return -1;
}
char num2sym(int n){
    if(n<26) return 'A'+n;
    return ",.?"[n-26];
}

int main(int argc,char* argv[]){
    if(argc!=6){ std::cerr<<"Usage: enc|dec a b c d  < infile > outfile\n";return 1;}
    bool enc = std::string(argv[1])=="enc";
    Mat K{{ {std::stoi(argv[2]),std::stoi(argv[3])},
            {std::stoi(argv[4]),std::stoi(argv[5])} }};
    Mat Kinv = mat_inv(K);

    std::vector<int> nums;
    char ch;
    while(std::cin.get(ch)){
        if(ch==' ') continue;              // 空白略過
        int v=sym2num(std::toupper(ch));
        if(v!=-1) nums.push_back(v);
    }
    if(nums.size()%2) nums.push_back(sym2num(','));  // Padding 用逗號

    for(size_t i=0;i<nums.size();i+=2){
        std::array<int,2> blk{nums[i],nums[i+1]};
        auto out = enc ? vec_mul(K,blk) : vec_mul(Kinv,blk);
        std::cout<<num2sym(out[0])<<num2sym(out[1]);
    }
    std::cout<<"\n";
    return 0;
}
