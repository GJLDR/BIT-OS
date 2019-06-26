#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include<sys/wait.h>

#define SHMKEY 100
#define SEMKEY 10

int EMPTY=1;
int FULL=2;
int MUTEX=3;
struct SHMbuff{
int stack;
int sum1;
int sum2;
char buff[3][15]; //共享内存区的buff
};

union semun{
int val;
struct semid_ds *buf;
unsigned short *array;
}arg;
int shmid;     //共享内存关键字
int semid;    //信号量集合关键字

void p(int sem_num);//p操作
void v(int sem_num);//v操作

void producer(int id);
void consumer(int id);

int main()
{
	if((shmid=shmget(SHMKEY,sizeof(struct SHMbuff),0666|IPC_CREAT))<0)//创建共享内存区
	{
	printf("shmget error!\n");
	return 0;
	}
   	printf("shmget success!\n");
    	struct SHMbuff * index; //初始化共享主存区
	index=(struct SHMbuff *)shmat(shmid,0,0);
	index->stack=-1;
	index->sum1=0;
	index->sum2=0;
	//SHMbuff=shmat(shmid,0,0);
	
	union semun sem1,sem2,sem3;
	sem1.val=3;//empty
	sem2.val=0;//full
	sem3.val=1;//mutex
	if((semid=semget(SEMKEY,3,0600|IPC_CREAT))<0) //创建信号量集合包含3个信号量  empty  full  mutex
	{
		printf("semget error!\n");
		return 0;
	}	
    	printf("semget success!\n");
	semctl(semid,EMPTY,SETVAL,sem1);//初始化信号量empty
	semctl(semid,FULL,SETVAL,sem2);//full
	semctl(semid,MUTEX,SETVAL,sem3);//mutex
	
	//创建两个生产者进程
        pid_t p_1,p_2;
	if(p_1=fork())//创建一个子进程
	{
		if(p_2=fork())
		{
			
		}
		if(p_2==0)
		{
		producer(2);
		exit(0);
		}
	}
	if(p_1==0)
	{
	producer(1);
	exit(0);
	}

	//创建三个消费者进程
	pid_t c_1,c_2,c_3;
	if(c_1=fork())
	{
		if(c_2=fork())
		{
			if(c_3=fork())
			{
			}
			if(c_3==0)
			{
			consumer(3);
			exit(0);
			}
		}
		if(c_2==0)
		{
		consumer(2);
		exit(0);
		}
	}
	if(c_1==0)
	{
	consumer(1);
	exit(0);
	}
	for(int i=0;i<5;i++)
	wait(0);
	shmctl(shmid,IPC_RMID,0);	
}
void producer(int id)
{
	time_t now;
	struct SHMbuff *index;
	index=(struct SHMbuff *)shmat(shmid,0,0);
	for(int i=0;i<6;i++)
	{	
		sleep((int)random()%2);
		//sleep(1);
		p(EMPTY);//p(empty)
		p(MUTEX);//p(mutex)
		char producer[15]="producer";
		producer[strlen(producer)]=id+'0';
		strcpy(index->buff[++index->stack],producer);
		
		time(&now);
		printf("the producer %d input %s at %ld\n",id,producer,now);
		index->sum1++;
		if(index->sum1==1)
		printf("the producer has put %d time.\n",index->sum1);
		else
		printf("the producer has put %d times.\n",index->sum1);
		printf("SHMbuff:\n");
		for(int j=0;j<3;j++)
		printf("buff%d:%s;\n",j,index->buff[j]);
		printf("\n............\n");
		v(FULL);//v(full)
		v(MUTEX);//v(mutex)
	 	
		
	}
	shmdt(index);	
}

void consumer(int id)
{
	time_t now;
	struct SHMbuff *index;
	index=(struct SHMbuff *)shmat(shmid,0,0);
	for(int i=0;i<4;i++)
	{
		sleep((int)random()%10);
		//sleep(5);
		p(FULL);//p(full)
		p(MUTEX);//p(mutex)
		char message[15];
		strcpy(message,index->buff[index->stack]);
		memset(index->buff[index->stack],0,sizeof(index->buff[index->stack]));
		index->stack--;
		time(&now);
		index->sum2++;
		printf("the consumer %d take %s at %ld\n",id,message,now);
		if(index->sum2==1)
		printf("the consumer han taken %d time.\n",index->sum2);
		else
		printf("the consumer han taken %d times.\n",index->sum2);
		printf("SHMbuff:\n");
		for(int j=0;j<3;j++)
		printf("buff%d:%s;\n",j,index->buff[j]);
		printf("\n............\n");
		
		v(EMPTY);//v(empty)
		v(MUTEX);//v(mutex)
	 	
		
	}

	
	shmdt(index);
	
}

void p(int sem_num)
{
	struct sembuf xx;
	xx.sem_num=sem_num;
	xx.sem_op=-1;
	xx.sem_flg=0;
	semop(semid,&xx,1);
}
void v(int sem_num)
{
	struct sembuf xx;
	xx.sem_num=sem_num;
	xx.sem_op=1;
	xx.sem_flg=0;
	semop(semid,&xx,1);
}

