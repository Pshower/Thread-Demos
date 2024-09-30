#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>

int fib(int n){
    if(n < 2){
        return 1;
    }
    return fib(n - 1) + fib(n - 2);
}

void *p_fib(void * arg){
    int *p = arg;
    int *res = malloc(sizeof(int));
    printf("arg: %d\n", *p);
    if(*p < 2){
        *res = 1;
    }else{
        
        // pthread_t pt1;
        pthread_t pt2;
        int arg1 = *p - 1;
        int arg2 = *p - 2;
        // pthread_create(&pt1, NULL, p_fib, (void *) &arg1);
        pthread_create(&pt2, NULL, p_fib, (void *) &arg2);

        // sync
        int *res1 = p_fib(&arg1);
        int *res2;
        // pthread_join(pt1, (void *) &res1);
        pthread_join(pt2, (void *) &res2);

        *res = *res1 + *res2;
        free(res1);
        free(res2);
    }
    return res;
}

int main(){
    pthread_t pthread;
    int arg = 20;

    pthread_create(&pthread, NULL, p_fib, (void *) &arg);
    int *res;

    pthread_join(pthread, (void *) &res); // jion使用**指针
    printf("res: %d\n", *res);
    return 0;
}