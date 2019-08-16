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
    
    return 0;
}