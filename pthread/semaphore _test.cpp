//
// Created by ruish on 2023/7/29.
//
#include "public.h"

/** 二值信号量实验
 *  * ipcs -s 查看信号量数组
 *  * ipcrm sem semid 删除信号量
 */

//信号量 -- 用于给共享内存加锁
CSEM sem;
struct st_pid
{
    //进程编号
    int pid;

    //进程名称
    char name[51];
};

int main(int argc,char* argv[])
{
    if(argc < 2)
    {
        printf("Using ./shared_memory procname\n");
        return -1;
    }

    //共享内存id
    int shmid;

    //获取或者创建共享内存，键值使用十六进制--0x5005 0640--权限 IPC_CREAT--内存策略（存在则取出，不存在则创建）
    if((shmid = shmget(0x5005,sizeof(st_pid),0640|IPC_CREAT)) == -1)
    {
        printf("shmget(0x5005) failed \n");
        return -1;
    }

    //如果信号量已存在，获取信号量；否则创建信号量，并初始化
    if(sem.init(0x5005) == false)
    {
        printf("sem.init(0x5005) failed \n");
        return -1;
    }
    //使用指针变量来指向共享内存
    st_pid* stpid = nullptr;
    //连接共享内存到当前进程的地址空间，传入shmid、0和0，返回值为指针void*
    if((stpid = (st_pid*)shmat(shmid,0,0)) == (void*)-1)
    {
        printf("shmat failed \n");
        return -1;
    }

    printf("加锁前：time=%d,val=%d\n",time(0),sem.value());
    sem.P(); //给共享变量加锁
    printf("加锁后：time=%d,val=%d\n",time(0),sem.value());

    printf("pid=%d,name=%s\n",stpid->pid,stpid->name);
    stpid->pid = getpid();

    sleep(10);//假设使用了10s
    printf("解锁前：time=%d,val=%d\n",time(0),sem.value());
    strcpy(stpid->name,argv[1]);
    printf("pid=%d,name=%s\n",stpid->pid,stpid->name);
    sem.V(); //给共享变量解锁,val+1 同时 唤醒等待中的进程 val-1  第一个进程的V和第二个进程的P，整个过程是原子操作
    printf("解锁后：time=%d,val=%d\n",time(0),sem.value());


    //分离共享内存
    shmdt(stpid);

    //shmctl 操作共享内存段 IPC_RMID 删除共享内存段
//    if(shmctl(shmid,IPC_RMID,0)==-1)
//    {
//        printf("shmctl failed \n");
//        return -1;
//    }
    return 0;
}