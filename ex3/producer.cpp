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
	
	for(int i=0;i<6;i++)
	{	
		Sleep((rand()%3000));
		//p����
		WaitForSingleObject(EMPTY, INFINITE);//p(empty) 
		WaitForSingleObject(MUTEX, INFINITE);//p(mutex) 
		
		char message[15];
		itoa(GetCurrentProcessId(),message,10);
		strcpy(pBuf->buff[++pBuf->stack],message);
		
		
		GetSystemTime(&time);
		cout<<time.wYear<<"��"<<time.wMonth<<"��"<<time.wDay<<"��"
		<<time.wHour<<"ʱ"<<time.wMinute<<"��"<<time.wSecond<<"��"<<time.wMilliseconds<<"����"<<endl;
		cout<<"�����߽���"<<GetCurrentProcessId()<<"��������д��"<<message<<"��"<<endl;
		pBuf->sum1++;
		cout<<"д������������ݣ�"<<endl;
		for(int i=0;i<3;i++)
		cout<<"buf"<<i<<":"<<pBuf->buff[i]<<";"<<endl; 
		cout<<endl<<"�����߽��̹�д��"<<pBuf->sum1<<"��"<<endl<<"............."<<endl<<endl<<endl;
		
		
		//�ͷ��ź���
		ReleaseSemaphore(MUTEX,1,NULL);//v(mutex)
		ReleaseSemaphore(FULL,1,NULL); //v(full)
		
	
		
		
	}
	
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
	
	
} 
