#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    char name;
    int arrival_time;
    int run_time;
    int start_time;
    int finish_time;
    int turnaround_time;
    double weighted_turnaround_time;
    bool completed;
} Job;

void print_result(Job jobs[], int n, char* algorithm, char* order) {
    printf("=== %s ===\n", algorithm);
    printf("调度顺序: %s\n", order);
    
    double total_turnaround = 0, total_weighted = 0;
    printf("作业\t到达时间\t运行时间\t完成时间\t周转时间\t带权周转时间\n");
    for (int i = 0; i < n; i++) {
        total_turnaround += jobs[i].turnaround_time;
        total_weighted += jobs[i].weighted_turnaround_time;
        printf("%c\t%d\t\t%d\t\t%d\t\t%d\t\t%.2f\n", 
               jobs[i].name, jobs[i].arrival_time, jobs[i].run_time, 
               jobs[i].finish_time, jobs[i].turnaround_time, 
               jobs[i].weighted_turnaround_time);
    }
    printf("平均周转时间: %.2f\n", total_turnaround / n);
    printf("平均带权周转时间: %.2f\n\n", total_weighted / n);
}

void fcfs(Job jobs[], int n) {
    Job temp[10];
    for (int i = 0; i < n; i++) temp[i] = jobs[i];
    
    int current_time = 0;
    int completed = 0;
    char order[30] = "";
    
    while (completed < n) {
        int selected = -1;
        for (int i = 0; i < n; i++) {
            if (!temp[i].completed && temp[i].arrival_time <= current_time) {
                selected = i;
                break;
            }
        }
        
        if (selected != -1) {
            temp[selected].start_time = current_time;
            temp[selected].finish_time = current_time + temp[selected].run_time;
            temp[selected].turnaround_time = temp[selected].finish_time - temp[selected].arrival_time;
            temp[selected].weighted_turnaround_time = (double)temp[selected].turnaround_time / temp[selected].run_time;
            temp[selected].completed = true;
            
            char name[2] = {temp[selected].name, '\0'};
            sprintf(order + strlen(order), "%c ", temp[selected].name);
            current_time = temp[selected].finish_time;
            completed++;
        } else {
            current_time++;
        }
    }
    
    print_result(temp, n, "FCFS 先来先服务", order);
}

void sjf(Job jobs[], int n) {
    Job temp[10];
    for (int i = 0; i < n; i++) temp[i] = jobs[i];
    
    int current_time = 0;
    int completed = 0;
    char order[30] = "";
    
    while (completed < n) {
        int selected = -1;
        for (int i = 0; i < n; i++) {
            if (!temp[i].completed && temp[i].arrival_time <= current_time) {
                if (selected == -1 || temp[i].run_time < temp[selected].run_time) {
                    selected = i;
                }
            }
        }
        
        if (selected != -1) {
            temp[selected].start_time = current_time;
            temp[selected].finish_time = current_time + temp[selected].run_time;
            temp[selected].turnaround_time = temp[selected].finish_time - temp[selected].arrival_time;
            temp[selected].weighted_turnaround_time = (double)temp[selected].turnaround_time / temp[selected].run_time;
            temp[selected].completed = true;
            
            sprintf(order + strlen(order), "%c ", temp[selected].name);
            current_time = temp[selected].finish_time;
            completed++;
        } else {
            current_time++;
        }
    }
    
    print_result(temp, n, "SJF 短作业优先", order);
}

void hrrn(Job jobs[], int n) {
    Job temp[10];
    for (int i = 0; i < n; i++) temp[i] = jobs[i];
    
    int current_time = 0;
    int completed = 0;
    char order[30] = "";
    
    while (completed < n) {
        int selected = -1;
        double highest_ratio = 0;
        for (int i = 0; i < n; i++) {
            if (!temp[i].completed && temp[i].arrival_time <= current_time) {
                int wait = current_time - temp[i].arrival_time;
                double ratio = (double)(wait + temp[i].run_time) / temp[i].run_time;
                if (ratio > highest_ratio) {
                    highest_ratio = ratio;
                    selected = i;
                }
            }
        }
        
        if (selected != -1) {
            temp[selected].start_time = current_time;
            temp[selected].finish_time = current_time + temp[selected].run_time;
            temp[selected].turnaround_time = temp[selected].finish_time - temp[selected].arrival_time;
            temp[selected].weighted_turnaround_time = (double)temp[selected].turnaround_time / temp[selected].run_time;
            temp[selected].completed = true;
            
            sprintf(order + strlen(order), "%c ", temp[selected].name);
            current_time = temp[selected].finish_time;
            completed++;
        } else {
            current_time++;
        }
    }
    
    print_result(temp, n, "HRRN 响应比高者优先", order);
}

void fcfs_parallel(Job jobs[], int n) {
    Job temp[10];
    for (int i = 0; i < n; i++) temp[i] = jobs[i];
    
    int current_time = 0;
    int completed = 0;
    char order[30] = "";
    
    int running[2] = {-1, -1};
    int remaining[2] = {0, 0};
    
    while (completed < n || running[0] != -1 || running[1] != -1) {
        for (int i = 0; i < 2; i++) {
            if (running[i] != -1) {
                remaining[i]--;
                if (remaining[i] == 0) {
                    int idx = running[i];
                    temp[idx].finish_time = current_time + 1;
                    temp[idx].turnaround_time = temp[idx].finish_time - temp[idx].arrival_time;
                    temp[idx].weighted_turnaround_time = (double)temp[idx].turnaround_time / temp[idx].run_time;
                    temp[idx].completed = true;
                    sprintf(order + strlen(order), "%c ", temp[idx].name);
                    completed++;
                    running[i] = -1;
                }
            }
        }
        
        for (int i = 0; i < 2; i++) {
            if (running[i] == -1) {
                int selected = -1;
                for (int j = 0; j < n; j++) {
                    if (!temp[j].completed && temp[j].arrival_time <= current_time) {
                        bool already_running = false;
                        for (int k = 0; k < 2; k++) {
                            if (running[k] == j) {
                                already_running = true;
                                break;
                            }
                        }
                        if (!already_running) {
                            selected = j;
                            break;
                        }
                    }
                }
                if (selected != -1) {
                    running[i] = selected;
                    remaining[i] = temp[selected].run_time;
                    temp[selected].start_time = current_time;
                }
            }
        }
        
        current_time++;
    }
    
    print_result(temp, n, "FCFS 先来先服务(双道作业)", order);
}

int main() {
    Job jobs[10] = {
        {'A', 0, 7, 0, 0, 0, 0, false},
        {'B', 2, 10, 0, 0, 0, 0, false},
        {'C', 5, 20, 0, 0, 0, 0, false},
        {'D', 7, 30, 0, 0, 0, 0, false},
        {'E', 12, 40, 0, 0, 0, 0, false},
        {'F', 15, 8, 0, 0, 0, 0, false},
        {'G', 4, 8, 0, 0, 0, 0, false},
        {'H', 6, 20, 0, 0, 0, 0, false},
        {'I', 8, 10, 0, 0, 0, 0, false},
        {'J', 10, 12, 0, 0, 0, 0, false}
    };
    
    fcfs(jobs, 10);
    sjf(jobs, 10);
    hrrn(jobs, 10);
    fcfs_parallel(jobs, 10);
    
    return 0;
}