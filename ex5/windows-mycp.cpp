#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<string>
#include<windows.h> 
#include<windowsx.h>
using namespace std;
void copydir(char *path1,char *path2);
void copyfile(char *path1,char *path2); 
int main(int argc,char*argv[])
{
	char oldpath[100],newpath[100];
	strcpy(oldpath,argv[1]);
	strcpy(newpath,argv[2]);
	if(argc!=3)
	{
		printf("error!\n");
		return 0;
	}
	WIN32_FIND_DATA findfiledata;
	if((FindFirstFile(oldpath,&findfiledata))==INVALID_HANDLE_VALUE)
	{
		printf("FindFirstFile error!\n");
		return 0;
	}
	if((FindFirstFile(newpath,&findfiledata))==INVALID_HANDLE_VALUE)//创建新目录 
	{
			
		CreateDirectory(newpath,NULL);
			
	}
	copydir(oldpath,newpath);
	return 0;
}
void copyfile(char *path1,char *path2)
{
	//printf("!\n");
	char oldpath[100],newpath[100];
	strcpy(oldpath,path1);
	strcpy(newpath,path2);
	WIN32_FIND_DATA findfiledata;
	HANDLE hFind=FindFirstFile(oldpath,&findfiledata);   //查找指定文件路径
	
	HANDLE open=CreateFile(oldpath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);//打开文件 
	HANDLE create=CreateFile(newpath,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);//创建文件 

	DWORDLONG FileSize=(findfiledata.nFileSizeHigh*(MAXDWORD+1))+findfiledata.nFileSizeLow;//计算文件大小
	char buff[FileSize];
	DWORD D;
	ReadFile(open,buff,FileSize,&D,NULL); //从源文件读数据
	WriteFile(create,buff,FileSize,&D,NULL);//向新文件写数据
	
	
	
	CloseHandle(open);
	CloseHandle(create); 
	
	return ;
	
}
void copydir(char *path1,char *path2)
{
	char oldpath[100],newpath[100];
	strcpy(oldpath,path1);
	strcpy(newpath,path2);
	strcat(oldpath,"\\");
	char a[10]="*.*"; //当前目录下的所有文件 
	strcat(oldpath,a);
	WIN32_FIND_DATAA findFileData; 
	//printf("!\n"); 
    HANDLE hFind=FindFirstFile(oldpath,&findFileData);  
    //printf("!\n");
    if (INVALID_HANDLE_VALUE==hFind)  
    { 
        int iErrno=GetLastError();  
        return;  
    }  
    while(FindNextFile(hFind,&findFileData)!=0)
    {
    	if(!strcmp(findFileData.cFileName,".")||!strcmp(findFileData.cFileName,".."))
    	continue;
		printf("%s\n",findFileData.cFileName);
    	strcpy(oldpath,path1);
    	strcat(oldpath,"\\");
		strcat(oldpath,findFileData.cFileName);
		strcat(newpath,"\\");
		strcat(newpath,findFileData.cFileName); 
    	if(findFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)//目录
		{
			CreateDirectory(newpath,NULL);
			copydir(oldpath,newpath);
			strcpy(oldpath,path1);
			strcpy(newpath,path2);
		}
		else
		{
			copyfile(oldpath,newpath);
			strcpy(oldpath,path1);
			strcpy(newpath,path2);
		} 
	}
	FindClose(hFind);
	return ;
}
