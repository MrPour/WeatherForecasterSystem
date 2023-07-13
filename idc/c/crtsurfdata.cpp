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

//创建站点信息容器 -- 全局变量
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
//创建观测数据容器 -- 全局变量
vector<St_surfdataInfo> v_surfdataInfo;

//使用当前时间作为观测时间
char strddatetime[21];

// 模拟生成全国气象站点分钟观测数据，存放在vsurfdata容器中。
void MockSurfData();

//vsurfdata容器数据存入文件
bool FileSurfData(const char *outPath,const char *dataFormat);

//日志文件声明为全局变量 -- 全局变量
CLogFile logFile;
int main(int argc,char *argv[])
{
    //inifile outpath logfile
    if(argc!=5)
    {
        cout<<"Using: ./crtsurfdata inifile outpath logfile dataFormat"<<endl;
        cout<<"Example:/project/idc/bin/crtsurfdata /project/idc/ini/stcode.ini "
              "/tmp/idc/surfdata /log/idc/crtsurfdata.log xml,csv,json\n\n"<<endl;
        cout<<"inifile 全国气象站点参数文件名。"<<endl;
        cout<<"outpath 全国气象站点数据文件存放目录。"<<endl;
        cout<<"logfile 本程序运行的日志文件名。"<<endl;
        cout<<"datafmt 生成数据文件的格式，支持xml、json和csv三种格式，中间用逗号分隔。"<<endl;

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

    // 模拟生成全国气象站点分钟观测数据
    MockSurfData();
    //生成的观测数据存入文件
    if(strstr(argv[4],"xml")!=0) FileSurfData(argv[2],"xml");
    if(strstr(argv[4],"json")!=0) FileSurfData(argv[2],"json");
    if(strstr(argv[4],"csv")!=0) FileSurfData(argv[2],"csv");
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


    //必须要初始化，否则数据会出现问题
    memset(strddatetime,0,sizeof(strddatetime));
    LocalTime(strddatetime,"yyyymmddhh24miss");

    St_surfdataInfo stSurfdataInfo;
    for(int i = 0;i<v_stationInfo.size();++i)
    {
        //c风格字符串只能strncpy
        strncpy(stSurfdataInfo.obtid,v_stationInfo[i].obtid,10);//站点代码
        strncpy(stSurfdataInfo.ddatetime,strddatetime,14);  // 数据时间：格式yyyymmddhh24miss长度为14
        stSurfdataInfo.t=rand()%351;       // 气温：单位，0.1摄氏度
        stSurfdataInfo.p=rand()%265+10000; // 气压：0.1百帕
        stSurfdataInfo.u=rand()%100+1;     // 相对湿度，0-100之间的值。
        stSurfdataInfo.wd=rand()%360;      // 风向，0-360之间的值。
        stSurfdataInfo.wf=rand()%150;      // 风速：单位0.1m/s
        stSurfdataInfo.r=rand()%16;        // 降雨量：0.1mm
        stSurfdataInfo.vis=rand()%5001+100000;  // 能见度：0.1米

        v_surfdataInfo.push_back(stSurfdataInfo);
    }
}

bool FileSurfData(const char *outPath,const char *dataFormat)
{
    CFile File;
    int total = v_surfdataInfo.size();
    // 拼接生成数据的文件名，例如：/tmp/idc/surfdata/SURF_ZH_20210629092200_2254.csv
    char strFileName[301];
    // 将格式化的字符存入数组中,拼接当前pid的目的是防止文件名重复
    sprintf(strFileName,"%s/SURF_ZH_%s_%d.%s",outPath,strddatetime,getpid(),dataFormat);
    // 打开文件并写入,Rename方式会生成中间文件，写入时不会被错误的读入脏数据
    if(File.OpenForRename(strFileName,"w")== false)
    {
        logFile.Write("File.OpenForRename(%s) failed.\n",strFileName);
        return false;
    }
    // 如果是csv格式写入第一行标题
    if(strcmp(dataFormat,"csv") == 0)  File.Fprintf("站点代码,数据时间,气温,气压,相对湿度,风向,风速,降雨量,能见度\n");
    // 遍历v_surfdataInfo
    for (int i=0;i < total;i++)
    {
        // 写入一条记录。注意浮点数的运算10.0
        if (strcmp(dataFormat,"csv")==0)
            File.Fprintf("%s,%s,%.1f,%.1f,%d,%d,%.1f,%.1f,%.1f\n",\
         v_surfdataInfo[i].obtid,v_surfdataInfo[i].ddatetime,v_surfdataInfo[i].t/10.0,v_surfdataInfo[i].p/10.0,\
         v_surfdataInfo[i].u,v_surfdataInfo[i].wd,v_surfdataInfo[i].wf/10.0,v_surfdataInfo[i].r/10.0,v_surfdataInfo[i].vis/10.0);
    }
    // 关闭文件并将中间文件改为目标文件名
    File.CloseAndRename();
    logFile.Write("生成数据文件%s成功，数据时间%s，记录数%d。\n",strFileName,strddatetime,total);
    return true;
}
