#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    char name;
    int arrival_time;
    int run_time;
    int finish_time;
    int turnaround_time;
    double weighted_turnaround_time;
    bool completed;
} Job;

/* 按到达时间排序（冒泡排序）*/
void sort_by_arrival(Job jobs[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            if (jobs[j].arrival_time > jobs[j + 1].arrival_time) {
                Job t = jobs[j];
                jobs[j] = jobs[j + 1];
                jobs[j + 1] = t;
            }
        }
    }
}

void print_result(Job jobs[], int n, const char *algorithm, const char *order) {
    printf("=== %s ===\n", algorithm);
    printf("调度顺序: %s\n\n", order);

    double total_tt = 0, total_wtt = 0;
    printf("作业\t到达\t运行\t完成\t周转\t带权周转\n");
    for (int i = 0; i < n; i++) {
        total_tt += jobs[i].turnaround_time;
        total_wtt += jobs[i].weighted_turnaround_time;
        printf("  %c\t%d\t%d\t%d\t%d\t%.2f\n",
               jobs[i].name,
               jobs[i].arrival_time, jobs[i].run_time,
               jobs[i].finish_time, jobs[i].turnaround_time,
               jobs[i].weighted_turnaround_time);
    }
    printf("--------------------------------------------------\n");
    printf("平均周转时间: %.2f\n", total_tt / n);
    printf("平均带权周转时间: %.2f\n\n", total_wtt / n);
}

/* ========== FCFS：先来先服务 ==========
 * 每次选择已到达作业中最早到达的 */
void fcfs(Job jobs[], int n) {
    Job tmp[10];
    for (int i = 0; i < n; i++) tmp[i] = jobs[i];

    int cur = 0, done = 0;
    char order[100] = "";

    while (done < n) {
        int sel = -1;
        int earliest = 999999;
        for (int i = 0; i < n; i++) {
            if (!tmp[i].completed && tmp[i].arrival_time <= cur) {
                if (tmp[i].arrival_time < earliest) {
                    earliest = tmp[i].arrival_time;
                    sel = i;
                }
            }
        }
        /* 如果还没有作业到达，时间向前推进 */
        if (sel == -1) {
            cur++;
            continue;
        }
        tmp[sel].finish_time = cur + tmp[sel].run_time;
        tmp[sel].turnaround_time = tmp[sel].finish_time - tmp[sel].arrival_time;
        tmp[sel].weighted_turnaround_time =
            (double)tmp[sel].turnaround_time / tmp[sel].run_time;
        tmp[sel].completed = true;

        char tag[4];
        sprintf(tag, "%c ", tmp[sel].name);
        strcat(order, tag);
        cur = tmp[sel].finish_time;
        done++;
    }
    print_result(tmp, n, "FCFS 先来先服务", order);
}

/* ========== SJF：短作业优先 ==========
 * 每次选择已到达作业中运行时间最短的 */
void sjf(Job jobs[], int n) {
    Job tmp[10];
    for (int i = 0; i < n; i++) tmp[i] = jobs[i];

    int cur = 0, done = 0;
    char order[100] = "";

    while (done < n) {
        int sel = -1;
        int shortest = 999999;
        for (int i = 0; i < n; i++) {
            if (!tmp[i].completed && tmp[i].arrival_time <= cur) {
                if (tmp[i].run_time < shortest) {
                    shortest = tmp[i].run_time;
                    sel = i;
                }
            }
        }
        if (sel == -1) { cur++; continue; }

        tmp[sel].finish_time = cur + tmp[sel].run_time;
        tmp[sel].turnaround_time = tmp[sel].finish_time - tmp[sel].arrival_time;
        tmp[sel].weighted_turnaround_time =
            (double)tmp[sel].turnaround_time / tmp[sel].run_time;
        tmp[sel].completed = true;

        char tag[4];
        sprintf(tag, "%c ", tmp[sel].name);
        strcat(order, tag);
        cur = tmp[sel].finish_time;
        done++;
    }
    print_result(tmp, n, "SJF 短作业优先", order);
}

/* ========== HRRN：响应比高者优先 ==========
 * 响应比 = (等待时间 + 运行时间) / 运行时间
 * 每次选择已到达作业中响应比最高的 */
