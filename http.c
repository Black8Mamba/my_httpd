/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-08-16 21:58:31
 * @LastEditTime: 2019-08-16 22:22:28
 * @LastEditors: Please set LastEditors
 */

#include <errno.h>
#include "http.h"

static const char* get_file_type(const char *type);
static void parse_uri(char *uri, int length, 
                        char *filename, char *query);
static void do_error(int fd, char *cause, char *err_num, 
                        char *short_msg, char *long_msg);
static void serve_static(int fd, char *filename, 
                        size_t filesize, tk_http_out_t *out);

static char *ROOT = NULL;

mime_type_t tkeed_mime[] = 
{
    {".html", "text/html"},
    {".xml", "text/xml"},
    {".xhtml", "application/xhtml+xml"},
    {".txt", "text/plain"},
    {".rtf", "application/rtf"},
    {".pdf", "application/pdf"},
    {".word", "application/msword"},
    {".png", "image/png"},
    {".gif", "image/gif"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".au", "audio/basic"},
    {".mpeg", "video/mpeg"},
    {".mpg", "video/mpeg"},
    {".avi", "video/x-msvideo"},
    {".gz", "application/x-gzip"},
    {".tar", "application/x-tar"},
    {".css", "text/css"},
    {NULL ,"text/plain"}
};

static void parse_uri(char *uri, int length, 
                        char *filename, char *query)
{
    uri[length] = '\0';
    char *delim_pos = strchr(uri, '?');
    int filename_length = (delim_pos != NULL) ? ((int)(delim_pos - uri)) : length;
    strcpy(filename, ROOT);
    strncat(filename, uri, filename_length);
    char* last_comp = strrchr(filename, '/');
    char* last_dot = strrchr(last_comp, '.');
    if ((last_dot == NULL) && (filename[strlen(filename) - 1] != '/')) {
        strcat(filename, "/");
    }

     // 默认请求index.html
    if(filename[strlen(filename) - 1] == '/'){
        strcat(filename, "index.html");
    }
    return;    
}


const char* get_file_type(const char *type)
{
    for (int i = 0; tkeed_mime[i].type != NULL; ++i) {
        if (strcmp(type, tkeed_mime[i].type) == 0)
            return tkeed_mime[i].value;
    }

    return "text/plain";
}

static void do_error(int fd, char *cause, char *err_num, 
                        char *short_msg, char *long_msg)
{
    // 响应头缓冲（512字节）和数据缓冲（8192字节）
    char header[MAXLINE];
    char body[MAXLINE];

    // 用log_msg和cause字符串填充错误响应体
    sprintf(body, "<html><title>TKeed Error<title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\n", body);
    sprintf(body, "%s%s : %s\n", body, err_num, short_msg);
    sprintf(body, "%s<p>%s : %s\n</p>", body, long_msg, cause);
    sprintf(body, "%s<hr><em>TKeed web server</em>\n</body></html>", body);    

    // 返回错误码，组织错误响应头
    sprintf(header, "HTTP/1.1 %s %s\r\n", err_num, short_msg);
    sprintf(header, "%sServer: TKeed\r\n", header);
    sprintf(header, "%sContent-type: text/html\r\n", header);
    sprintf(header, "%sConnection: close\r\n", header);
    sprintf(header, "%sContent-length: %d\r\n\r\n", header, (int)strlen(body));

    // 发送错误信息
    rio_writen(fd, header, strlen(header));
    rio_writen(fd, body, strlen(body));   
}