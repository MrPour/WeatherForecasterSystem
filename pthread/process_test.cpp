//
// Created by chenh on 2023/7/27.
//
// fork函数
// 一个现有的进程调用函数fork创建一个新的进程
// 子进程和父进程继续执行fork函数后的代码，顺序不确定
// fork函数调用一次，返回两次：子进程返回0，父进程返回子进程的进程ID。
// 子进程是父进程的副本
// 子进程获得了父进程的数据空间、堆和栈的副本，不是共享
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
    int pid = fork();
    printf("pid=%d\n",pid);
    sleep(10);
    if(pid == 0)
    {
        printf("this is son=%d\n",getpid());
        sleep(5);
    }

    if(pid > 0)
    {
        printf("this is father=%d\n",getpid());
        sleep(5);
    }
}