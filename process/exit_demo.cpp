//
// Created by chenh on 2023/7/27.
// 程序退出的善后处理
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

void EXIT(int signal)
{
    printf("接收到了信号%d,程序即将退出。\n",signal);
    exit(0);
}

int main(int argc,char *argv[])
{
    for(int ii = 0;ii<=64;++ii)
        // 忽略全部的信号，防止程序被信号异常中止。
        signal(ii, SIG_IGN);

    //ctrl+c，等价于信号2
    signal(SIGINT, EXIT);
    //kill|killall，等价于信号15
    signal(SIGTERM, EXIT);

    while(1)
    {
        printf("执行了一次。\n");
        sleep(1);
    }
    return 0;
}