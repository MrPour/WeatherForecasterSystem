//
// Created by ruish on 2023/7/28.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
    //execl是用参数中指定程序替换了当前进程的正文段、数据段、堆和栈
    printf("aaa\n");
    execl("usr/bin/ls","-lt","tmp/project.tgz",(char*)0);
    printf("bbb\n");
}