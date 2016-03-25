#include <iostream>
#include <ctime>
#include <cstdlib>

using namespace std;

unsigned long long lcm(unsigned long long a, unsigned long long b) {
    unsigned long long m = a, n = b;  
    /* a, b不相等，大数减小数，直到相等为止。*/    
    while (a != b) {   
        if (a > b)  a = a - b;        
        else b = b - a;
    }
   return (m * n / a);
}

int main() {
    int N = 5;
    int periods[N];
    srand((unsigned)time(NULL));
    for(int i = 0; i < N; i++) {
        periods[i] = 1 + (rand() % 200);
    }
    
    int i,j,temp;
    for (i = 0 ; i < N; i++) {
        for (j = 0; j < N - 1 - i; j++) {
            if (periods[j] > periods[j+1]) {
                temp = periods[j];
                periods[j] = periods[j+1];
                periods[j+1] = temp;
            }
        }
    }
    
    
    for(int i = 0; i < N; i++) {
        cout << periods[i] << endl;
    }    

    unsigned long long macro = 1;
    for (i = 0; i < 3; i++) {
        macro = lcm(macro, periods[i]);
    }
    cout << macro << endl;
    return 0;
}