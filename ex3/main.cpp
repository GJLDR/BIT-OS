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
char szName[]="SHMbuff";    // �����ڴ������
char NFULL[]="FULL";			//�ź���FULL���� 
char NEMPTY[]="EMPTY";	//�ź���EMPTY���� 
char NMUTEX[]="MUTEX";	//�ź���MUTEX���� 
PROCESS_INFORMATION Newprocess(char szfilename[]);
int main()
{
	SYSTEMTIME time;
	//�����ź���
	HANDLE FULL=CreateSemaphore(NULL,0,3,NFULL);//��ȫ���Բ������ź�����ֵ���ź������ֵ���ź�������(����һ�����̿�ͨ�����������ź���) 
	HANDLE EMPTY=CreateSemaphore(NULL,3,3,NEMPTY);
	HANDLE MUTEX=CreateSemaphore(NULL,1,1,NMUTEX);
	
	
	// ���������ļ����
    HANDLE hMapFile=CreateFileMapping(
        INVALID_HANDLE_VALUE,    // �����ļ����
        NULL,                    // Ĭ�ϰ�ȫ����
        PAGE_READWRITE,          // �ɶ���д
        0,                       // ��λ�ļ���С
        sizeof(struct SHMbuff),   // ��λ�ļ���С
        szName                   // �����ڴ�����
	);

	//���ļ�ӳ���ϴ�����ͼ
    struct SHMbuff *pBuf=(struct SHMbuff *)MapViewOfFile(
		hMapFile,            // �����ڴ�ľ��
        FILE_MAP_ALL_ACCESS, // �ɶ�д���
        0,
        0,
        sizeof(struct SHMbuff) 
	);
	pBuf->stack=-1;
	pBuf->sum1=0; 
	pBuf->sum2=0;
	PROCESS_INFORMATION p1,p2; //2�����߽��� 
	PROCESS_INFORMATION c1,c2,c3;//3�������߽��� 
	
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
	
	cout<<"�����������߶�д���,�����������ݣ�"<<endl;
	for(int i=0;i<3;i++)
	cout<<"buf"<<i<<":"<<pBuf->buff[i]<<endl;
	cout<<"..........."<<endl; 
	
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
	
}

PROCESS_INFORMATION Newprocess(char szfilename[])
{
	
	STARTUPINFO si;  //�����ӽ��̵�	STARTUPINFO�ṹ
	ZeroMemory(reinterpret_cast<void*>(&si),sizeof(si));
	si.cb=sizeof(si); 
 	PROCESS_INFORMATION pi;      //�ýṹ�����й��½��̼������̵߳���Ϣ
	bool bCreatok=CreateProcess(szfilename,	// ���ڴ�ָ����ִ���ļ����ļ���
				NULL,            //���̷��ű�ʾ 
				NULL,			// Ĭ�Ͻ��̰�ȫ��
				NULL,			// Ĭ���̰߳�ȫ��
				FALSE,			// ָ����ǰ�����ڵľ�������Ա��ӽ��̼̳�
				FALSE,	// Ϊ�½��̴���һ���µĿ���̨����
				NULL,			// ʹ�ñ����̵Ļ�������
				NULL,			// ʹ�ñ����̵���������Ŀ¼
				&si,
				&pi);
	return pi;
}
