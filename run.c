#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include<stdio.h>
#include<sys/wait.h>
int main(void)
{
	int pid,status;
	pid=fork();
	if(pid>0)
	{
		wait(&status);
		char *argv[] = { "lash", NULL};
		execvp("./lash", argv);
	}
	else if(pid==0)
	{
		execl("/bin/gcc","gcc","lashell.c", "-lreadline", "-o", "lash",(char *)0);
		exit(EXIT_SUCCESS);
	}
}