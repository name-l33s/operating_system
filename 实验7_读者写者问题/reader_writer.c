#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define TOTAL_OPERATIONS 12

// 操作类型
#define OP_READER 0
#define OP_WRITER 1

typedef struct {
    int type;
    int id;
    int arrival_time;
    int burst_time;
    long long start_exec;
    double wait_time;
} Operation;

Operation operations[TOTAL_OPERATIONS] = {
    {OP_WRITER, 1, 0, 100, 0, 0},
    {OP_READER, 1, 100, 400, 0, 0},
    {OP_READER, 2, 200, 100, 0, 0},
    {OP_WRITER, 2, 300, 100, 0, 0},
    {OP_WRITER, 1, 400, 200, 0, 0},
    {OP_READER, 3, 500, 400, 0, 0},
    {OP_READER, 2, 600, 300, 0, 0},
    {OP_WRITER, 2, 700, 200, 0, 0},
    {OP_WRITER, 1, 800, 200, 0, 0},
    {OP_READER, 1, 900, 300, 0, 0},
    {OP_READER, 3, 1000, 400, 0, 0},
    {OP_WRITER, 1, 1100, 200, 0, 0}
};

// 同步变量
pthread_mutex_t mutex;
pthread_cond_t can_read;
pthread_cond_t can_write;
int readers_active = 0;
int writers_active = 0;
int readers_waiting = 0;
int writers_waiting = 0;
int writer_deferred_count = 0;  // 用于避免写者饥饿
long long start_time;

long long get_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

void simulate_time(int ms) {
    usleep(ms * 1000);
}

void* perform_operation(void* arg) {
    int op_idx = *(int*)arg;
    Operation* op = &operations[op_idx];
    
    // 等待到达
    long long arrival = start_time + op->arrival_time;
    while (get_time_ms() < arrival) {
        usleep(100);
    }
    long long wait_start = get_time_ms();
    
    if (op->type == OP_READER) {
        // 读者逻辑
        pthread_mutex_lock(&mutex);
        readers_waiting++;
        // 如果写者延迟次数>=3，或者有写者活动，就等待
        while (writers_active > 0 || (writer_deferred_count >= 3 && writers_waiting > 0)) {
            pthread_cond_wait(&can_read, &mutex);
        }
        readers_waiting--;
        readers_active++;
        pthread_mutex_unlock(&mutex);
        
        op->start_exec = get_time_ms();
        op->wait_time = op->start_exec - wait_start;
        printf("[%4lld ms] Reader %d starts reading\n", op->start_exec - start_time, op->id);
        
        simulate_time(op->burst_time);
        
        printf("[%4lld ms] Reader %d finishes reading\n", get_time_ms() - start_time, op->id);
        
        pthread_mutex_lock(&mutex);
        readers_active--;
        if (readers_active == 0) {
            if (writers_waiting > 0) {
                pthread_cond_signal(&can_write);
            }
        }
        pthread_mutex_unlock(&mutex);
        
    } else {
        // 写者逻辑
        pthread_mutex_lock(&mutex);
        writers_waiting++;
        // 如果读者活动或有其他写者活动，就等待
        while (readers_active > 0 || writers_active > 0) {
            writer_deferred_count++;
            pthread_cond_wait(&can_write, &mutex);
        }
        writers_waiting--;
        writer_deferred_count = 0;  // 重置计数
        writers_active = 1;
        pthread_mutex_unlock(&mutex);
        
        op->start_exec = get_time_ms();
        op->wait_time = op->start_exec - wait_start;
        printf("[%4lld ms] Writer %d starts writing\n", op->start_exec - start_time, op->id);
        
        simulate_time(op->burst_time);
        
        printf("[%4lld ms] Writer %d finishes writing\n", get_time_ms() - start_time, op->id);
        
        pthread_mutex_lock(&mutex);
        writers_active = 0;
        // 优先唤醒等待的读者，除非写者已经被延迟很多次了
        if (writers_waiting > 0 && writer_deferred_count >= 3) {
            pthread_cond_signal(&can_write);
        } else if (readers_waiting > 0) {
            pthread_cond_broadcast(&can_read);
        } else if (writers_waiting > 0) {
            pthread_cond_signal(&can_write);
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t threads[TOTAL_OPERATIONS];
    int args[TOTAL_OPERATIONS];
    
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&can_read, NULL);
    pthread_cond_init(&can_write, NULL);
    
    start_time = get_time_ms();
    
    printf("=== 读者-写者问题（公平调度，避免写者饥饿）===\n");
    printf("时间\t事件\n");
    printf("--------------------------------\n");
    
    for (int i = 0; i < TOTAL_OPERATIONS; i++) {
        args[i] = i;
        pthread_create(&threads[i], NULL, perform_operation, &args[i]);
    }
    
    for (int i = 0; i < TOTAL_OPERATIONS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("\n=== 等待时间统计 ===\n");
    double tr = 0, tw = 0;
    int nr = 0, nw = 0;
    
    for (int i = 0; i < TOTAL_OPERATIONS; i++) {
        Operation* op = &operations[i];
        if (op->type == OP_READER) {
            printf("Reader %d: %.0f ms\n", op->id, op->wait_time);
            tr += op->wait_time;
            nr++;
        } else {
            printf("Writer %d: %.0f ms\n", op->id, op->wait_time);
            tw += op->wait_time;
            nw++;
        }
    }
    
    printf("\n=== 平均等待时间 ===\n");
    printf("Readers: %.2f ms\n", tr / nr);
    printf("Writers: %.2f ms\n", tw / nw);
    printf("Overall: %.2f ms\n", (tr + tw) / TOTAL_OPERATIONS);
    
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&can_read);
    pthread_cond_destroy(&can_write);
    
    return 0;
}
