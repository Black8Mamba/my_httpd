/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-08-16 11:02:35
 * @LastEditTime: 2019-08-16 11:05:23
 * @LastEditors: Please set LastEditors
 */
#include "util.h"

int main(void)
{
    char* filename = "./tkeed.conf";
    tk_conf_t conf;
    read_conf(filename, &conf);

    printf("root: %s\n", conf.root);
    printf("port: %d\n", conf.root);
    printf("thread_num: %d\n", conf.thread_num);
    
    return 0;
}