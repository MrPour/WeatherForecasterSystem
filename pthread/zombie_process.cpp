//
// Created by chenh on 2023/7/27.
//
//如果父进程先退出，子进程会成为孤儿进程，将被1号进程收养。
//如果子进程先退出，内核向父进程发送SIGCHLD信号，如果父进程不处理这个信号，子进程会成为僵尸进程。

//僵尸进程的危害
//如果子进程在父进程之前终止，内核为每个子进程保留了一个数据结构，
// 包括进程编号、终止状态和使用cpu时间等父进程如果处理了子进程退出的信息，
// 内核就会释放这个数据结构，如果父进程没有处理子进程退出的信息，内核就不会释放这个数据结构，
// 子进程进程编号就会一直被占用，但是系统可用的进程号是有限的，如果大量的产生僵死进程，
// 将因为没有可用的进程号而导致系统不能产生新的进程，这就是僵尸进程的危害。
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/wait.h>

void func(int signal)
{
    //进程一旦调用了 wait，就立即阻塞自己，由
    // wait自动分析是否当前进程的某个子进程已经退出，
    // 如果让它找到了这样一个已经变成僵尸的子进程，wait 就会收集这个子进程的信息， 并把它彻底销毁后返回；
    // 如果没有找到这样一个子进程，wait就会一直阻塞在这里，直到有一个出现为止
    int status;
    wait(&status);
}

int main()
{
    //方法一：父进程处理SIGCHLD信号，就不会产生僵尸进程
//    signal(SIGCHLD,SIG_IGN);
    //方法二：子进程退出，触发异步非阻塞函数func
    signal(SIGCHLD,func);

    int pid = fork();
    if(pid == 0)
    {
        //子进程先退出
        sleep(5);
    }

    if(pid > 0)
    {
        sleep(20);
    }
    return 0;
}