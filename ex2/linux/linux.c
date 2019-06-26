#include<sys/types.h>
#include<sys/wait.h>
#include<time.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>

int main(int argc,char * argv[])
{
	struct timeval start,end;
	pid_t pid;
        char path[30]="/home/gjldr/ex2/";
	strcat(path,argv[1]);
	gettimeofday(&start,NULL);
	if((pid=fork())<0)
	{
	   	printf("fork error!\n");		
		exit(-1);
	}
	printf("father:fork success!\n");
	if(pid==0)
	{
		if(execv(path,NULL)<0)
		{
			printf("execv error!\n");
			exit(-1);
		}
		else
		printf("father:execv successful!\n");
	}
	wait(0);
	gettimeofday(&end,NULL);
	double time1=start.tv_sec*1000+start.tv_usec/1000;
	double time2=end.tv_sec*1000+end.tv_usec/1000;
	double time=time2-time1;
	printf("The time of process:%.2f ms\n",time);
	exit(0);
}




