#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_PROCESSES 20
#define QUEUE1_QUANTUM 4
#define QUEUE2_QUANTUM 8
#define QUEUE3_QUANTUM 16
#define BOOST_TIME 32

typedef struct {
    char name;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int start_time;
    int finish_time;
    int turnaround_time;
    int current_queue;  // 0=Q1, 1=Q2, 2=Q3
    int wait_in_queue;  // 在当前队列等待的时间
    bool is_urgent;
    bool completed;
    bool in_queue;
} Process;

typedef struct {
    Process *data[MAX_PROCESSES];
    int front, rear;
    int count;
} Queue;

void init_queue(Queue *q) {
    q->front = 0;
    q->rear = -1;
    q->count = 0;
}

bool is_empty(Queue *q) {
    return q->count == 0;
}

void enqueue(Queue *q, Process *p) {
    if (q->count >= MAX_PROCESSES) return;
    q->rear = (q->rear + 1) % MAX_PROCESSES;
    q->data[q->rear] = p;
    q->count++;
    p->in_queue = true;
}

Process *dequeue(Queue *q) {
    if (is_empty(q)) return NULL;
    Process *p = q->data[q->front];
    q->front = (q->front + 1) % MAX_PROCESSES;
    q->count--;
    p->in_queue = false;
    p->wait_in_queue = 0;
    return p;
}

// 将进程放回队列头部（用于被抢占的进程）
void enqueue_front(Queue *q, Process *p) {
    if (q->count >= MAX_PROCESSES) return;
    q->front = (q->front - 1 + MAX_PROCESSES) % MAX_PROCESSES;
    q->data[q->front] = p;
    q->count++;
    p->in_queue = true;
}

void print_queue(Queue *q) {
    if (is_empty(q)) {
        printf("空");
        return;
    }
    int idx = q->front;
    for (int i = 0; i < q->count; i++) {
        printf("%c", q->data[idx]->name);
        if (i < q->count - 1) printf(" ");
        idx = (idx + 1) % MAX_PROCESSES;
    }
}

