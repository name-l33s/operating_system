#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

pid_t child1, child2;
int pipefd[2];

void sigint_handler(int sig) {
    printf("\n[父进程] 收到 SIGINT 信号，正在终止子进程...\n");
    if (child1 > 0) {
        kill(child1, SIGUSR1);
    }
    if (child2 > 0) {
        kill(child2, SIGUSR1);
    }
}

void child1_handler(int sig) {
    printf("[子进程1] Child Process 1 is killed by Parent!\n");
    exit(0);
}

void child2_handler(int sig) {
    printf("[子进程2] Child Process 2 is killed by Parent!\n");
    exit(0);
}

int main() {
    if (pipe(pipefd) < 0) {
        perror("管道创建失败");
        exit(1);
    }
    
    signal(SIGINT, sigint_handler);
    
    child1 = fork();
    if (child1 < 0) {
        perror("子进程1创建失败");
        exit(1);
    } else if (child1 == 0) {
        signal(SIGUSR1, child1_handler);
        close(pipefd[0]);
        
        int count = 1;
        char message[100];
        while (1) {
            sprintf(message, "I send message %d times.\n", count);
            write(pipefd[1], message, strlen(message));
            printf("[子进程1] 发送: %s", message);
            count++;
            sleep(1);
        }
    }
    
    child2 = fork();
    if (child2 < 0) {
        perror("子进程2创建失败");
        kill(child1, SIGTERM);
        exit(1);
    } else if (child2 == 0) {
        signal(SIGUSR1, child2_handler);
        close(pipefd[1]);
        
        char buffer[100];
        ssize_t bytes_read;
        while (1) {
            bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1);
            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                printf("[子进程2] 接收: %s", buffer);
            } else if (bytes_read == 0) {
                break;
            } else {
                perror("读取管道失败");
                break;
            }
        }
        exit(0);
    }
    
    close(pipefd[0]);
    close(pipefd[1]);
    
    waitpid(child1, NULL, 0);
    waitpid(child2, NULL, 0);
    
    printf("[父进程] Parent Process is Killed!\n");
    return 0;
}