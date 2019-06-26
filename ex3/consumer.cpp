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
char szName[] = "SHMbuff";    // 共享内存的名字
char NFULL[]="FULL";			//信号量FULL名字 
char NEMPTY[]="EMPTY";	//信号量EMPTY名字 
char NMUTEX[]="MUTEX";	//信号量MUTEX名字 
int main()
{
	SYSTEMTIME time;
	//打开信号量获得句柄
	HANDLE EMPTY=OpenSemaphore(SEMAPHORE_ALL_ACCESS,NULL,NEMPTY);
	HANDLE FULL=OpenSemaphore(SEMAPHORE_ALL_ACCESS,NULL,NFULL);
	HANDLE MUTEX=OpenSemaphore(SEMAPHORE_ALL_ACCESS,NULL,NMUTEX);
	
	//打开共享文件 
	HANDLE hMapFile=OpenFileMapping(FILE_MAP_WRITE,FALSE,szName);	
	
	//在文件映射上创建视图
    struct SHMbuff *pBuf=(struct SHMbuff *)MapViewOfFile(
		hMapFile,            // 共享内存的句柄
        FILE_MAP_ALL_ACCESS, // 可读写许可
        0,
        0,
        sizeof(struct SHMbuff)   
	);
	
	for(int i=0;i<4;i++)
	{
		Sleep((rand()%5000));
		//p操作
		WaitForSingleObject(FULL, INFINITE);//p(full) 
		WaitForSingleObject(MUTEX, INFINITE);//p(mutex) 
		
		char information[15];
		strcpy(information,pBuf->buff[pBuf->stack]);
		memset(pBuf->buff[pBuf->stack],0,sizeof(pBuf->buff[pBuf->stack]));
		pBuf->stack--;
		
		
		GetSystemTime(&time);
		cout<<time.wYear<<"年"<<time.wMonth<<"月"<<time.wDay<<"日"
		<<time.wHour<<"时"<<time.wMinute<<"分"<<time.wSecond<<"秒"<<time.wMilliseconds<<"毫秒"<<endl;
		cout<<"消费者进程"<<GetCurrentProcessId()<<"从共享主存取出数据："<<endl;
		cout<<information<<endl; 
		pBuf->sum2++;
		cout<<"取出后共享主存内容："<<endl;
		for(int i=0;i<3;i++)
		cout<<"buf"<<i<<":"<<pBuf->buff[i]<<";"<<endl; 
		cout<<endl<<"消费者者进程共取出"<<pBuf->sum2<<"次"<<endl<<"............."<<endl<<endl<<endl;
		
		
		//释放信号量
		ReleaseSemaphore(MUTEX,1,NULL);//v(mutex)
		ReleaseSemaphore(EMPTY,1,NULL); //v(enpty)
		
		
		
		
	}
	
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
	
	
} 
