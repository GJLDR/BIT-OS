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
char szName[] = "SHMbuff";    // �����ڴ������
char NFULL[]="FULL";			//�ź���FULL���� 
char NEMPTY[]="EMPTY";	//�ź���EMPTY���� 
char NMUTEX[]="MUTEX";	//�ź���MUTEX���� 
int main()
{
	SYSTEMTIME time;
	//���ź�����þ��
	HANDLE EMPTY=OpenSemaphore(SEMAPHORE_ALL_ACCESS,NULL,NEMPTY);
	HANDLE FULL=OpenSemaphore(SEMAPHORE_ALL_ACCESS,NULL,NFULL);
	HANDLE MUTEX=OpenSemaphore(SEMAPHORE_ALL_ACCESS,NULL,NMUTEX);
	
	//�򿪹����ļ� 
	HANDLE hMapFile=OpenFileMapping(FILE_MAP_WRITE,FALSE,szName);	
	
	//���ļ�ӳ���ϴ�����ͼ
    struct SHMbuff *pBuf=(struct SHMbuff *)MapViewOfFile(
		hMapFile,            // �����ڴ�ľ��
        FILE_MAP_ALL_ACCESS, // �ɶ�д���
        0,
        0,
        sizeof(struct SHMbuff)   
	);
	
	for(int i=0;i<4;i++)
	{
		Sleep((rand()%5000));
		//p����
		WaitForSingleObject(FULL, INFINITE);//p(full) 
		WaitForSingleObject(MUTEX, INFINITE);//p(mutex) 
		
		char information[15];
		strcpy(information,pBuf->buff[pBuf->stack]);
		memset(pBuf->buff[pBuf->stack],0,sizeof(pBuf->buff[pBuf->stack]));
		pBuf->stack--;
		
		
		GetSystemTime(&time);
		cout<<time.wYear<<"��"<<time.wMonth<<"��"<<time.wDay<<"��"
		<<time.wHour<<"ʱ"<<time.wMinute<<"��"<<time.wSecond<<"��"<<time.wMilliseconds<<"����"<<endl;
		cout<<"�����߽���"<<GetCurrentProcessId()<<"�ӹ�������ȡ�����ݣ�"<<endl;
		cout<<information<<endl; 
		pBuf->sum2++;
		cout<<"ȡ�������������ݣ�"<<endl;
		for(int i=0;i<3;i++)
		cout<<"buf"<<i<<":"<<pBuf->buff[i]<<";"<<endl; 
		cout<<endl<<"�������߽��̹�ȡ��"<<pBuf->sum2<<"��"<<endl<<"............."<<endl<<endl<<endl;
		
		
		//�ͷ��ź���
		ReleaseSemaphore(MUTEX,1,NULL);//v(mutex)
		ReleaseSemaphore(EMPTY,1,NULL); //v(enpty)
		
		
		
		
	}
	
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
	
	
} 
