#include<windows.h>
#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<cstdio>
#include<time.h>
using namespace std;
struct SHMbuff{
	int stack;
	int sum1;
	int sum2;
	char buff[3][15];
}; 
char szName[]="SHMbuff";    // 共享内存的名字
char NFULL[]="FULL";			//信号量FULL名字 
char NEMPTY[]="EMPTY";	//信号量EMPTY名字 
char NMUTEX[]="MUTEX";	//信号量MUTEX名字 
PROCESS_INFORMATION Newprocess(char szfilename[]);
int main()
{
	SYSTEMTIME time;
	//建立信号量
	HANDLE FULL=CreateSemaphore(NULL,0,3,NFULL);//安全属性参数，信号量初值，信号量最大值，信号量名称(任意一个进程可通过名称引用信号量) 
	HANDLE EMPTY=CreateSemaphore(NULL,3,3,NEMPTY);
	HANDLE MUTEX=CreateSemaphore(NULL,1,1,NMUTEX);
	
	
	// 创建共享文件句柄
    HANDLE hMapFile=CreateFileMapping(
        INVALID_HANDLE_VALUE,    // 物理文件句柄
        NULL,                    // 默认安全级别
        PAGE_READWRITE,          // 可读可写
        0,                       // 高位文件大小
        sizeof(struct SHMbuff),   // 地位文件大小
        szName                   // 共享内存名称
	);

	//在文件映射上创建视图
    struct SHMbuff *pBuf=(struct SHMbuff *)MapViewOfFile(
		hMapFile,            // 共享内存的句柄
        FILE_MAP_ALL_ACCESS, // 可读写许可
        0,
        0,
        sizeof(struct SHMbuff) 
	);
	pBuf->stack=-1;
	pBuf->sum1=0; 
	pBuf->sum2=0;
	PROCESS_INFORMATION p1,p2; //2生产者进程 
	PROCESS_INFORMATION c1,c2,c3;//3个消费者进程 
	
	p1=Newprocess("producer.exe");
	p2=Newprocess("producer.exe");
	
	
	c1=Newprocess("consumer.exe");
	c2=Newprocess("consumer.exe");
	c3=Newprocess("consumer.exe");
	
	WaitForSingleObject(p1.hProcess,INFINITE);
	WaitForSingleObject(p2.hProcess,INFINITE);
	
	WaitForSingleObject(c1.hProcess,INFINITE);
	WaitForSingleObject(c2.hProcess,INFINITE);
	WaitForSingleObject(c3.hProcess,INFINITE);
	
	cout<<"生产者消费者读写完毕,共享主存内容："<<endl;
	for(int i=0;i<3;i++)
	cout<<"buf"<<i<<":"<<pBuf->buff[i]<<endl;
	cout<<"..........."<<endl; 
	
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
	
}

PROCESS_INFORMATION Newprocess(char szfilename[])
{
	
	STARTUPINFO si;  //用于子进程的	STARTUPINFO结构
	ZeroMemory(reinterpret_cast<void*>(&si),sizeof(si));
	si.cb=sizeof(si); 
 	PROCESS_INFORMATION pi;      //该结构返回有关新进程及其主线程的信息
	bool bCreatok=CreateProcess(szfilename,	// 不在此指定可执行文件的文件名
				NULL,            //进程符号表示 
				NULL,			// 默认进程安全性
				NULL,			// 默认线程安全性
				FALSE,			// 指定当前进程内的句柄不可以被子进程继承
				FALSE,	// 为新进程创建一个新的控制台窗口
				NULL,			// 使用本进程的环境变量
				NULL,			// 使用本进程的驱动器和目录
				&si,
				&pi);
	return pi;
}