void hrrn(Job jobs[], int n) {
    Job tmp[10];
    for (int i = 0; i < n; i++) tmp[i] = jobs[i];

    int cur = 0, done = 0;
    char order[100] = "";

    while (done < n) {
        int sel = -1;
        double best_ratio = -1;
        for (int i = 0; i < n; i++) {
            if (!tmp[i].completed && tmp[i].arrival_time <= cur) {
                int wait = cur - tmp[i].arrival_time;
                double ratio = (double)(wait + tmp[i].run_time) / tmp[i].run_time;
                if (ratio > best_ratio) {
                    best_ratio = ratio;
                    sel = i;
                }
            }
        }
        if (sel == -1) { cur++; continue; }

        tmp[sel].finish_time = cur + tmp[sel].run_time;
        tmp[sel].turnaround_time = tmp[sel].finish_time - tmp[sel].arrival_time;
        tmp[sel].weighted_turnaround_time =
            (double)tmp[sel].turnaround_time / tmp[sel].run_time;
        tmp[sel].completed = true;

        char tag[4];
        sprintf(tag, "%c ", tmp[sel].name);
        strcat(order, tag);
        cur = tmp[sel].finish_time;
        done++;
    }
    print_result(tmp, n, "HRRN 响应比高者优先", order);
}

/* ========== 双道作业模拟（先到先服务的思想）==========
 * 系统可同时运行两道作业。每个时间片，任何一个CPU空闲时
 * 就从已到达队列中取最早到达的作业运行。 */
void fcfs_dual(Job jobs[], int n) {
    Job tmp[10];
    for (int i = 0; i < n; i++) {
        tmp[i] = jobs[i];
        tmp[i].completed = false;
    }

    int cur = 0, done = 0;
    char order[100] = "";

    /* 两个运行槽位：记录当前运行的作业下标和剩余时间 */
    int slot_job[2] = {-1, -1};
    int slot_remain[2] = {0, 0};

    while (done < n || slot_job[0] != -1 || slot_job[1] != -1) {
        /* ---- 时钟滴答：两个槽位各执行一个单位 ---- */
        for (int s = 0; s < 2; s++) {
            if (slot_job[s] != -1) {
                slot_remain[s]--;
                if (slot_remain[s] == 0) {
                    int idx = slot_job[s];
                    tmp[idx].finish_time = cur + 1; /* 在这个时间单位结束时完成 */
                    tmp[idx].turnaround_time = tmp[idx].finish_time - tmp[idx].arrival_time;
                    tmp[idx].weighted_turnaround_time =
                        (double)tmp[idx].turnaround_time / tmp[idx].run_time;
                    tmp[idx].completed = true;
                    char tag[4];
                    sprintf(tag, "%c ", tmp[idx].name);
                    strcat(order, tag);
                    done++;
                    slot_job[s] = -1;
                }
            }
        }

        /* ---- 填充空闲槽位 ---- */
        for (int s = 0; s < 2; s++) {
            if (slot_job[s] == -1) {
                /* 找已到达且不在运行中的最早到达作业 */
                int sel = -1;
                int earliest = 999999;
                for (int i = 0; i < n; i++) {
                    if (!tmp[i].completed && tmp[i].arrival_time <= cur) {
                        bool already_running = false;
                        for (int k = 0; k < 2; k++)
                            if (slot_job[k] == i) already_running = true;
                        if (!already_running && tmp[i].arrival_time < earliest) {
                            earliest = tmp[i].arrival_time;
                            sel = i;
                        }
                    }
                }
                if (sel != -1) {
                    slot_job[s] = sel;
                    slot_remain[s] = tmp[sel].run_time;
                }
            }
        }

        cur++;
        /* 处理这期间新到达的作业 */
        for (int i = 0; i < n; i++) {
            if (!tmp[i].completed && tmp[i].arrival_time == cur) {
                /* 刚到达，被后面的填充逻辑感知 */
            }
        }
    }

    print_result(tmp, n, "FCFS（双道并行）", order);
}

int main() {
    Job jobs[10] = {
        {'A', 0, 7},
        {'B', 2, 10},
        {'C', 5, 20},
        {'D', 7, 30},
        {'E', 12, 40},
        {'F', 15, 8},
        {'G', 4, 8},
        {'H', 6, 20},
        {'I', 8, 10},
        {'J', 10, 12}
    };

    /* 恢复初始状态（每个算法独立拷贝） */
    Job base[10];
    for (int i = 0; i < 10; i++) {
        base[i] = jobs[i];
        base[i].completed = false;
    }

    fcfs(base, 10);
    for (int i = 0; i < 10; i++) base[i].completed = false;
    sjf(base, 10);
    for (int i = 0; i < 10; i++) base[i].completed = false;
    hrrn(base, 10);
    for (int i = 0; i < 10; i++) base[i].completed = false;
    fcfs_dual(base, 10);

    return 0;
}
