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
	
	for(int i=0;i<6;i++)
	{	
		Sleep((rand()%3000));
		//p操作
		WaitForSingleObject(EMPTY, INFINITE);//p(empty) 
		WaitForSingleObject(MUTEX, INFINITE);//p(mutex) 
		
		char message[15];
		itoa(GetCurrentProcessId(),message,10);
		strcpy(pBuf->buff[++pBuf->stack],message);
		
		
		GetSystemTime(&time);
		cout<<time.wYear<<"年"<<time.wMonth<<"月"<<time.wDay<<"日"
		<<time.wHour<<"时"<<time.wMinute<<"分"<<time.wSecond<<"秒"<<time.wMilliseconds<<"毫秒"<<endl;
		cout<<"生产者进程"<<GetCurrentProcessId()<<"向共享主存写入"<<message<<"。"<<endl;
		pBuf->sum1++;
		cout<<"写入后共享主存内容："<<endl;
		for(int i=0;i<3;i++)
		cout<<"buf"<<i<<":"<<pBuf->buff[i]<<";"<<endl; 
		cout<<endl<<"生产者进程共写入"<<pBuf->sum1<<"次"<<endl<<"............."<<endl<<endl<<endl;
		
		
		//释放信号量
		ReleaseSemaphore(MUTEX,1,NULL);//v(mutex)
		ReleaseSemaphore(FULL,1,NULL); //v(full)
		
	
		
		
	}
	
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
	
	
} 
