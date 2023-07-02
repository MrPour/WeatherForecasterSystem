//
// Created by chenhao on 2023/7/2.
//
#include "public.h"

int main(int argc,char *argv[])
{
    //inifile outpath logfile
    if(argc!=4)
    {
        printf("Using: ./crtsurfdata inifile outpath logfile\n");
        printf("Example:/project/idc/bin/crtsurfdata /project/idc/ini/stcode.ini /tmp/surfdata /log/idc\n");

        printf("inifile 全国气象站点参数文件名。\n");
        printf("outpath 全国气象站点数据文件存放目录。\n");
        printf("logfile 本程序运行的日志文件名。\n\n");

        return -1;
    }
    return 0;
}