#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

int bIsDone = 0;
int bCouldPrint = 0;
pthread_cond_t cond_b;
pthread_cond_t cond_c;
pthread_mutex_t lock;

// int idx = 0;

void *f1(void *arg){
    // todo
    // while(1){
    //     if(idx == 1){
    printf("f1 init\n");
    pthread_mutex_lock(&lock);
    while(bCouldPrint == 0){
    
        printf("wait for signal\n");
        pthread_cond_wait(&cond_b, &lock);// will unlock the mutex
    }
    printf("B\n");
    bIsDone = 1;
    
    pthread_cond_signal(&cond_c);
    pthread_mutex_unlock(&lock);
    //         idx = 2;
    //         break;
    //     }
    // }
    return NULL;
}

void *f2(void *arg){
    printf("f2 init\n");
    // todo
    printf("A\n");
    bCouldPrint = 1;
    pthread_mutex_lock(&lock);
    pthread_cond_signal(&cond_b);
    // pthread_mutex_unlock(&lock);
    // idx = 1;
    // while(1){
    //     if(idx == 2){
    
    // pthread_mutex_lock(&lock);
    // usleep(1000000);
    while(bIsDone == 0){
        pthread_cond_wait(&cond_c, &lock);
    }
    
    printf("C\n");
    pthread_mutex_unlock(&lock);
    //         break;
    //     }
    // }
    
    return NULL;
}

void init(){
    // todo
    pthread_cond_init(&cond_b, NULL);
    pthread_cond_init(&cond_c, NULL);
    pthread_mutex_init(&lock, NULL);
}

int main(){
    init();
    pthread_t pt1;
    pthread_t pt2;
    pthread_create(&pt1, NULL, f1, NULL);
    pthread_create(&pt2, NULL, f2, NULL);
    pthread_join(pt1, NULL);
    pthread_join(pt2, NULL);
    return 0;
}