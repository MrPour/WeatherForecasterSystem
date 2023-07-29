//
// Created by ruish on 2023/7/28.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/** 使用多进程的方式，每隔10s执行一次指令。并且不留下僵尸进程
 * ./procctl 5 /usr/bin/ls "/usr/bin/ls" "-lt" "/tmp"
 * 每隔60s自动执行模拟数据生成工作
 * /project/tool/bin/procctl 60 /project/idc/bin/crtsurfdata /project/idc/ini/stcode.ini /tmp/idc/surfdata /log/idc/crtsurfdata.log xml,csv,json
 * */

int main(int argc,char* argv[])
{
    printf("爷爷进程%d\n",getpid());
    //程序运行最少需要2个参数
    if(argc < 3)
    {
        printf("Using:./procctl timetvl program args ...\n");
        printf("Example:/project/tool/bin/procctl 5 /usr/bin/ls -lt /project\n");
        printf("本程序是服务程序的调度程序，周期性启动服务程序或she11脚本。\n");
        printf("timetvl 运行周期，单位:秒。被调度的程序运行结束后，在 timetvl 秒后会被 procctl 重新启动\n");
        printf("program 被调度的程序名，必须使用全路径。\n");
        printf("argvs 被调度的程序的参数。\n");
        printf("注意，本程序不会被ki11杀死，但可以用ki11 -9强行杀死。\n\n\n");

        return -1;
    }

    //程序不希望被打扰，关闭IO，屏蔽所有信号
    for(int ii = 0; ii < 64; ++ii)
    {
        //只有信号9能kill
        signal(ii,SIG_IGN);
        //关闭IO
        close(ii);
    }

    //生成子进程后，父进程退出，程序在后台运行，被init进程托管
    if(fork() != 0) exit(0);
    //启用SIGCHLD信号，采用缺省的方式，通过自己让父进程可以wait子进程退出的状态，
    signal(SIGCHLD,SIG_DFL);
    //使用execv用数组的方式来处理参数不确定的个数
    char* pargv[argc];
    for(int ii = 2;ii<argc;++ii)
    {
        //把第二个参数之后的参数存入新的数组中打包作为入参
        pargv[ii-2] = argv[ii];
    }
    //最后一个参数设为NULL以结束
    pargv[argc-2] = NULL ;

    while(true)
    {
        printf("爸爸进程%d\n",getpid());
        //子进程执行ls操作
        if(fork() == 0)
        {
            printf("孙子进程%d\n",getpid());
//            sleep(20);
            //execl是用参数中指定程序替换了当前进程的正文段、数据段、堆和栈,调用成功，后续的代码已经不会被执行
            //参数一：程序绝对路径  参数二：程序标识符 参数三、四：程序需要传入的参数  参数五：必须以0结束
//            if(argc == 3)
//            {
//                //如果没有参数：
//                execl(argv[2],argv[2],(char*)0);
//            }
//            else
//            {
//                execl(argv[2],argv[2],argv[3],argv[4],(char*)0);
//            }
                execv(argv[2],pargv);
                //如果执行失败，则子进程退出
                exit(0);
        }
        else
        {
            printf("爸爸进程等待阶段%d\n",getpid());
            //父进程阻塞等待子进程执行完毕
            int status;
            wait(&status);
            //隔10s执行下一次
            sleep(atoi(argv[1]));
        }
    }
}