int main() {
    Process processes[] = {
        {'A', 0, 12, 12, -1, -1, 0, 0, 0, false, false, false},
        {'B', 2, 8, 8, -1, -1, 0, 0, 0, false, false, false},
        {'C', 5, 32, 32, -1, -1, 0, 0, 0, false, false, false},
        {'D', 6, 3, 3, -1, -1, 0, 0, 0, true, false, false},   // 紧急进程
        {'E', 8, 21, 21, -1, -1, 0, 0, 0, false, false, false},
        {'F', 9, 34, 34, -1, -1, 0, 0, 0, false, false, false},
        {'G', 10, 7, 7, -1, -1, 0, 0, 0, true, false, false},  // 紧急进程
        {'H', 12, 22, 22, -1, -1, 0, 0, 0, false, false, false},
        {'I', 15, 10, 10, -1, -1, 0, 0, 0, false, false, false}
    };
    int n = 9;
    
    Queue q1, q2, q3;
    init_queue(&q1);
    init_queue(&q2);
    init_queue(&q3);
    
    int current_time = 0;
    int completed = 0;
    int last_boost_time = 0;
    Process *running = NULL;  // 当前运行的进程
    int running_quantum = 0;  // 当前进程的时间片
    int running_time = 0;     // 已运行时间
    
    printf("=== 多级反馈队列调度 ===\n");
    printf("时间\t事件\t\tQ1\t\tQ2\t\tQ3\n");
    printf("----------------------------------------------------------------\n");
    
    while (completed < n) {
        // 检查新到达的进程
        for (int i = 0; i < n; i++) {
            if (!processes[i].completed && processes[i].arrival_time == current_time && !processes[i].in_queue && &processes[i] != running) {
                processes[i].current_queue = 0;
                enqueue(&q1, &processes[i]);
            }
        }
        
        // 检查是否有紧急进程到达，需要抢占
        Process *urgent = NULL;
        for (int i = 0; i < n; i++) {
            if (processes[i].is_urgent && processes[i].arrival_time == current_time && !processes[i].completed) {
                urgent = &processes[i];
                break;
            }
        }
        
        // 如果有紧急进程到达，抢占当前进程
        if (urgent && running) {
            // 将当前运行进程放回其队列头部
            if (running->current_queue == 0) {
                enqueue_front(&q1, running);
            } else if (running->current_queue == 1) {
                enqueue_front(&q2, running);
            } else {
                enqueue_front(&q3, running);
            }
            printf("%d\t[%c被抢占]\t", current_time, running->name);
            print_queue(&q1);
            printf("\t\t");
            print_queue(&q2);
            printf("\t\t");
            print_queue(&q3);
            printf("\n");
            running = NULL;
        }
        
        // 选择要执行的进程
        if (!running) {
            if (urgent) {
                running = urgent;
                running_quantum = urgent->remaining_time;
                running_time = 0;
            } else if (!is_empty(&q1)) {
                running = dequeue(&q1);
                running_quantum = QUEUE1_QUANTUM;
                running_time = 0;
            } else if (!is_empty(&q2)) {
                running = dequeue(&q2);
                running_quantum = QUEUE2_QUANTUM;
                running_time = 0;
            } else if (!is_empty(&q3)) {
                running = dequeue(&q3);
                running_quantum = QUEUE3_QUANTUM;
                running_time = 0;
            }
            
            if (running) {
                if (running->start_time == -1) {
                    running->start_time = current_time;
                }
                char event[50];
                if (running->is_urgent && running->arrival_time == current_time) {
                    sprintf(event, "%c(紧急)", running->name);
                } else if (running->current_queue == 0) {
                    sprintf(event, "%c(Q1)", running->name);
                } else if (running->current_queue == 1) {
                    sprintf(event, "%c(Q2)", running->name);
                } else {
                    sprintf(event, "%c(Q3)", running->name);
                }
                printf("%d\t%-12s\t", current_time, event);
                print_queue(&q1);
                printf("\t\t");
                print_queue(&q2);
                printf("\t\t");
                print_queue(&q3);
                printf("\n");
            }
        }
        
        if (running) {
            // 执行一个时间单位
            current_time++;
            running_time++;
            running->remaining_time--;
            
            // 检查新到达的进程
            for (int i = 0; i < n; i++) {
                if (!processes[i].completed && processes[i].arrival_time == current_time && !processes[i].in_queue && &processes[i] != running) {
                    processes[i].current_queue = 0;
                    enqueue(&q1, &processes[i]);
                }
            }
            
            // 增加队列中进程的等待时间
            for (int i = 0; i < n; i++) {
                if (processes[i].in_queue && !processes[i].completed) {
                    processes[i].wait_in_queue++;
                }
            }
            
            // 检查进程是否完成
            if (running->remaining_time == 0) {
                running->finish_time = current_time;
                running->turnaround_time = running->finish_time - running->arrival_time;
                running->completed = true;
                completed++;
                running = NULL;
            } else if (running_time >= running_quantum) {
                // 时间片用完，降级
                if (running->current_queue == 0) {
                    running->current_queue = 1;
                    enqueue(&q2, running);
                } else if (running->current_queue == 1) {
                    running->current_queue = 2;
                    enqueue(&q3, running);
                } else {
                    enqueue(&q3, running);
                }
                running = NULL;
            }
            
            // 优先级提升检查
            if (current_time - last_boost_time >= BOOST_TIME) {
                last_boost_time = current_time;
                
                // 如果有正在运行的进程，先把它放回队列
                if (running) {
                    if (running->current_queue == 0) {
                        enqueue_front(&q1, running);
                    } else if (running->current_queue == 1) {
                        enqueue_front(&q2, running);
                    } else {
                        enqueue_front(&q3, running);
                    }
                    running = NULL;
                }
                
                printf("%d\t[优先级提升]\t", current_time);
                
                // 将Q2中的进程提升到Q1
                Queue temp_q2;
                init_queue(&temp_q2);
                while (!is_empty(&q2)) {
                    Process *p = dequeue(&q2);
                    p->current_queue = 0;
                    p->wait_in_queue = 0;
                    enqueue(&temp_q2, p);
                }
                while (!is_empty(&temp_q2)) {
                    enqueue(&q1, dequeue(&temp_q2));
                }
                
                // 将Q3中的进程提升到Q2
                Queue temp_q3;
                init_queue(&temp_q3);
                while (!is_empty(&q3)) {
                    Process *p = dequeue(&q3);
                    p->current_queue = 1;
                    p->wait_in_queue = 0;
                    enqueue(&temp_q3, p);
                }
                while (!is_empty(&temp_q3)) {
                    enqueue(&q2, dequeue(&temp_q3));
                }
                
                print_queue(&q1);
                printf("\t\t");
                print_queue(&q2);
                printf("\t\t");
                print_queue(&q3);
                printf("\n");
            }
        } else {
            // CPU空闲
            printf("%d\t[空闲]\t\t", current_time);
            print_queue(&q1);
            printf("\t\t");
            print_queue(&q2);
            printf("\t\t");
            print_queue(&q3);
            printf("\n");
            current_time++;
            
            // 检查新到达的进程
            for (int i = 0; i < n; i++) {
                if (!processes[i].completed && processes[i].arrival_time == current_time && !processes[i].in_queue) {
                    processes[i].current_queue = 0;
                    enqueue(&q1, &processes[i]);
                }
            }
        }
    }
    
    printf("\n=== 调度结果 ===\n");
    printf("进程\t到达\t运行\t开始\t完成\t周转\n");
    printf("------------------------------------------------\n");
    
    double total_turnaround = 0;
    for (int i = 0; i < n; i++) {
        total_turnaround += processes[i].turnaround_time;
        printf("%c\t%d\t%d\t%d\t%d\t%d\n", 
               processes[i].name, 
               processes[i].arrival_time, 
               processes[i].burst_time, 
               processes[i].start_time,
               processes[i].finish_time, 
               processes[i].turnaround_time);
    }
    
    printf("\n平均周转时间: %.2f\n", total_turnaround / n);
    
    return 0;
}
