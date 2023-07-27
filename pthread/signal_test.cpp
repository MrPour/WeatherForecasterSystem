//
// Created by chenh on 2023/7/27.
// 信号处理
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

void handler(int signal)
{
    printf("接收到了%d信号。\n",signal);
}

void alarmfunc(int signal)
{
    printf("接收到了时钟信号%d。\n",signal);
    //每三秒触发一次
    alarm(3);
}
int main(int argc,char *argv[])
{
    for(int ii = 0;ii<=64;++ii)
        //所有缺省的信号处理方法用handler去替代，信号9不能被【捕获和忽略】
        signal(ii, handler);

    //始终信号处理，宏SIGALRM等价于信号14
    signal(SIGALRM, alarmfunc);
    alarm(3);
    //宏定义忽略信号15的捕捉
    signal(15,SIG_IGN);
    //宏定义信号15的为缺省方式处理
    signal(15,SIG_DFL);

    while(1)
    {
        printf("执行了一次。\n");
        sleep(1);
    }
    return 0;
}
