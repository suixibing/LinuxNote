// 体会条件变量的基本使用以及基本操作接口
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex;
pthread_cond_t cond_gourmet, cond_cheif;
int have_delicacy = 0; // 0-表示没有美食
void* gourmet(void *argv)
{
    while (1) {
        pthread_mutex_lock(&mutex);
        while (have_delicacy == 0) {
            // 没有美食需要等待，但是需要在等待前先解锁
            // 并且这个解锁操作和陷入休眠的操作必须是原子操作
            // 解锁->休眠->被唤醒之后重新加锁
            pthread_cond_wait(&cond_gourmet, &mutex);
        }
        printf("Readlly delicious\n");
        have_delicacy--;
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond_cheif);
    }
    return NULL;
}

void* chief(void *argv)
{
    while (1) {
        pthread_mutex_lock(&mutex);
        while (have_delicacy == 1) {
            // 若做好了美食但没人吃，也陷入等待
            pthread_cond_wait(&cond_cheif, &mutex);
        }
        printf("I made a bowl of Buddha jumping over the wall\n");
        have_delicacy++;
        pthread_mutex_unlock(&mutex);
        // 做出美食之后，应该唤醒等待的人
        pthread_cond_signal(&cond_gourmet); // 唤醒等待的人
    }
    return NULL;
}

int main()
{
    pthread_t gourmet_tid, chief_tid;
    int i, ret;

    // 初始化条件变量
    pthread_cond_init(&cond_gourmet, NULL);
    pthread_cond_init(&cond_cheif, NULL);
    pthread_mutex_init(&mutex, NULL);
    for (i = 0; i < 4; i++) {
        ret = pthread_create(&gourmet_tid, NULL, gourmet, NULL);
        if (ret != 0) {
            printf("pthread create error\n");
            return -1;
        }
    }
    for (i = 0; i < 4; i++) {
        ret = pthread_create(&chief_tid, NULL, chief, NULL);
        if (ret != 0) {
            printf("pthread create error\n");
            return -1;
        }
    }
    pthread_join(gourmet_tid, NULL);
    pthread_join(chief_tid, NULL);
    pthread_cond_destroy(&cond_gourmet); // 销毁条件变量
    pthread_cond_destroy(&cond_cheif);
    pthread_mutex_destroy(&mutex); // 销毁互斥锁
    return 0;
}
