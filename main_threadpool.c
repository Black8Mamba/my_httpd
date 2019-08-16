#include "threadpool.h"
#include <stdio.h>

void echo(void* arg)
{
  printf("hello:%d\n", (int)arg);
}

int main(void)
{
    int thread_num = 10;
    tk_threadpool_t *pool = NULL;
    pool = threadpool_init(thread_num);
    if (pool != NULL) {
      for (int i = 0; i < 100; ++i) {
          threadpool_add(pool, echo, (void*)i);
      }
    }

    threadpool_destroy(pool, 2);
    return 0;
}