/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-08-16 15:59:29
 * @LastEditTime: 2019-08-16 16:34:34
 * @LastEditors: Please set LastEditors
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "rio.h"

//fd 读到 userbuf
ssize_t rio_readn(int fd, void *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nread;
    char *bufp = (char*)usrbuf;
    while(nleft > 0) {
        if ((nread = read(fd, bufp, nleft)) < 0) {
            if (errno == EINTR)
                nread = 0;
            else
                return -1;
        } else if (nread == 0)
            break;
        nleft -= nread;
        bufp += nread;
    }

    return (n-nleft);
}

//userbuf 写到fd
ssize_t rio_writen(int fd, void *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nwriten;
    char *bufp = (char*)usrbuf;

    while(nleft > 0) {
        if ((nwriten = write(fd, bufp, nleft)) <= 0) {
            if (errno == EINTR)
                nwriten = 0;
            else 
                return -1;
        }
        nleft -= nwritten;
        bufp += nwritten;
    }

    return n;
}

//从rio读到userbuf
static ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n)
{
    size_t cnt;
    while(rp->rio_cnt <= 0){
        rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));
        if (rp->rio_cnt < 0){
            if(errno == EAGAIN){
                return -EAGAIN;
            }
            if(errno != EINTR){
                return -1;
            }
        }
        else if(rp->rio_cnt == 0)
            return 0;
        else
            rp->rio_bufptr = rp->rio_buf;
    }
    cnt = n;
    if(rp->rio_cnt < (ssize_t)n)
        cnt = rp->rio_cnt;
    memcpy(usrbuf, rp->rio_bufptr, cnt);
    rp->rio_bufptr += cnt;
    rp->rio_cnt -= cnt;
    return cnt;
}

//rio初始化
void rio_readinitb(rio_t *rp, int fd)
{
    rp->rio_fd = fd;
    rp->rio_cnt = 0;
        //rp->rio_buf == NULL?
    rp->rio_bufptr = rp->rio_buf;
}

ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nread;
    char *bufp = (char *)usrbuf;
    while (nleft > 0){
        if((nread = rio_read(rp, bufp, nleft)) < 0){
            if(errno == EINTR)
                nread = 0;
            else
                return -1;
        }
        else if(nread == 0)
            break;
        nleft -= nread;
        bufp += nread;
    }
    return (n - nleft);
}

ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen)
{
    size_t n;
    ssize_t rc;
    char c, *bufp = (char *)usrbuf;
    for(n = 1; n < maxlen; n++){
        if((rc = rio_read(rp, &c, 1)) == 1){
            *bufp++ = c;
            if(c == '\n')
            break;
        }
        else if(rc == 0){
            if (n == 1){
                return 0;
            }
            else
                break;
        }
        else if(rc == -EAGAIN){
            return rc;
        }
        else{
            return -1;
        }
    }
    *bufp = 0;
    return n;    
}