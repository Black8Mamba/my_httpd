/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-08-15 19:59:04
 * @LastEditTime: 2019-08-15 21:57:39
 * @LastEditors: Please set LastEditors
 */
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>

typedef struct tk_task {
    void (*func) (void*);
    void *arg;
    struct tk_task *next; //任务链表
} tk_task_t;

typedef struct threadpool {
    pthread_mutex_t mutex;
    pthread_cont_t cond;
    pthread_t *threads; //线程数组
    tk_task_t *head;
    int thread_count;
    int queue_size;
    int shutdown;
    int started;
}tk_threadpool_t;

typedef enum{
    tk_tp_invalid = -1,
    tk_tp_lock_fail = -2,
    tk_tp_already_shutdown = -3,
    tk_tp_cond_broadcast = -4,
    tk_tp_thread_fail = -5,
}tk_threadpool_error_t;

typedef enum {
    immediate_shutdown = 1;
    graceful_shutdown = 2;
}tk_threadpool_sd_t;

tk_threadpool_t* threadpool_init(int thread_num);
int threadpool_add(tk_threadpool_t* pool, 
                    void (*func)(void*), 
                    void* arg);
int threadpool_destroy(tk_threadpool_t* pool, int gracegul);

#endif