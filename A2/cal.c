#include <stdio.h>
#include <sys/types.h>

int main(){
    pid_t x = getpid();
    printf("%u", x);
    int a, b;
    scanf("%d %d", &a, &b);
    printf("%d\n", a+b);
}