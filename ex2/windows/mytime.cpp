#include<stdio.h>
#include<iostream>
#include<windows.h>
using namespace std;

int main(int argc,char *argv[] )
{
	SYSTEMTIME start,end;
	char szfilename[100]="";
	strcpy(szfilename,argv[1]);
	//scanf("%s",szfilename);
	
	STARTUPINFO si;  //�����ӽ��̵�	STARTUPINFO�ṹ
	ZeroMemory(reinterpret_cast<void*>(&si),sizeof(si));
	si.cb=sizeof(si); 
 	si.dwFlags = STARTF_USESHOWWINDOW;	// ָ��wShowWindow��Ա��Ч
	si.wShowWindow = FALSE;			// �˳�Ա��ΪTRUE�Ļ�����ʾ�½����̵�������	
	
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
	if(!bCreatok)
	{
		cout<<"�ӽ��̴���ʧ�ܣ�"<<endl;
		return 0;
	}
	cout<<"�ӽ��̴����ɹ���"<<endl; 
	
	GetSystemTime(&start);//�ӽ��̿�ʼʱ�� 
	cout<<start.wYear<<"��"<<start.wMonth<<"��"<<start.wDay<<"��"
	<<start.wHour<<"ʱ"<<start.wMinute<<"��"<<start.wSecond<<"��"<<start.wMilliseconds<<"����"<<endl; 
	
	WaitForSingleObject(pi.hProcess, INFINITE);//�ȴ��ӽ��̽��� 
	
	CloseHandle(pi.hProcess);//�رս��̺����̵߳ľ�� 
	CloseHandle(pi.hThread);
	
	GetSystemTime(&end);//�ӽ��̽���ʱ�� 
	cout<<end.wYear<<"��"<<end.wMonth<<"��"<<end.wDay<<"��"
	<<end.wHour<<"ʱ"<<end.wMinute<<"��"<<end.wSecond<<"��"<<end.wMilliseconds<<"����"<<endl;
	
	cout<<"�ӽ�������ʱ��:"<<end.wMinute-start.wMinute<<"minute "<<end.wSecond-start.wSecond<<"."<<end.wMilliseconds-start.wMilliseconds<<"S"<<endl; 
} 
