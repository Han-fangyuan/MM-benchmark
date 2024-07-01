#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <sys/wait.h>

//启动基准测试程序，该基准测试程序调用两个自定义的系统调用
int syscall_htmm_start = 449;
int syscall_htmm_end = 450;

long htmm_start(pid_t pid, int node)
{
    return syscall(syscall_htmm_start, pid, node);
}

long htmm_end(pid_t pid)
{
    return syscall(syscall_htmm_end, pid);
}


int main(int argc, char** argv)
{
    pid_t pid;
    int state;

    //没有提供基准测试程序的名称，
    if (argc < 2) {
        printf("Usage ./launch_bench [BENCHMARK]");	
        htmm_end(-1);   //调用 htmm_end(-1) 进行清理
        return 0;
    }

    pid = fork();//创建子进程
    //子进程
    if (pid == 0) 
    {
        execvp(argv[1], &argv[1]);//子进程执行基准程序
        perror("Fails to run bench");
        exit(-1);
    }

#ifdef __NOPID
    htmm_start(-1, 0);
#else
    htmm_start(pid, 0);//子进程的pid
#endif

    printf("pid: %d\n", pid);//打印子进程的pid，等待子进程结束
    waitpid(pid, &state, 0);

    htmm_end(-1);
    
    return 0;
}
