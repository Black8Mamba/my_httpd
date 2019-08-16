/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-08-16 12:42:02
 * @LastEditTime: 2019-08-16 14:04:56
 * @LastEditors: Please set LastEditors
 */
#include "epoll.h"

struct epoll_event* events;

int tk_epoll_create(int flags)
{
    int epoll_fd = epoll_create1(flags);
    if (epoll_fd == -1)
        return -1;
    events = (struct epoll_event*)malloc(sizeof(struct epoll_event) * MAXEVENTS);
    return epoll_fd;
}

// 注册新描述符
int tk_epoll_add(int epoll_fd, int fd, 
                tk_http_request_t* request, 
                int events)
{
    struct epoll_event event;
    event.data.ptr = (void*)request;
    event.events = events;
    int ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
    if (ret == -1)
        return -1;

    return 0;
}

// 修改描述符状态
int tk_epoll_mod(int epoll_fd, int fd, 
                tk_http_request_t* request,
                int events)
{
    struct epoll_event event;
    event.data.ptr = (void*)request;
    event.events = events;
    int ret = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event);
    if (ret == -1)
        return -1;
    
    return 0;
}

// 从epoll中删除描述符
int tk_epoll_del(int epoll_fd, int fd,
                tk_http_request_t* request, 
                int events)
{
    struct epoll_event event;
    event.data.ptr = (void*)request;
    event.events = events;
    int ret = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &event);
    if(ret == -1)
        return -1;    

    return 0;
}

// 返回活跃事件数
int tk_eopll_wait(int epoll_fd, struct epoll_event* events,
                int max_events, int timeout)
{
    int ret_count = epoll_wait(epoll_fd, events, max_events, timeout);
    return ret_count;
}

void tk_handle_events(int epoll_fd, int listen_fd,
                    struct epoll_event* events,
                    int events_num,
                    char* path,
                    tk_threadpool_t* tp)
{
    for (int i = 0; i < events.num; ++i) {
        tk_http_request_t *request = (tk_http_request_t*)(events[i].data.ptr);
        int fd = request->fd;

        if (fd == listen_fd) {
            accept_connection(listen_fd, epoll_fd, path);
        } else {
            if ((events[i].events & EPOLLERR) || 
                (events[i].events & EPOLLHUP) ||
                (!(events[i].evnets & EPOLLIN))) {
                    close(fd);
                    continue;
                }

                int rc = threadpool_add(tp, do_request, 
                evnets[i].data.ptr);
        }

    }
}