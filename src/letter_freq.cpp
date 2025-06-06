#include <iostream>
#include <iomanip>
#include <array>
#include <cctype>

int main() {
    std::array<size_t,26> cnt{};            // A-Z 的計數
    char ch; size_t total = 0;
    while(std::cin.get(ch)){                // 從 stdin 讀全文
        if(std::isalpha(static_cast<unsigned char>(ch))){
            cnt[std::toupper(ch)-'A']++;
            ++total;
        }
    }
    if(total==0){ std::cerr<<"No letters!\n"; return 1; }

    std::cout<<"Letter  Count  Frequency(%)\n";
    for(int i=0;i<26;++i){
        double f = 100.0*cnt[i]/total;
        std::cout<<static_cast<char>('A'+i)<<std::setw(9)<<cnt[i]
                 <<std::setw(12)<<std::fixed<<std::setprecision(2)<<f<<"\n";
    }
    return 0;
}
