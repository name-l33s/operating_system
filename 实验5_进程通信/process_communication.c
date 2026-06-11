#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <time.h>

/* ==================== 1. 管道通信 ==================== */
void pipe_communication() {
    printf("\n===== 管道通信 =====\n");
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("pipe 创建失败");
        return;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork 失败");
        return;
    }

    if (pid == 0) {                     /* 子进程 */
        close(pipefd[0]);               /* 关闭读端 */
        pid_t my_pid = getpid();
        char message[128];
        snprintf(message, sizeof(message),
                 "PID %d is sending a message to parent", my_pid);
        write(pipefd[1], message, strlen(message) + 1);
        printf("[子进程 PID=%d] 发送: %s\n", my_pid, message);
        close(pipefd[1]);
        exit(0);
    } else {                            /* 父进程 */
        close(pipefd[1]);               /* 关闭写端 */
        char buffer[128];
        ssize_t n = read(pipefd[0], buffer, sizeof(buffer));
        if (n > 0) {
            printf("[父进程 PID=%d] 接收: %s\n", getpid(), buffer);
        }
        close(pipefd[0]);
        wait(NULL);
    }
}

/* ==================== 2. 消息队列通信 (System V IPC) ==================== */
/*
 * 在 Linux 上使用 msgget / msgsnd / msgrcv 实现。
 * macOS 不完全支持 System V 消息队列，故通过条件编译区分。
 */

#define SVKEY 75
#define REQ 1

struct msgbuf {
    long mtype;
    int  sender_pid;
    char text[100];
};

#if defined(__linux__)
#include <sys/ipc.h>
#include <sys/msg.h>

void message_queue_communication() {
    printf("\n===== 消息队列通信 (System V IPC) =====\n");

    int msqid = msgget(SVKEY, IPC_CREAT | 0666);
    if (msqid < 0) {
        perror("msgget 失败");
        return;
    }
    printf("[Server] 创建/获取消息队列, ID=%d\n", msqid);

    pid_t pid = fork();
    if (pid < 0) { perror("fork 失败"); return; }

    if (pid == 0) {                     /* 子进程 → Client */
        struct msgbuf msg;
        /* 发送请求消息 */
        msg.mtype     = REQ;
        msg.sender_pid = getpid();
        strcpy(msg.text, "request");
        if (msgsnd(msqid, &msg, sizeof(msg) - sizeof(long), 0) < 0) {
            perror("Client msgsnd 失败");
            exit(1);
        }
        printf("[Client PID=%d] 发送请求消息\n", msg.sender_pid);

        /* 等待应答（消息类型为自己的 PID）*/
        memset(&msg, 0, sizeof(msg));
        if (msgrcv(msqid, &msg, sizeof(msg) - sizeof(long),
                   getpid(), 0) < 0) {
            perror("Client msgrcv 失败");
            exit(1);
        }
        printf("[Client PID=%d] receive reply from %d\n",
               getpid(), msg.sender_pid);
        exit(0);

    } else {                            /* 父进程 → Server */
        struct msgbuf msg;
        /* 等待类型为 REQ 的请求 */
        if (msgrcv(msqid, &msg, sizeof(msg) - sizeof(long),
                   REQ, 0) < 0) {
            perror("Server msgrcv 失败");
            kill(pid, SIGTERM);
            wait(NULL);
            msgctl(msqid, IPC_RMID, NULL);
            return;
        }
        printf("[Server PID=%d] serving for client %d\n",
               getpid(), msg.sender_pid);

        /* 发送应答：消息类型设为 client 的 PID */
        msg.mtype     = msg.sender_pid;
        msg.sender_pid = getpid();
        strcpy(msg.text, "reply");
        if (msgsnd(msqid, &msg, sizeof(msg) - sizeof(long), 0) < 0) {
            perror("Server msgsnd 失败");
        }
        printf("[Server PID=%d] 发送应答给 client %ld\n",
               getpid(), msg.mtype);

        wait(NULL);

        /* 清理消息队列 */
        if (msgctl(msqid, IPC_RMID, NULL) < 0)
            perror("msgctl IPC_RMID 失败");
        else
            printf("[Server] 消息队列已删除\n");
    }
}

