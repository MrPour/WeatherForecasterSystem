//
// Created by chenhao on 2023/7/30.
//
#include "public.h"

#define MAXNUMP_  1000 //最大进程数
#define SHMKEYP_  0X5095 //共享内存的key
#define SEMKEYP_  0X5095 //信号量的key
/**
 * 心跳信息操作类
 * */
 //进程心跳信息结构体
struct st_pinfo
{
    int pid;       //进程id
    char pname[51];//进程名称，可为空
    int timeout;   //超时时间，单位:s
    time_t atime;  //最后一次心跳的时间，整数表示
};

//进程心跳操作类
class HeartBeatHandler
{
private:
    //共享内存信号量工具
    CSEM m_sem;
    //共享内存的id
    int m_shmid;
    //当前进程在共享内存数组中的位置
    int m_pos;
    //指向共享内存的地址空间的指针
    struct st_pinfo *m_shm;
public:
    HeartBeatHandler();
    //写入本进程的心跳信息
    bool AddPInfo(const int timeout,const char* pname);
    //更新共享内存中本进程的心跳时间
    bool UptATime();
    ~HeartBeatHandler();
};

HeartBeatHandler::HeartBeatHandler()
{
    //构造函数初始化变量
    this->m_shmid = -1;
    this->m_pos = -1;
    this->m_shm = nullptr;
}

bool HeartBeatHandler::AddPInfo(const int timeout, const char *pname) {
    //如果已经找到了位置，不需要再找也不需要再添加
    if(this->m_pos != -1) return true;

    //1、创建/获取共享内存。大小为n*sizeof(struct st_pinfo)
    if((this->m_shmid=shmget(SHMKEYP_,MAXNUMP_* sizeof(struct st_pinfo),0640|IPC_CREAT)) == -1)
    {
        printf("shmget(%x) failed \n",SHMKEYP_); return false;
    }

    //给共享内存加锁，锁的时间越短越好
    //创建信号量
    if(this->m_sem.init(SEMKEYP_) == false)
    {
        printf("m_sem.init(%x) failed \n",SEMKEYP_); return false;
    }
    //2、将获取的共享内存连接到当前进程的地址空间
    //使用指针变量来指向共享内存
    this->m_shm = (st_pinfo*) shmat(this->m_shmid,0,0);//指向首地址，相当于数组指针
    //3、创建当前进程心跳信息的结构体变量，把本进程的信息填入
    struct st_pinfo stPinfo;
    //创建一个空的结构体并赋值
    memset(&stPinfo,0, sizeof(struct st_pinfo));
    stPinfo.pid = getpid();
    STRNCPY(stPinfo.pname, sizeof(stPinfo.pname),pname,50);
    stPinfo.timeout = timeout;
    stPinfo.atime = time(0); //最后有一次心跳，填充当前时间

    //4、在共享内存中查找到一个空闲位置，将该结构体存入
    /**
     * pid是循环使用的，如果有异常退出的进程，其心跳信息不会被清理残留在内存中。
     * 当新进程分配到上述进程的id后，会产生冲突。
     * 存入之前需要提前检查pid是否已经存在,存在则一定是残留进程,使用该位置。
     * */
    //使用残留共享内存位置
    for(int ii=0;ii<MAXNUMP_;++ii)
    {
        if(this->m_shm[ii].pid == stPinfo.pid)
        {
            this->m_pos=ii;
            break;
        }
    }
    this->m_sem.P(); //加锁
    //找到空闲内存位置
    if(this->m_pos == -1)
    {
        for(int ii=0;ii<MAXNUMP_;++ii)
        {
            //数组方式取值
            if((this->m_shm[ii]).pid == 0)
            {
                this->m_pos = ii;
                break;
            }
        }
    }

    //没找到则耗尽，退出
    if(this->m_pos == -1)
    {
        this->m_sem.V(); //解锁
        printf("共享内存空间已用完。\n");
        return false;
    }
    //把当前进程心跳信息存入共享内存数组,m_shm指针指向头部位置，+m_pos则指向空余位置的指针
    memcpy(this->m_shm+this->m_pos,&stPinfo, sizeof(struct st_pinfo));

    this->m_sem.V(); //解锁
    return true;
}

bool HeartBeatHandler::UptATime()
{
    if(this->m_pos == -1) return false;
    this->m_shm[this->m_pos].atime = time(0);
    return true;
}

HeartBeatHandler::~HeartBeatHandler()
{
    //如果空位找到成功才需要删除当前进程的心跳信息
    if(this->m_pos != -1) memset(this->m_shm+this->m_pos,0, sizeof(struct st_pinfo));
    //如果共享内存连接成功才需要分离
    if(this->m_shm != nullptr) shmdt(this->m_shm);
}

int main(int argc,char* argv[])
{
    if(argc < 2)
    {
        printf("Using ./heartbeat_test procname\n");
        return -1;
    }
    HeartBeatHandler handler;
    handler.AddPInfo(30,argv[1]);
    //5、循环体更新共享内存中本进程的心跳时间
    while(true)
    {
        handler.UptATime();
    }
    return 0;
}