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
//��ʾ������ǣ��ñ�Ǳ�ʾ����Ӧ�ó�����ڴ���з��ʵ�����
inline bool TestSet(DWORD dwTarget, DWORD dwMask)
{
	return ((dwTarget&dwMask) == dwMask);
}
#define SHOWMASK(dwTarget,type)\
if (TestSet(dwTarget, PAGE_##type))\
{cout << "," << #type; }
//�����ҳ�汣����ʽ
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

//�������������ڴ棬��ʾ�������������ַ�ռ䲼��
void WalkVM(HANDLE hProcess)
{
	SYSTEM_INFO si;                     //ϵͳ��Ϣ�ṹ
	ZeroMemory(&si, sizeof(si));		//��ʼ��
	GetSystemInfo(&si);                 //���ϵͳ��Ϣ
	MEMORY_BASIC_INFORMATION mbi;       //���������ڴ�ռ�Ļ�����Ϣ�ṹ
	ZeroMemory(&mbi, sizeof(mbi));      //���仺���������ڱ�����Ϣ				
	LPCVOID pBlock = (LPVOID)si.lpMinimumApplicationAddress; //ѭ������Ӧ�ó����ַ�ռ�
	while (pBlock < si.lpMaximumApplicationAddress)
	{
		//�����һ�������ڴ�����Ϣ
		if (VirtualQueryEx(hProcess,           //���̾��
			pBlock,                            //��ʼλ��
			&mbi,                              //������
			sizeof(mbi)) == sizeof(mbi))       //���ȵ�ȷ�ϣ����ʧ���򷵻�0
		{
			//��Ľ�βָ��
			LPCVOID pEnd = (PBYTE)pBlock + mbi.RegionSize;
			TCHAR szSize[MAX_PATH];
			StrFormatByteSize(mbi.RegionSize, szSize, MAX_PATH);

			//��ʾ���ַ�ͳ���
			cout.fill('0');
			cout << hex << setw(8) << (DWORD)pBlock
				<< "--"
				<< hex << setw(8) << (DWORD)pEnd
				<< (wcslen(szSize) == 7 ? " (" : " (") << szSize
				<< ") ";
			//��ʾ���״̬
			switch (mbi.State) {
			case MEM_COMMIT:
				printf(" Committed"); break;
			case MEM_FREE:
				printf(" Free"); break;
			case MEM_RESERVE:
				printf(" Reserved"); break;
			}

			//��ʾ����
			if (mbi.Protect == 0 && mbi.State != MEM_FREE) {
				mbi.Protect = PAGE_READONLY;
			}
			ShowProtection(mbi.Protect);

			//��ʾ����
			switch (mbi.Type) {
			case MEM_IMAGE:
				printf(", Image"); break;
			case MEM_MAPPED:
				printf(", Mapped"); break;
			case MEM_PRIVATE:
				printf(", Private"); break;
			}

			//����ִ�е�ӳ��
			TCHAR szFilename[MAX_PATH];
			if (GetModuleFileName(
				(HMODULE)pBlock,        //ʵ�������ڴ��ģ����
				szFilename,             //��ȫָ�����ļ�����
				MAX_PATH) > 0)            //ʵ��ʹ�õĻ���������
			{
				//��ȥ·������ʾ
				PathStripPath(szFilename);
				printf("%s", szFilename);
			}

			printf("\n");
			//�ƶ���ָ���Ի����һ����
			pBlock = pEnd;
		}
	}
}
//WIN API�õ���ǰconsole ��(x,y)
void console_gotoxy(int x, int y)
{
	// �õ���ǰconsole�ľ��
	HANDLE hc = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD cursor = { x, y };
	//�����µ�cursorλ�� 
	SetConsoleCursorPosition(hc, cursor);
}
//WIN API���õ�ǰconsole ��(x, y)
void console_getxy(int& x, int& y)
{
	// �õ���ǰconsole�ľ��
	HANDLE hc = GetStdHandle(STD_OUTPUT_HANDLE);
	// ��Ļ��������Ϣ 
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	//�õ���Ӧ��������Ϣ
	GetConsoleScreenBufferInfo(hc, &csbi);
	x = csbi.dwCursorPosition.X;
	y = csbi.dwCursorPosition.Y;
}
void systeminfo()
{
	//��ȡϵͳ��Ϣ 
	SYSTEM_INFO si;
	ZeroMemory(&si,sizeof(si));		
	GetSystemInfo(&si);
	
	//�Գߴ���и�ʽת��
	TCHAR szPageSize[MAX_PATH];
	StrFormatByteSize(si.dwPageSize,szPageSize,MAX_PATH);

	cout<<"�ڴ��ҳ��С��"<<szPageSize<<endl;
	cout.fill('0');

	DWORD mensize=(DWORD)si.lpMaximumApplicationAddress-(DWORD)si.lpMinimumApplicationAddress;
	TCHAR szMemSize[MAX_PATH];
	StrFormatByteSize(mensize,szMemSize,MAX_PATH);
	cout<<"���̿��õ�ַ�ռ����С�ڴ��ַ��0x"<<hex<<setw(8)
		<<(DWORD)si.lpMinimumApplicationAddress<<endl;
	cout<<"���̿��õ�ַ�ռ������ڴ��ַ��0x"<<hex<<setw(8)
		<<(DWORD)si.lpMaximumApplicationAddress<<endl;
	printf("���̿��õ�ַ�ռ��ڴ��ܴ�С��%d\n",szMemSize);
	printf("ϵͳ�䱸��CPU������%d\n",si.dwNumberOfProcessors);
	printf("ϵͳ�䱸��CPU���ͣ�%d\n",si.dwProcessorType);
	printf("ϵͳ�䱸��CPU����%d\n",si.wProcessorLevel);
}

void globalmenorystatus()
{
	//��ǰ�����ڴ�������ڴ���Ϣ
	MEMORYSTATUSEX mem;
	mem.dwLength=sizeof(mem);
	GlobalMemoryStatusEx(&mem);
	
	printf("�����ڴ�ʹ���ʣ�%d",mem.dwMemoryLoad);cout<<"%"<<endl;
	printf("�ܵ������ڴ棺%.2lfGB\n",double(mem.ullTotalPhys/(1024*1024*1024)));
	printf("���������ڴ棺%.2lfMB\n",double(mem.ullAvailPhys/(1024*1024)));
	printf("�ܵĽ����ļ���%.2lfGB\n",double(mem.ullTotalPageFile/(1024*1024*1024)));
	printf("���ý����ļ���%.2lfGB\n",double(mem.ullAvailPageFile/(1024*1024*1024)));
	printf("�ܵ������ڴ棺%.2lfGB\n",double(mem.ullTotalVirtual/(1024*1024*1024)));
	printf("���������ڴ棺%.2lfGB\n",double(mem.ullAvailVirtual/(1024*1024*1024)));
	printf("��չ�ڴ��С��%.2lfGB\n",double(mem.ullAvailExtendedVirtual/(1024*1024*1024)));

}

void performaceinfo()
{
	//��ȡϵͳ��������Ϣ

	PERFORMANCE_INFORMATION  pi;
	pi.cb=sizeof(pi);
	
		GetPerformanceInfo(&pi,sizeof(pi));
		cout<<"��ҳ��С��" << pi.PageSize/1024<<"KB"<< endl;
		printf("ϵͳ��ǰ�ύҳ��������%dҳ\n",pi.CommitTotal);
		printf("ϵͳ��ǰ���ύ���ҳ������%dҳ\n",pi.CommitLimit);
		printf("ϵͳ��ʷ�ύҳ���ֵ��%dҳ\n",pi.CommitPeak);
		printf("��ҳ����������ڴ���ҳ����%dҳ\n",pi.PhysicalTotal);
		printf("��ǰ���������ڴ棺%dҳ\n",pi.PhysicalAvailable);
		printf("�����ڴ��С��%.2lfGB\n",double((pi.PhysicalTotal*pi.PageSize)/1024/1024/1024));
		printf("���������ڴ��С��%.2lfMB\n",double(pi.PhysicalAvailable*pi.PageSize)/1024/1024);
		printf("ϵͳ�ں��ڴ�ռ��ҳ������%dҳ\n",pi.KernelTotal);
		printf("ϵͳ��ҳ�ش�С��%dҳ\n",pi.KernelPaged);
		printf("ϵͳ�Ƿ�ҳ�ش�С��%dҳ\n",pi.KernelNonpaged);
		printf("ϵͳ�򿪵ľ��������%d\n",pi.HandleCount);
		printf("ϵͳ����������%d\n",pi.ProcessCount);
		printf("ϵͳ�߳�������%d\n",pi.ThreadCount);
		printf("ϵͳcache������%d\n",pi.SystemCache);
	

}
void progressinformation(int pid)
{
	//��ý��̵���Ϣ
	PROCESSENTRY32 pe;    //�洢���̿��յĽṹ��
	pe.dwSize=sizeof(pe);
	PROCESS_MEMORY_COUNTERS pps;
	pps.cb=sizeof(pps); 
	HANDLE hProcessSnap=::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);//��ǰ���н��̿��գ���ȡϵͳ���������еĽ�����Ϣ
	BOOL bMore=::Process32First(hProcessSnap,&pe);//ͨ����ǰ���н��̵Ŀ�������ȡ��һ�����̵ľ��
	
	while(bMore)
	{
		HANDLE hP=OpenProcess(PROCESS_ALL_ACCESS, FALSE,pe.th32ProcessID);
		GetProcessMemoryInfo(hP,&pps,sizeof(pps));//��ȡ�����ڴ���Ϣ
		if(pid==pe.th32ProcessID)
		{
			cout<<"Ҫ��ѯ�Ľ��̵Ĺ�������Ϣ��"<<endl;
			GetProcessMemoryInfo(hP,&pps,sizeof(pps));//��ȡ�����ڴ���Ϣ
			wcout<<"��������:"<<pe.szExeFile<<endl;
			printf("����ID��%d\n",pe.th32ProcessID);
			printf("�߳�����%d\n",pe.cntThreads);
			printf("��ǰʹ���ڴ棺%.2lfKB\n",(double)pps.WorkingSetSize/1024);
			printf("ʹ���ڴ�߷壺%.2lfKB\n",(double)pps.PeakWorkingSetSize/1024);
			printf("ʹ�÷�ҳ�ļ���%.2lfKB\n",(double)pps.PagefileUsage/1024);
			printf("ʹ�÷�ҳ�ļ��߷壺%.2lfKB\n",(double)pps.PeakPagefileUsage/1024);
			printf("ȱҳ�жϴ�����%d\n",pps.PageFaultCount);
			printf("ʹ��ҳ�滺��أ�%.2lfKB\n",(double)pps.QuotaPagedPoolUsage/1024);
			printf("ʹ��ҳ�滺��ظ߷壺%.2lfKB\n",(double)pps.QuotaPeakPagedPoolUsage/1024);
			
		}
			
		bMore=Process32Next(hProcessSnap,&pe);
			
	}
}
int main()
{
	int pid;
	int lineX=0,lineY=0;
	int flag=0;
	cout<<"������Ҫ��ѯ���̵�ID"<<endl;
	cin>>pid;
	cout<<"����ɹ�!"<<endl;
	console_gotoxy(lineX,lineY);
	cout<<"ϵͳ������Ϣ��"<<endl;
	systeminfo();cout<<endl<<endl;
	while(1)
	{
	//ʹ��win api���ƻ�����ˢ�����
		if(flag==0)
		{
			console_getxy(lineX,lineY);
			flag++;
		}
		else
		cout<<"ϵͳ������Ϣ:"<<endl;
		performaceinfo();cout<<endl<<endl;
		cout<<"ϵͳ��ǰ�����ڴ�������ڴ��ʹ�����:"<<endl;
		globalmenorystatus();cout<<endl<<endl;
		progressinformation(pid);
		if(flag==1)
		{
			cout<<"���̵������ַ�ռ䲼�֣�"<<endl;
			HANDLE hProcess=OpenProcess(PROCESS_ALL_ACCESS, FALSE,pid);
			WalkVM(hProcess);
			flag++;
		}
		Sleep(2000);
	}
}