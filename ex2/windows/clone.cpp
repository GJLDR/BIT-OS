#include<stdio.h>
#include<windows.h>
#include<iostream>
using namespace std;

int main()
{
	printf("子进程标识符:%d\n",GetCurrentProcessId());
	Sleep(3000);
	return 0;
		
} 
