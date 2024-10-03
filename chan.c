#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "stdlib.h"
#include "pthread.h"

#define N_SERVER 2
#define N_CLIENT 10
#define N_BUF 10
#define N_TIMES 1000

struct chan {
    char *s;

    pthread_mutex_t lock;
    pthread_cond_t cond_fill;
    pthread_cond_t cond_empty;
};

int chan_create(struct chan *ch, int n) {
    ch->s = malloc(n);
    pthread_mutex_init(&ch->lock, NULL);
    pthread_cond_init(&ch->cond_fill, NULL);
    pthread_cond_init(&ch->cond_empty, NULL);
    if (memset(ch->s, ' ', n) < 0) {
        assert(0);
    }
    return 0;
}

int chan_send(struct chan *ch, char c) {
    // todo
    *ch->s = c;
    pthread_mutex_lock(&ch->lock);
    pthread_cond_signal(&ch->cond_fill);
    pthread_cond_wait(&ch->cond_empty, &ch->lock);
    pthread_mutex_unlock(&ch->lock);
    return 0;
}

int chan_recv(struct chan *ch, char *c) {
    // todo
    pthread_mutex_lock(&ch->lock);
    pthread_cond_wait(&ch->cond_fill, &ch->lock);
    *c = *ch->s;
    pthread_cond_signal(&ch->cond_empty);
    pthread_mutex_unlock(&ch->lock);
    return 0;
}

struct server {
    char v[65536];
    char *p;
    struct chan *ch;
    int killed;
    int serv_no;
    pthread_t pthread;
};

struct client {
    struct chan *ch_lst;
    int n_serv;
    int cli_no;
    pthread_t pthread;
};

int server_create(struct server *serv, struct chan *ch, int server_no) {
    serv->ch = ch;
    serv->p = &serv->v[0];
    serv->serv_no = server_no;
    serv->killed = 0;
    return 0;
}

int client_create(struct client *cli, struct chan *ch_lst, int n_serv, int cli_no) {
    cli->ch_lst = ch_lst;
    cli->n_serv = n_serv;
    cli->cli_no = cli_no;
    return 0;
}

void *client_start(void *arg) {
    struct client *cli = arg;
    for (int i = 0; i < N_TIMES; ++i) {
        for (int j = 0; j < cli->n_serv; ++j) {
            chan_send(cli->ch_lst + j, '0' + cli->cli_no);
        }
    }
    return NULL;
}

void *server_start(void *arg) {
    struct server *serv = arg;
    while (serv->killed == 0) {
        char c;
        chan_recv(serv->ch, &c);
        *serv->p = c;
        *(serv->p + 1) = '\0';
        serv->p++;
    }
    return NULL;
}

int mymin(int a, int b, int c){
    b = b<c ? b:c;
    return a<b ? a:b;
}

// leetcode72. 编辑距离
int minDistance(char *word1, char *word2) {
    // todo impl it
    int len1 = strlen(word1);
    int len2 = strlen(word2);
    if(!len1 * len2){
        return len1 + len2;
    }

    int dp[len1 + 1][len2 + 1];
    for(int i = 0; i < len1 + 1; ++i){
        dp[i][0] = i;
    }
    for(int j = 0; j < len2 + 1; ++j){
        dp[0][j] = j;
    }

    for(int i = 1; i < len1 + 1; ++i){
        for(int j = 1; j < len2 + 1; ++j){
            if(word1[i - 1] == word2[j - 1]){
                dp[i][j] = dp[i-1][j-1];
            }
            else{
                dp[i][j] = mymin(dp[i][j-1], dp[i-1][j], dp[i-1][j-1]) + 1;
            }
        }
    }
    return dp[len1][len2];
}

int main() {
    // 启动所有服务端
    struct chan ch_lst[N_SERVER];
    struct server servers[N_SERVER];
    for (int i = 0; i < N_SERVER;
         ++i) {
        chan_create(&ch_lst[i], N_BUF);
        server_create(&servers[i], &ch_lst[i], i);
        pthread_create(&servers[i].pthread, NULL, server_start, (void *) &servers[i]);
    }

    // 启动所有客户端
    struct client clients[N_CLIENT];
    for (int i = 0; i < N_CLIENT; ++i) {
        client_create(&clients[i], &ch_lst[0], N_SERVER, i);
        pthread_create(&clients[i].pthread, NULL, client_start, (void *) &clients[i]);
    }

    for (int i = 0; i < N_CLIENT; ++i) {
        pthread_join(clients[i].pthread, NULL);
    }
    printf("servers[0].v: %s\n", servers[0].v);
    printf("-------\n");
    printf("servers[1].v%s\n", servers[1].v);

    int cnt = 0;
    for (int i = 0; i < strlen(servers[0].v); ++i) {
        if (servers[0].v[i] != servers[1].v[i]) {
            cnt++;
        }
    }
    printf("不一样的字符数: %d\n", cnt);

    printf("二者的编辑距离: %d\n", minDistance(&servers[0].v[0], &servers[1].v[0]));

    return 0;
}