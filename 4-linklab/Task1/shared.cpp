#include "shared.h"
#include <cstdio>
#include <cstring>

void printDebug(){
    printf("task1 测试模式开启\n");
    printf("task1\n");
}

void print(){
    printf("task1\n");
}

void doSomeCalc(int t){
    int a = 1,b = 1;
    for (int i = 0;i < t;++i){
        int c = a + b;
        a = b;
        b = c;

        # ifdef DEBUG
        printf("%d\n",b);
        # endif
    }
    printf("斐波拉契数列第%d项为:%d\n",t,b);
}