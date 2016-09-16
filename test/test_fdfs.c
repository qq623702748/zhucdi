#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/wait.h>

#include "make_log.h"

#define FILE_ID_LEN     (128)

#define TEST_LOG_MODULE "test"
#define FDFS_LOG_PROC   "fdfs"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("usage ./a.out file_path");
        exit(1);
    }


    char *filename = argv[1];
    int pfd[2];
    char file_id[FILE_ID_LEN] = {0};
    pid_t pid;

    if (pipe(pfd) < 0)
    {
        perror("pip error");
        exit(1);
    }

    pid = fork();

    if (pid == 0) {
        //child
        //关闭读管道
        close(pfd[0]);


        //将标准输出 重定向 写管道
        dup2(pfd[1], STDOUT_FILENO);

        //exec 执行fdfs_upload_file
        execlp("fdfs_upload_file", "fdfs_upload_file", "/etc/fdfs/client.conf", filename, NULL);
        perror("exec error");
    }

    //parent
    //关闭写管道
    close(pfd[1]);


    wait(NULL);

    //从管道中去读数据
    read(pfd[0], file_id, FILE_ID_LEN);

    //将结果记入领导log中
    LOG(TEST_LOG_MODULE, FDFS_LOG_PROC, "get file_id[%s]", file_id);
    

    

	return 0;
}
