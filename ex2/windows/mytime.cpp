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
	
	STARTUPINFO si;  //用于子进程的	STARTUPINFO结构
	ZeroMemory(reinterpret_cast<void*>(&si),sizeof(si));
	si.cb=sizeof(si); 
 	si.dwFlags = STARTF_USESHOWWINDOW;	// 指定wShowWindow成员有效
	si.wShowWindow = FALSE;			// 此成员设为TRUE的话则显示新建进程的主窗口	
	
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
	if(!bCreatok)
	{
		cout<<"子进程创建失败！"<<endl;
		return 0;
	}
	cout<<"子进程创建成功！"<<endl; 
	
	GetSystemTime(&start);//子进程开始时间 
	cout<<start.wYear<<"年"<<start.wMonth<<"月"<<start.wDay<<"日"
	<<start.wHour<<"时"<<start.wMinute<<"分"<<start.wSecond<<"秒"<<start.wMilliseconds<<"毫秒"<<endl; 
	
	WaitForSingleObject(pi.hProcess, INFINITE);//等待子进程结束 
	
	CloseHandle(pi.hProcess);//关闭进程和其线程的句柄 
	CloseHandle(pi.hThread);
	
	GetSystemTime(&end);//子进程结束时间 
	cout<<end.wYear<<"年"<<end.wMonth<<"月"<<end.wDay<<"日"
	<<end.wHour<<"时"<<end.wMinute<<"分"<<end.wSecond<<"秒"<<end.wMilliseconds<<"毫秒"<<endl;
	
	cout<<"子进程运行时间:"<<end.wMinute-start.wMinute<<"minute "<<end.wSecond-start.wSecond<<"."<<end.wMilliseconds-start.wMilliseconds<<"S"<<endl; 
} 
