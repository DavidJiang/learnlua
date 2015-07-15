#include <stdio.h>
#include "mutex_linux.h"
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

int g_num = 0;
t_threadMutex g_mutex;

void* thread_func( void * )
{
	while(true)
	{
		g_mutex.Lock();
		g_num++;
		printf("thread %d's value is %d\n", pthread_self(), g_num);
		g_mutex.Unlock();
		sleep(2);
	}
}

void test_thread_mutex()
{
	bool bRet = g_mutex.Open();
	assert( bRet == true );

	pthread_t threads[10];
	for(int i = 0; i < 10; i++ )
	{
		int ret = pthread_create(&threads[i], NULL, thread_func, NULL );
	}

	for(int i = 0; i < 10; i++ )
	{
		pthread_join(threads[i], NULL);
	}
}
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	t_processMutex mutex;
	bool bRet = mutex.Open(L"/home/shawn/work/test_process_mutex");
	assert(bRet == true );
	mode_t oldmode = umask(0);
	int fd = shm_open("test", O_RDWR | O_CREAT, 0666);
	umask(oldmode);

	ftruncate(fd, sizeof(int) );

	int *count = (int *)mmap(NULL, sizeof(int), 
							 PROT_READ | PROT_WRITE, MAP_SHARED, 
							 fd, 0);
	if( count == NULL )
	{
		perror( strerror(errno) );
		return -1;
	}
	close(fd);

	while( true )
	{
		mutex.Lock();
		*count = *count + 1;
		printf("process %d's count is%d\n", getpid(), *count);
		mutex.Unlock();
		sleep(3);
	}
	shm_unlink("test_process_mutex");

	getchar();
	return 0;
}
