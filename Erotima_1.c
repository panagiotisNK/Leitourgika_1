#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#define N 30
int main()
{
pid_t pid[N];
int i;
int child_status;
for (i = 0; i < N; i++)
{
pid[i] = fork();
if (pid[i] == 0)
{
sleep(60-2*i);
exit(100+i);
}
}
for (i = 0; i < N; i++)
{
pid_t wpid = waitpid(pid[i], &child_status, 0);
if (WIFEXITED(child_status))
{
printf("Child%d terminated with exit status %d\n", wpid,
WEXITSTATUS(child_status));
}
else
{
printf("Child%d terminated abnormally\n", wpid);
}
}
return (0);
}

