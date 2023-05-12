#include <stdio.h>
#include <limits.h>
#include <stdbool.h>

int main(){
    unsigned long long a = 0xa521147fde45f45a;
    unsigned long long b = 0xf5554ed4f4522365;
    unsigned long long c;
    int im = 0x5;
    bool carry = 1;

    printf("%llx\n", a + (int)carry);

    if(a>=b){
        if((ULLONG_MAX-a)<b){
            printf("yes 1\n");
        }
    } else {
        if((ULLONG_MAX-b)<a){
            printf("yes 2\n");
        }
    }

    printf("resultat = %llu\n", a+b);
    printf("%llu\n", ULLONG_MAX);
}