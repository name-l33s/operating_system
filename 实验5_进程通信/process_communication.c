#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

#define SHM_NAME "/my_shm"
#define SHM_SIZE 512

void pipe_communication() {
    printf("\n=== 管道通信 ===\n");
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("pipe创建失败");
        return;
    }
    
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork失败");
        return;
    } else if (pid == 0) {
        close(pipefd[0]);
        pid_t my_pid = getpid();
        char message[100];
        sprintf(message, "PID %d is sending a message to parent", my_pid);
        write(pipefd[1], message, strlen(message) + 1);
        printf("[子进程] 发送: %s\n", message);
        close(pipefd[1]);
        exit(0);
    } else {
        close(pipefd[1]);
        char buffer[100];
        read(pipefd[0], buffer, sizeof(buffer));
        printf("[父进程] 接收: %s\n", buffer);
        close(pipefd[0]);
        wait(NULL);
    }
}

void message_queue_demo() {
    printf("\n=== 消息队列通信 (演示) ===\n");
    printf("注意: Mac系统的System V IPC与Linux有差异\n");
    printf("在Linux上可以使用以下系统调用:\n");
    printf("  msgget() - 创建/获取消息队列\n");
    printf("  msgsnd() - 发送消息\n");
    printf("  msgrcv() - 接收消息\n");
    printf("  msgctl() - 控制消息队列\n");
    printf("\n工作流程:\n");
    printf("1. Server创建消息队列(key=75)，等待类型为1的请求\n");
    printf("2. Client发送类型为1的消息(包含自身PID)\n");
    printf("3. Server接收后显示'serving for client'和PID\n");
    printf("4. Server发送应答消息(类型=client PID，内容=server PID)\n");
    printf("5. Client接收后显示'receive reply from'和server PID\n");
}

void shared_memory_communication() {
    printf("\n=== 共享内存通信 ===\n");
    
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0) {
        perror("shm_open失败");
        printf("提示: 如遇权限问题，可尝试修改代码中的权限值\n");
        return;
    }
    
    if (ftruncate(shm_fd, SHM_SIZE) < 0) {
        perror("ftruncate失败");
        shm_unlink(SHM_NAME);
        return;
    }
    
    void *ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap失败");
        shm_unlink(SHM_NAME);
        return;
    }
    
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork失败");
        munmap(ptr, SHM_SIZE);
        shm_unlink(SHM_NAME);
        return;
    } else if (pid == 0) {
        sleep(1);
        char *shm_ptr = (char *)ptr;
        char message[] = "Hello from Process B!";
        strcpy(shm_ptr, message);
        printf("[进程B] 写入共享内存: %s\n", message);
        munmap(ptr, SHM_SIZE);
        exit(0);
    } else {
        char *shm_ptr = (char *)ptr;
        char init_msg[] = "Shared memory initialized by Process A!";
        strcpy(shm_ptr, init_msg);
        printf("[进程A] 初始写入: %s\n", init_msg);
        
        wait(NULL);
        
        printf("[进程A] 从共享内存读取: %s\n", shm_ptr);
        munmap(ptr, SHM_SIZE);
        shm_unlink(SHM_NAME);
    }
}

int main() {
    int choice;
    do {
        printf("\n===== 进程通信 =====\n");
        printf("0. 退出\n");
        printf("1. 管道通信\n");
        printf("2. 消息队列通信\n");
        printf("3. 共享内存通信\n");
        printf("请选择通信方式: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 0:
                printf("退出程序\n");
                break;
            case 1:
                pipe_communication();
                break;
            case 2:
                message_queue_demo();
                break;
            case 3:
                shared_memory_communication();
                break;
            default:
                printf("无效选项！\n");
        }
    } while (choice != 0);
    
    return 0;
}