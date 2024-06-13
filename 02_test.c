#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>
#include<time.h>
#include<sys/wait.h>
#include<pthread.h>
#include<semaphore.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sqlite3.h>

int main(int argc, const char *argv[])
{
	char buf[1024]={};
	int fd1, fd2, fd3;

	//打开设备文件
	fd1 = open("/dev/mycdev0", O_RDWR);
	if(fd1<0)
	{
		printf("打开设备文件失败\n");
		exit(-1);
	}
	printf("打开设备文件成功\n");	

	//打开源文件
	fd2 = open("./03.txt", O_RDONLY);
	if(fd2<0)
	{
    	printf("打开源文件失败\n");
		exit(-1);
	}
    printf("打开源文件成功\n");

	//打开目标文件
	fd3 = open("./05.txt", O_WRONLY|O_CREAT|O_TRUNC, 0664);
	if(fd3<0)
	{
    	printf("打开目标文件失败\n");
		exit(-1);
	}
    printf("打开目标文件成功\n");


	//从源文件中读取数据
	int ret = read(fd2, buf, sizeof(buf));
	if(ret < 0)
	{
    	printf("read fd2 error\n");
		exit(-1);	
	}
    printf("read fd2 success\n");

	//文件数据写入到申请内存中
	ret = 1;
	ret = write(fd1, buf, sizeof(buf));
	if(ret < 0)
	{
    	printf("write fd1 error\n");
		exit(-1);	
	}
    printf("write fd1 success\n");

	//从内核模块中读出数据
	memset(buf, 0, sizeof(buf));
	ret = 1;
	ret = read(fd1, buf, sizeof(buf));
	if(ret < 0)
	{
    	printf("read fd1 error\n");
		exit(-1);	
	}
    printf("read fd1 success\n");

	//读出的文件数据写入到目标文件中
	ret = 1;
	ret = write(fd3, buf, sizeof(buf));
	if(ret < 0)
	{
    	printf("write fd3 error\n");
		exit(-1);	
	}
    printf("write fd3 success\n");


	close(fd1);
	close(fd2);
	close(fd3);

	return 0;
}

