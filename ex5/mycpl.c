#include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <dirent.h>
 #include <unistd.h>
 #include <sys/types.h> 
 #include <sys/stat.h>    
#include <fcntl.h>  
#include <errno.h>  
  
#define SIZE 1024

 int readAll(char *srcPath,char *tgtPath)
 {
     DIR *dir,*dir2;
     struct dirent *ptr;
     char base[1000];//源目录基地址
	 char base2[1000];//目标目录基地址
	 char baseli[1000];//符号链接指向目录
 
     if ((dir=opendir(srcPath)) == NULL)
     {
         perror(srcPath);
         exit(1);
     }
     if ((dir2=opendir(tgtPath)) == NULL)
     {
		if(mkdir(tgtPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1){
			perror("tgtPath");
		}         //目标目录创建子目录
         
     }
	 
     memset(base,'\0',sizeof(base));
     memset(base2,'\0',sizeof(base2));
	 memset(baseli,'\0',sizeof(baseli));
	 
     strcpy(base,srcPath);
     strcpy(base2,tgtPath);	 
     strcat(base,"/");
     strcat(base2,"/");	 
	 
     while ((ptr=readdir(dir)) != NULL)
     {
         if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
         {
			 continue;
		 }
         else if(ptr->d_type == 8)    ///file
		 {	
			printf("d_name:%s/%s\n",srcPath,ptr->d_name);
            strcat(base,ptr->d_name);
            strcat(base2,ptr->d_name);
			copy(base,base2);
     memset(base,'\0',sizeof(base));
     memset(base2,'\0',sizeof(base2));
     strcpy(base,srcPath);
     strcpy(base2,tgtPath);	 
     strcat(base,"/");
     strcat(base2,"/");				
		 }
         else if(ptr->d_type == 10)    ///link file
		 {
             printf("d_name:%s/%s\n",srcPath,ptr->d_name);
             strcat(base,ptr->d_name);
             strcat(base2,ptr->d_name);
			 //int fd=dirfd(base2);
			 readlink(base,baseli,sizeof(baseli)); 
			 printf("linkfile %s\n",baseli);
			 symlink(baseli,base2);  
     memset(base,'\0',sizeof(base));
     memset(base2,'\0',sizeof(base2));
     strcpy(base,srcPath);
     strcpy(base2,tgtPath);	 
     strcat(base,"/");
     strcat(base2,"/");				 
         }
		 else if(ptr->d_type == 4)    ///dir
         {
             strcat(base,ptr->d_name);
			 strcat(base2,ptr->d_name);
             readAll(base,base2);
     memset(base,'\0',sizeof(base));
     memset(base2,'\0',sizeof(base2));
     strcpy(base,srcPath);
     strcpy(base2,tgtPath);	 
     strcat(base,"/");
     strcat(base2,"/");	
         }
		 else{
			 printf("zheshawenjian\n");
		 }
     }
     closedir(dir);
     return 1;
 }
 
 int copy(char *srcFile,char *tgtFile)
 {
    int src_fd;  
    int tgt_fd;  
  
    int nread;  
    int nwrite;  
  
    char buff[SIZE];  
    char *ptr;  

    if((src_fd = open(srcFile,O_RDONLY)) == -1)  
    {  
        fprintf(stderr,"open %serror: %s\n",srcFile,strerror(errno));  
        exit(-1);  
    }  
      
    if((tgt_fd = open(tgtFile,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR)) == -1)  
    {  
        fprintf(stderr,"open %serror: %s\n",tgtFile,strerror(errno));  
        exit(-1);  
    }  
  
    while(nread = read(src_fd,buff,SIZE))  
    {  
        if((nread == -1) && (errno != EINTR))  
        {  
            break;  
        }  
        else if(nread > 0)  
        {  
            ptr = buff;  
            while(nwrite = write(tgt_fd,ptr,nread))  
            {  
                if((nwrite == -1) && (errno != EINTR))  
                {  
                    break;  
                }  
                else if(nwrite == nread)  
                {  
                   break;  
                }  
                else if(nwrite > 0)  
                {  
                    ptr += nwrite;  
                    nread -= nwrite;  
                }  
            }  
            if(nwrite == -1)  
            {  
                break;  
            }  
        }  
    }  
  
    close(src_fd);  
    close(tgt_fd);   
  
    return 0;  	
 }
 
 int main(int argc,char* argv[])
 {
	if(argc != 3)  
    {  
        fprintf(stderr,"Usage: %s fromfile tofile\n\a",argv[0]);  
        exit(-1);  
    }  
	
	int status;
	char srcPath[1000];
	char tgtPath[1000];
    memset(srcPath,'\0',sizeof(srcPath));
    memset(tgtPath,'\0',sizeof(tgtPath));
	
	strcpy(srcPath,argv[1]);
	stpcpy(tgtPath,argv[2]);
	
	if(mkdir(tgtPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1){
		perror(tgtPath);
	}
	
	readAll(srcPath,tgtPath);
	
    return 0;
 }