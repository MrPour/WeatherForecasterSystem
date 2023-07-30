//
// Created by chenh on 2023/7/27.
//
//父进程中打开的文件描述符也被复制到子进程中。

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
    //文件操作也是副本
    FILE *fp = fopen("/tmp/tmp.txt","w+");
    if(fp == nullptr)
    {
        printf("Could not open file\n");
        return 0;
    }

    fprintf(fp,"hello world!\n");
    fflush(fp);

    int pid = fork();

    if(pid == 0)
    {
        fprintf(fp,"son hello world!\n");

        sleep(5);
    }

    if(pid > 0)
    {
        fprintf(fp,"father hello world!\n");

        sleep(5);
    }

    fclose(fp);
    return 0;
}