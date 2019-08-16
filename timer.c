/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-08-16 14:24:57
 * @LastEditTime: 2019-08-16 15:16:54
 * @LastEditors: Please set LastEditors
 */
#include <sys/time.h>
#include "timer.h"

tk_pq_t tk_timer;
size_t tk_current_msec;

int timer_comp(void* ti, void* tj)
{
    tk_timer_t* timeri = (tk_timer_t*)ti;
    tk_timer_t* timerj = (tk_timer_t*)tj;
    return (timeri->key < timerj->key) ? 1 : 0;
}

static void tk_time_update()
{
    // 获取当前时间
    struct timeval tv;
    int rc = gettimeofday(&tv, NULL);
    tk_current_msec = ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

int tk_timer_init()
{
    // 建立连接后立即初始化
    // 初始优先队列大小TK_PQ_DEFAULT_SIZE = 10
    int rc = tk_pq_init(&tk_timer, timer_comp, TK_PQ_DEFAULT_SIZE);

    // 更新当前时间
    tk_time_update();
    return 0;
}

int tk_find_timer()
{
    int time;
    while(!tk_pq_is_empty(&tk_timer)) {
        tk_time_update();
        tk_timer_t timer_node = (tk_timer_t*)tk_pq_min(&tk_timer);
        if (timer_node->deleted) {
            int rc = tk_pq_delmin(&tk_timer);
            free(timer_node);
            continue;
        }

        time = (int)(timer_node->key - tk_current_msec);//???
        time = (time > 0) ? time : 0;
        break;
    }

    return time;
}

void tk_handle_expire_timers()
{
    while(!tk_pq_is_empty(&tk_timer)) {
        tk_time_update();
        tk_timer_t* timer_node = (tk_timer_t*)tk_pq_min(&tk_timer);

        if(timer_node->deleted){
            int rc = tk_pq_delmin(&tk_timer); 
            free(timer_node);
            continue;
        }

        if (timer_node->key > tk_current_msec) {
            return ;
        }

        if (timer_node->handler) {
            timer_node->handler(timer_node->request);
        }

        int rc = tk_pq_delmin(&tk_timer);
        free(timer_node);
    }
}

void tk_add_timer(tk_http_request_t* request, size_t timeout,
                  timer_handler_pt handler)
{
    tk_time_update();
    tk_timer_t* timer_node = (tk_timer_t*)malloc(sizeof(tk_timer_t));
    request->timer = timer_node;
    timer_node->key = tk_current_msec + timeout;
    timer_node->deleted = 0;
    timer_node->handler = handler;
    timer_node->request = request;
    int rc = tk_pq_insert(&tk_timer, timer_node);
}

void tk_del_timer(tk_http_request_t* request)
{
    tk_time_update();
    tk_timer_t* timer_node = request->timer;
    timer_node->deleted = 1;
}