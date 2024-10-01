#include<stdio.h>
#include<pthread.h>
#define N 10

int cnt;
pthread_mutex_t lock; // 用锁也可以消除并发的不确定性

void *incr(void *arg){
    for(int i = 0; i < 1e5; ++i){
        pthread_mutex_lock(&lock);
        ++cnt;
        pthread_mutex_unlock(&lock);
        // int reg = *&cnt;
        // reg++;

        // if(cnt > reg){
        //     printf("wrong!! diff: %d\n", cnt - reg);
        // }
        // if(cnt < reg - 1){
        //     printf("wrong!! diff: %d\n", cnt - reg);
        // }
        // *&cnt = reg;
        // __sync_fetch_and_add(&cnt, 1); // 原子指令可以消除并发的不确定性
    }
    return NULL;
}



int main(){
    pthread_t pt[N];
    pthread_mutex_init(&lock, NULL);

    for(int i = 0; i < N; ++i){
        pthread_create(&pt[i], NULL, incr, NULL);
    }

    for(int i = 0; i < N; ++i){
        pthread_join(pt[i], NULL);
    }

    printf("cnt: %d\n", cnt);

    pthread_mutex_destroy(&lock);

    return 0;

}