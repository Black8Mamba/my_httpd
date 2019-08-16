/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-08-16 17:11:48
 * @LastEditTime: 2019-08-16 19:35:27
 * @LastEditors: Please set LastEditors
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "http_request.h"

static int tk_http_process_ignore(tk_http_request_t* request, 
                                  tk_http_out_t* out, 
                                  char* data, 
                                  int len);
static int tk_http_process_connection(tk_http_request_t* request, 
                                      tk_http_out_t* out, 
                                      char* data, 
                                      int len);
static int tk_http_process_if_modified_since(tk_http_request_t* request, 
                                            tk_http_out_t* out, 
                                            char* data, 
                                            int len);

tk_http_header_handle_t tk_http_headers_in[] = {
    {"Host", tk_http_process_ignore},
    {"Connection", tk_http_process_connection},
    {"If-Modified-Since", tk_http_process_if_modified_since},
    {"", tk_http_process_ignore}
};

static int tk_http_process_ignore(tk_http_request_t* request, 
                                  tk_http_out_t* out, 
                                  char* data, 
                                  int len)
{
    return 0;
}


static int tk_http_process_connection(tk_http_request_t* request, 
                                      tk_http_out_t* out, 
                                      char* data, 
                                      int len)
{
    if (strncasecmp("keep-alive", data, len) == 0) {
        out->keep_alive = 1;
    }

    return 0;
}

static int tk_http_process_if_modified_since(tk_http_request_t* request, 
                                            tk_http_out_t* out, 
                                            char* data, 
                                            int len)
{
    struct tm tm;
    // 转换data格式为GMT格式
    if(strptime(data, "%a, %d %b %Y %H:%M:%S GMT", &tm) == (char*)NULL){
        return 0;
    }

    time_t client_time = mktime(&tm);
    double time_diff = difftime(out->mtime, client_time);
    if (fabs(time_diff) < 1e-6) {
        out->modified = 0;
        out->status = TK_HTTP_NOT_MODIFIED;
    }

    return 0;
}

// 初始化请求数据结构
int tk_init_request_t(tk_http_request_t* request, 
                      int fd, 
                      int epoll_fd, 
                      char* path)
{
    request->fd = fd;
    request->epoll_fd = epoll_fd;
    request->pos = 0;
    request->last = 0;
    request->state = 0;
    request->root = path;
    INIT_LIST_HEAD(&(request->list)); 

    return 0;   
}

int tk_init_out_t(tk_http_out_t* out, int fd)
{
    out->fd = fd;
    out->keep_alive = 1;
    out->modified = 1;
    out->status = 200;
    
    return 0;
}


// 根据状态码返回shortmsg
const char* get_shortmsg_from_status_code(int status_code)
{
    if (status_code == TK_HTTP_OK) {
        return "OK";
    } 

    if (status_code == TK_HTTP_NOT_MODIFIED) {
        return "Not Modified";
    }

    if (status_code == TK_HTTP_NOT_FOUND) {
        return "Not Found";
    }

    return "Unknown";
}

// 关闭描述符，释放请求数据结构
int tk_http_close_conn(tk_http_request_t* request)
{
    close(request->fd);
    free(request);
    return 0;
}