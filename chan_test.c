#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define NBUF 10

struct chan {
    char buffer[NBUF];
    int head;
    int tail;
    int count;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
};

void chan_init(struct chan *ch) {
    memset(ch, 0, sizeof(struct chan));
    pthread_mutex_init(&ch->lock, NULL);
    pthread_cond_init(&ch->not_empty, NULL);
    pthread_cond_init(&ch->not_full, NULL);
}

void chan_destroy(struct chan *ch) {
    pthread_mutex_destroy(&ch->lock);
    pthread_cond_destroy(&ch->not_empty);
    pthread_cond_destroy(&ch->not_full);
}

int chan_send(struct chan *ch, char c) {
    pthread_mutex_lock(&ch->lock);
    while (ch->count == NBUF) {
        pthread_cond_wait(&ch->not_full, &ch->lock);
    }
    ch->buffer[ch->tail] = c;
    ch->tail = (ch->tail + 1) % NBUF;
    ch->count++;
    pthread_cond_signal(&ch->not_empty);
    pthread_mutex_unlock(&ch->lock);
    return 0;
}

int chan_recv(struct chan *ch, char *c) {
    pthread_mutex_lock(&ch->lock);
    while (ch->count == 0) {
        pthread_cond_wait(&ch->not_empty, &ch->lock);
    }
    *c = ch->buffer[ch->head];
    ch->head = (ch->head + 1) % NBUF;
    ch->count--;
    pthread_cond_signal(&ch->not_full);
    pthread_mutex_unlock(&ch->lock);
    return 0;
}

// 以下是线程函数的示例
void *producer(void *arg) {
    struct chan *ch = (struct chan *)arg;
    for (int i = 0; i < 100; ++i) {
        chan_send(ch, 'A' + (i % 26));
    }
    return NULL;
}

void *consumer(void *arg) {
    struct chan *ch = (struct chan *)arg;
    char c;
    for (int i = 0; i < 100; ++i) {
        chan_recv(ch, &c);
        printf("%c ", c);
    }
    printf("\n");
    return NULL;
}

int main() {
    struct chan ch;
    pthread_t prod, cons;

    chan_init(&ch);

    pthread_create(&prod, NULL, producer, &ch);
    pthread_create(&cons, NULL, consumer, &ch);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    chan_destroy(&ch);

    return 0;
}
