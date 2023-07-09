//
// Created by chenhao on 2023/7/2.
//
#include <memory>
#include "public.h"

struct St_stationInfo{
    //省
    char provname[31];
    //站点id
    char obtid[11];
    //站点名
    char obtname[31];
    //纬度
    double lat;
    //经度
    double lon;
    //海拔
    double height;
};

//创建容器 -- 全局变量
vector<St_stationInfo> v_stationInfo;
//将站点信息存入vector
bool LoadStationInfo(const char *inifile);
// 全国气象站点分钟观测数据结构
struct St_surfdataInfo
{
    char obtid[11];      // 站点代码。
    char ddatetime[21];  // 数据时间：格式yyyymmddhh24miss
    int  t;              // 气温：单位，0.1摄氏度。
    int  p;              // 气压：0.1百帕。
    int  u;              // 相对湿度，0-100之间的值。
    int  wd;             // 风向，0-360之间的值。
    int  wf;             // 风速：单位0.1m/s
    int  r;              // 降雨量：0.1mm。
    int  vis;            // 能见度：0.1米。
};
//创建容器 -- 全局变量
vector<St_surfdataInfo> v_surfdataInfo;

// 模拟生成全国气象站点分钟观测数据，存放在vsurfdata容器中。
void MockSurfData();

//日志文件声明为全局变量 -- 全局变量
CLogFile logFile;
int main(int argc,char *argv[])
{
    //inifile outpath logfile
    if(argc!=4)
    {
        cout<<"Using: ./crtsurfdata inifile outpath logfile"<<endl;
        cout<<"Example:/project/idc/bin/crtsurfdata /project/idc/ini/stcode.ini /tmp/surfdata /log/idc/crtsurfdata.log"<<endl;

        cout<<"inifile 全国气象站点参数文件名。"<<endl;
        cout<<"outpath 全国气象站点数据文件存放目录。"<<endl;
        cout<<"logfile 本程序运行的日志文件名。"<<endl;

        return -1;
    }

    if(logFile.Open(argv[3]) == false)
    {
        printf("logFile.Open(%s) failed.\n",argv[3]);
        return -1;
    }

    logFile.Write("crtsurfdata 开始运行。\n");
    if(LoadStationInfo(argv[1])== false) return -1;
    logFile.Write("crtsurfdata 开始结束。\n");
    return 0;
}

bool LoadStationInfo(const char *inifile)
{
    //文件操作类
    CFile file;
    //打开站点参数文件
    if(file.Open(inifile,"r")== false)
    {
        logFile.Write("File.Open(%s) failed.\n",inifile);return false;
    }

    char strBuffer[301];

    //字符串拆分工具
    CCmdStr CmdStr;
    //结构体存放数据
    St_stationInfo stCode;
    while(true)
    {
      //站点参数文件中读取一行，若已经读取完则跳出循环
        if(file.Fgets(strBuffer,300, true) == false) break;
      //把读取到的数据拆分
        CmdStr.SplitToCmd(strBuffer,",", true);
      //扔掉无效行
        if(CmdStr.CmdCount()!=6) continue;
      //把拆分的数据存入结构体
        CmdStr.GetValue(0,stCode.provname,30);
        CmdStr.GetValue(1,stCode.obtid,10);
        CmdStr.GetValue(2,stCode.obtname,30);
        CmdStr.GetValue(3,&stCode.lat);
        CmdStr.GetValue(4,&stCode.lon);
        CmdStr.GetValue(5,&stCode.height);
      //把站点参数结构体放入站点参数容器
        v_stationInfo.push_back(stCode);
    }
    return true;
}

void MockSurfData()
{
    //Set the random generator seed before calling rand()
    //seed:time(0) 系统时间，单位s 为了时间不重复，加入种子
    srand(time(0));

    //使用当前时间作为观测时间
    char* strddatetime = new char[21];
    LocalTime(strddatetime,"yyyymmddhh24miss");

    auto stSurfdataInfo = make_shared<St_surfdataInfo>();
    for(int i = 0;i<v_stationInfo.size();++i)
    {
        //c风格字符串只能strncpy
        strncpy(stSurfdataInfo->obtid,v_stationInfo[i].obtid,10);//站点代码
        strncpy(stSurfdataInfo->ddatetime,strddatetime,14);  // 数据时间：格式yyyymmddhh24miss长度为14
        stSurfdataInfo->t=rand()%351;       // 气温：单位，0.1摄氏度
        stSurfdataInfo->p=rand()%265+10000; // 气压：0.1百帕
        stSurfdataInfo->u=rand()%100+1;     // 相对湿度，0-100之间的值。
        stSurfdataInfo->wd=rand()%360;      // 风向，0-360之间的值。
        stSurfdataInfo->wf=rand()%150;      // 风速：单位0.1m/s
        stSurfdataInfo->r=rand()%16;        // 降雨量：0.1mm
        stSurfdataInfo->vis=rand()%5001+100000;  // 能见度：0.1米

        v_surfdataInfo.push_back(*stSurfdataInfo);
    }
    delete [] strddatetime;
}