#else   /* macOS / 其他系统 — 打印说明并模拟输出 */

void message_queue_communication() {
    printf("\n===== 消息队列通信 (演示模拟) =====\n");
    printf("说明: System V 消息队列 (msgget/msgsnd/msgrcv) 在 Linux 上完整实现。\n");
    printf("当前系统不支持，以下为模拟输出：\n\n");

    /* 用 fork + pipe 模拟输出效果 */
    int pipe_mq[2];
    pipe(pipe_mq);

    pid_t pid = fork();
    if (pid == 0) {  /* Client */
        close(pipe_mq[0]);
        pid_t me = getpid();
        char line[256];
        snprintf(line, sizeof(line),
                 "[Client PID=%d] 发送请求消息\n"
                 "[Client PID=%d] receive reply from Server\n",
                 me, me);
        write(pipe_mq[1], line, strlen(line));
        close(pipe_mq[1]);
        exit(0);
    } else {         /* Server */
        close(pipe_mq[1]);
        printf("[Server PID=%d] 创建消息队列 (key=%d)\n", getpid(), SVKEY);
        printf("[Server PID=%d] serving for client %d\n", getpid(), pid);
        char buf[512];
        ssize_t n = read(pipe_mq[0], buf, sizeof(buf) - 1);
        if (n > 0) { buf[n] = 0; printf("%s", buf); }
        printf("[Server] 消息队列已删除\n");
        close(pipe_mq[0]);
        wait(NULL);
    }
}
#endif

/* ==================== 3. 共享内存通信 (POSIX) ==================== */
#define SHM_NAME "/os_exp5_shm"
#define SHM_SIZE 512

void shared_memory_communication() {
    printf("\n===== 共享内存通信 (POSIX) =====\n");

    /* 创建/打开共享内存对象 */
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0) { perror("shm_open 失败"); return; }

    if (ftruncate(shm_fd, SHM_SIZE) < 0) {
        perror("ftruncate 失败");
        shm_unlink(SHM_NAME);
        return;
    }

    void *ptr = mmap(NULL, SHM_SIZE,
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap 失败");
        shm_unlink(SHM_NAME);
        return;
    }

    pid_t pid = fork();
    if (pid < 0) { perror("fork 失败"); goto cleanup; }

    if (pid == 0) {                     /* 进程 B (子进程) */
        sleep(1);                       /* 等父进程完成初始写入 */
        printf("[进程B PID=%d] 从共享内存读到: %s\n",
               getpid(), (char *)ptr);

        char msg[] = "Hello from Process B!";
        strcpy((char *)ptr, msg);
        printf("[进程B PID=%d] 写入共享内存: %s\n", getpid(), msg);

        munmap(ptr, SHM_SIZE);
        exit(0);
    } else {                            /* 进程 A (父进程) */
        char init[] = "Shared memory initialized by Process A!";
        strcpy((char *)ptr, init);
        printf("[进程A PID=%d] 初始写入: %s\n", getpid(), init);

        wait(NULL);                     /* 等子进程写完后读取 */

        printf("[进程A PID=%d] 从共享内存读到: %s\n",
               getpid(), (char *)ptr);
    }

cleanup:
    munmap(ptr, SHM_SIZE);
    shm_unlink(SHM_NAME);
}

/* ==================== 主菜单 ==================== */
int main() {
    int choice;
    do {
        printf("\n======================================\n");
        printf("         进程通信演示\n");
        printf("======================================\n");
        printf("1. 管道通信 (父子进程)\n");
        printf("2. 消息队列通信 (Client-Server)\n");
        printf("3. 共享内存通信 (POSIX)\n");
        printf("0. 退出\n");
        printf("请选择: ");
        scanf("%d", &choice);

        switch (choice) {
        case 0:  printf("退出程序\n"); break;
        case 1:  pipe_communication();           break;
        case 2:  message_queue_communication();  break;
        case 3:  shared_memory_communication();  break;
        default: printf("无效选项！\n");
        }
    } while (choice != 0);

    return 0;
}
