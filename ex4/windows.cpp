#include<Windows.h>
#include<WinBase.h>
#include<Psapi.h>
#include<TlHelp32.h>
#include<iostream>
#include<iomanip>
#include<Shlwapi.h>
#include<stdio.h>
#include<cstdio>
#include<tchar.h>
#include<psapi.h>
using namespace std;
#pragma comment(lib,"psapi.lib")
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"Shlwapi.lib")
//显示保护标记，该标记表示允许应用程序对内存进行访问的类型
inline bool TestSet(DWORD dwTarget, DWORD dwMask)
{
	return ((dwTarget&dwMask) == dwMask);
}
#define SHOWMASK(dwTarget,type)\
if (TestSet(dwTarget, PAGE_##type))\
{cout << "," << #type; }
//定义的页面保护方式
void ShowProtection(DWORD dwTarget)
{
	SHOWMASK(dwTarget, READONLY);
	SHOWMASK(dwTarget, GUARD);
	SHOWMASK(dwTarget, NOCACHE);
	SHOWMASK(dwTarget, READWRITE);
	SHOWMASK(dwTarget, WRITECOPY);
	SHOWMASK(dwTarget, EXECUTE);
	SHOWMASK(dwTarget, EXECUTE_READ);
	SHOWMASK(dwTarget, EXECUTE_READWRITE);
	SHOWMASK(dwTarget, EXECUTE_WRITECOPY);
	SHOWMASK(dwTarget, NOACCESS);
}

//遍历整个虚拟内存，显示单个进程虚拟地址空间布局
void WalkVM(HANDLE hProcess)
{
	SYSTEM_INFO si;                     //系统信息结构
	ZeroMemory(&si, sizeof(si));		//初始化
	GetSystemInfo(&si);                 //获得系统信息
	MEMORY_BASIC_INFORMATION mbi;       //进程虚拟内存空间的基本信息结构
	ZeroMemory(&mbi, sizeof(mbi));      //分配缓冲区，用于保存信息				
	LPCVOID pBlock = (LPVOID)si.lpMinimumApplicationAddress; //循环整个应用程序地址空间
	while (pBlock < si.lpMaximumApplicationAddress)
	{
		//获得下一个虚拟内存块的信息
		if (VirtualQueryEx(hProcess,           //进程句柄
			pBlock,                            //开始位置
			&mbi,                              //缓冲区
			sizeof(mbi)) == sizeof(mbi))       //长度的确认，如果失败则返回0
		{
			//块的结尾指针
			LPCVOID pEnd = (PBYTE)pBlock + mbi.RegionSize;
			TCHAR szSize[MAX_PATH];
			StrFormatByteSize(mbi.RegionSize, szSize, MAX_PATH);

			//显示块地址和长度
			cout.fill('0');
			cout << hex << setw(8) << (DWORD)pBlock
				<< "--"
				<< hex << setw(8) << (DWORD)pEnd
				<< (wcslen(szSize) == 7 ? " (" : " (") << szSize
				<< ") ";
			//显示块的状态
			switch (mbi.State) {
			case MEM_COMMIT:
				printf(" Committed"); break;
			case MEM_FREE:
				printf(" Free"); break;
			case MEM_RESERVE:
				printf(" Reserved"); break;
			}

			//显示保护
			if (mbi.Protect == 0 && mbi.State != MEM_FREE) {
				mbi.Protect = PAGE_READONLY;
			}
			ShowProtection(mbi.Protect);

			//显示类型
			switch (mbi.Type) {
			case MEM_IMAGE:
				printf(", Image"); break;
			case MEM_MAPPED:
				printf(", Mapped"); break;
			case MEM_PRIVATE:
				printf(", Private"); break;
			}

			//检测可执行的映像
			TCHAR szFilename[MAX_PATH];
			if (GetModuleFileName(
				(HMODULE)pBlock,        //实际虚拟内存的模块句柄
				szFilename,             //完全指定的文件名称
				MAX_PATH) > 0)            //实际使用的缓冲区长度
			{
				//除去路径并显示
				PathStripPath(szFilename);
				printf("%s", szFilename);
			}

			printf("\n");
			//移动块指针以获得下一个块
			pBlock = pEnd;
		}
	}
}
//WIN API得到当前console 的(x,y)
void console_gotoxy(int x, int y)
{
	// 得到当前console的句柄
	HANDLE hc = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD cursor = { x, y };
	//设置新的cursor位置 
	SetConsoleCursorPosition(hc, cursor);
}
//WIN API设置当前console 的(x, y)
void console_getxy(int& x, int& y)
{
	// 得到当前console的句柄
	HANDLE hc = GetStdHandle(STD_OUTPUT_HANDLE);
	// 屏幕缓冲区信息 
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	//得到相应缓冲区信息
	GetConsoleScreenBufferInfo(hc, &csbi);
	x = csbi.dwCursorPosition.X;
	y = csbi.dwCursorPosition.Y;
}
void systeminfo()
{
	//获取系统信息 
	SYSTEM_INFO si;
	ZeroMemory(&si,sizeof(si));		
	GetSystemInfo(&si);
	
	//对尺寸进行格式转化
	TCHAR szPageSize[MAX_PATH];
	StrFormatByteSize(si.dwPageSize,szPageSize,MAX_PATH);

	cout<<"内存分页大小："<<szPageSize<<endl;
	cout.fill('0');

	DWORD mensize=(DWORD)si.lpMaximumApplicationAddress-(DWORD)si.lpMinimumApplicationAddress;
	TCHAR szMemSize[MAX_PATH];
	StrFormatByteSize(mensize,szMemSize,MAX_PATH);
	cout<<"进程可用地址空间的最小内存地址：0x"<<hex<<setw(8)
		<<(DWORD)si.lpMinimumApplicationAddress<<endl;
	cout<<"进程可用地址空间的最大内存地址：0x"<<hex<<setw(8)
		<<(DWORD)si.lpMaximumApplicationAddress<<endl;
	printf("进程可用地址空间内存总大小：%d\n",szMemSize);
	printf("系统配备的CPU数量：%d\n",si.dwNumberOfProcessors);
	printf("系统配备的CPU类型：%d\n",si.dwProcessorType);
	printf("系统配备的CPU级别：%d\n",si.wProcessorLevel);
}

void globalmenorystatus()
{
	//当前物理内存和虚拟内存信息
	MEMORYSTATUSEX mem;
	mem.dwLength=sizeof(mem);
	GlobalMemoryStatusEx(&mem);
	
	printf("物理内存使用率：%d",mem.dwMemoryLoad);cout<<"%"<<endl;
	printf("总的物理内存：%.2lfGB\n",double(mem.ullTotalPhys/(1024*1024*1024)));
	printf("可用物理内存：%.2lfMB\n",double(mem.ullAvailPhys/(1024*1024)));
	printf("总的交换文件：%.2lfGB\n",double(mem.ullTotalPageFile/(1024*1024*1024)));
	printf("可用交换文件：%.2lfGB\n",double(mem.ullAvailPageFile/(1024*1024*1024)));
	printf("总的虚拟内存：%.2lfGB\n",double(mem.ullTotalVirtual/(1024*1024*1024)));
	printf("可用虚拟内存：%.2lfGB\n",double(mem.ullAvailVirtual/(1024*1024*1024)));
	printf("扩展内存大小：%.2lfGB\n",double(mem.ullAvailExtendedVirtual/(1024*1024*1024)));

}

void performaceinfo()
{
	//获取系统的性能信息

	PERFORMANCE_INFORMATION  pi;
	pi.cb=sizeof(pi);
	
		GetPerformanceInfo(&pi,sizeof(pi));
		cout<<"分页大小：" << pi.PageSize/1024<<"KB"<< endl;
		printf("系统当前提交页面总数：%d页\n",pi.CommitTotal);
		printf("系统当前可提交最大页面数：%d页\n",pi.CommitLimit);
		printf("系统历史提交页面峰值：%d页\n",pi.CommitPeak);
		printf("按页分配的物理内存总页数：%d页\n",pi.PhysicalTotal);
		printf("当前可用物理内存：%d页\n",pi.PhysicalAvailable);
		printf("物理内存大小：%.2lfGB\n",double((pi.PhysicalTotal*pi.PageSize)/1024/1024/1024));
		printf("可用物理内存大小：%.2lfMB\n",double(pi.PhysicalAvailable*pi.PageSize)/1024/1024);
		printf("系统内核内存占据页面数：%d页\n",pi.KernelTotal);
		printf("系统分页池大小：%d页\n",pi.KernelPaged);
		printf("系统非分页池大小：%d页\n",pi.KernelNonpaged);
		printf("系统打开的句柄总量：%d\n",pi.HandleCount);
		printf("系统进程总量：%d\n",pi.ProcessCount);
		printf("系统线程总量：%d\n",pi.ThreadCount);
		printf("系统cache容量：%d\n",pi.SystemCache);
	

}
void progressinformation(int pid)
{
	//获得进程的信息
	PROCESSENTRY32 pe;    //存储进程快照的结构体
	pe.dwSize=sizeof(pe);
	PROCESS_MEMORY_COUNTERS pps;
	pps.cb=sizeof(pps); 
	HANDLE hProcessSnap=::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);//当前运行进程快照，获取系统中正在运行的进程信息
	BOOL bMore=::Process32First(hProcessSnap,&pe);//通过当前运行进程的快照来获取第一个进程的句柄
	
	while(bMore)
	{
		HANDLE hP=OpenProcess(PROCESS_ALL_ACCESS, FALSE,pe.th32ProcessID);
		GetProcessMemoryInfo(hP,&pps,sizeof(pps));//获取进程内存信息
		if(pid==pe.th32ProcessID)
		{
			cout<<"要查询的进程的工作集信息："<<endl;
			GetProcessMemoryInfo(hP,&pps,sizeof(pps));//获取进程内存信息
			wcout<<"进程名称:"<<pe.szExeFile<<endl;
			printf("进程ID：%d\n",pe.th32ProcessID);
			printf("线程数：%d\n",pe.cntThreads);
			printf("当前使用内存：%.2lfKB\n",(double)pps.WorkingSetSize/1024);
			printf("使用内存高峰：%.2lfKB\n",(double)pps.PeakWorkingSetSize/1024);
			printf("使用分页文件：%.2lfKB\n",(double)pps.PagefileUsage/1024);
			printf("使用分页文件高峰：%.2lfKB\n",(double)pps.PeakPagefileUsage/1024);
			printf("缺页中断次数：%d\n",pps.PageFaultCount);
			printf("使用页面缓冲池：%.2lfKB\n",(double)pps.QuotaPagedPoolUsage/1024);
			printf("使用页面缓冲池高峰：%.2lfKB\n",(double)pps.QuotaPeakPagedPoolUsage/1024);
			
		}
			
		bMore=Process32Next(hProcessSnap,&pe);
			
	}
}
int main()
{
	int pid;
	int lineX=0,lineY=0;
	int flag=0;
	cout<<"请输入要查询进程的ID"<<endl;
	cin>>pid;
	cout<<"输入成功!"<<endl;
	console_gotoxy(lineX,lineY);
	cout<<"系统特征信息："<<endl;
	systeminfo();cout<<endl<<endl;
	while(1)
	{
	//使用win api控制缓冲区刷新输出
		if(flag==0)
		{
			console_getxy(lineX,lineY);
			flag++;
		}
		else
		cout<<"系统性能信息:"<<endl;
		performaceinfo();cout<<endl<<endl;
		cout<<"系统当前物理内存和虚拟内存的使用情况:"<<endl;
		globalmenorystatus();cout<<endl<<endl;
		progressinformation(pid);
		if(flag==1)
		{
			cout<<"进程的虚拟地址空间布局："<<endl;
			HANDLE hProcess=OpenProcess(PROCESS_ALL_ACCESS, FALSE,pid);
			WalkVM(hProcess);
			flag++;
		}
		Sleep(2000);
	}
}