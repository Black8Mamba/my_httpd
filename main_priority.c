/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-08-16 11:36:10
 * @LastEditTime: 2019-08-16 11:44:24
 * @LastEditors: Please set LastEditors
 */
#include "priority_queue.h"
#include <stdio.h>

int compare(void* pi, void* pj)
{
    if (*(int*)pi > *(int*)pj)
        return 1;
    else
        return 0;  
}

int main(void)
{
    tk_pq_t pq;
    tk_pq_init(&pq, compare, 10);
    printf("empty: %d\n", tk_pq_is_empty(&pq));
    printf("size: %d\n", tk_pq_size(&pq));
    char buf[] = {1,2,3,4,5,6,7,8,9,10};
    for (int i = 0; i < 10; ++i)
        tk_pq_insert(&pq, (void*)&buf[i]);
    printf("empty: %d\n", tk_pq_is_empty(&pq));
    printf("size: %d\n", tk_pq_size(&pq));
    while(!tk_pq_is_empty(&pq)) {
        printf("%d\n", *(int*)tk_pq_min(&pq));
        tk_pq_delmin(&pq);
    }
    printf("empty: %d\n", tk_pq_is_empty(&pq));
    printf("size: %d\n", tk_pq_size(&pq));
        
    return 0;
}