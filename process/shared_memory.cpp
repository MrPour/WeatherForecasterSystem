//
// Created by ruish on 2023/7/29.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
/**共享内存实验
 * ipcs -m 查看共享内存 nattch是指当前有多少个进程连接到该共享存储区
 * ipcrm -m shmid 删除共享内存

 * key -- 键值 shmid -- 共享内存标识
 */

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

    //使用指针变量来指向共享内存
    st_pid* stpid = nullptr;
    //连接共享内存，传入shmid、0和0，返回值为指针void*
    if((stpid = (st_pid*)shmat(shmid,0,0)) == (void*)-1)
    {
        printf("shmat failed \n");
        return -1;
    }

    printf("pid=%d,name=%s\n",stpid->pid,stpid->name);
    stpid->pid = getpid();
    strcpy(stpid->name,argv[1]);
    printf("pid=%d,name=%s\n",stpid->pid,stpid->name);

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
