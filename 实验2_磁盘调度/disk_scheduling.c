#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    char name;
    int track;
    bool visited;
} Request;

void fcfs(Request requests[], int n, int current_track) {
    int total_movement = 0;
    char order[50] = "";
    
    for (int i = 0; i < n; i++) {
        total_movement += abs(current_track - requests[i].track);
        sprintf(order + strlen(order), "%c ", requests[i].name);
        current_track = requests[i].track;
    }
    
    printf("=== FCFS 先来先服务 ===\n");
    printf("访问顺序: %s\n", order);
    printf("总移动道数: %d\n", total_movement);
    printf("平均移动道数: %.2f\n\n", (double)total_movement / n);
}

void sstf(Request requests[], int n, int current_track) {
    Request temp[14];
    for (int i = 0; i < n; i++) temp[i] = requests[i];
    
    int total_movement = 0;
    char order[50] = "";
    int completed = 0;
    
    while (completed < n) {
        int selected = -1;
        int min_dist = -1;
        
        for (int i = 0; i < n; i++) {
            if (!temp[i].visited) {
                int dist = abs(current_track - temp[i].track);
                if (selected == -1 || dist < min_dist) {
                    min_dist = dist;
                    selected = i;
                }
            }
        }
        
        if (selected != -1) {
            total_movement += abs(current_track - temp[selected].track);
            sprintf(order + strlen(order), "%c ", temp[selected].name);
            current_track = temp[selected].track;
            temp[selected].visited = true;
            completed++;
        }
    }
    
    printf("=== SSTF 最短寻道优先 ===\n");
    printf("访问顺序: %s\n", order);
    printf("总移动道数: %d\n", total_movement);
    printf("平均移动道数: %.2f\n\n", (double)total_movement / n);
}

void scan(Request requests[], int n, int current_track) {
    Request temp[14];
    for (int i = 0; i < n; i++) temp[i] = requests[i];
    
    int total_movement = 0;
    char order[50] = "";
    bool direction_up = true;
    int remaining_count = n;
    
    while (remaining_count > 0) {
        if (direction_up) {
            int selected_idx = -1;
            int min_diff = -1;
            
            for (int i = 0; i < n; i++) {
                if (!temp[i].visited && temp[i].track >= current_track) {
                    int diff = temp[i].track - current_track;
                    if (selected_idx == -1 || diff < min_diff) {
                        min_diff = diff;
                        selected_idx = i;
                    }
                }
            }
            
            if (selected_idx != -1) {
                total_movement += abs(current_track - temp[selected_idx].track);
                sprintf(order + strlen(order), "%c ", temp[selected_idx].name);
                current_track = temp[selected_idx].track;
                temp[selected_idx].visited = true;
                remaining_count--;
            } else {
                direction_up = false;
            }
        } else {
            int selected_idx = -1;
            int max_diff = -1;
            
            for (int i = 0; i < n; i++) {
                if (!temp[i].visited && temp[i].track <= current_track) {
                    int diff = current_track - temp[i].track;
                    if (selected_idx == -1 || diff > max_diff) {
                        max_diff = diff;
                        selected_idx = i;
                    }
                }
            }
            
            if (selected_idx != -1) {
                total_movement += abs(current_track - temp[selected_idx].track);
                sprintf(order + strlen(order), "%c ", temp[selected_idx].name);
                current_track = temp[selected_idx].track;
                temp[selected_idx].visited = true;
                remaining_count--;
            } else {
                direction_up = true;
            }
        }
    }
    
    printf("=== SCAN 电梯调度 ===\n");
    printf("访问顺序: %s\n", order);
    printf("总移动道数: %d\n", total_movement);
    printf("平均移动道数: %.2f\n\n", (double)total_movement / n);
}

int main() {
    Request requests[14] = {
        {'A', 30, false},
        {'B', 50, false},
        {'C', 100, false},
        {'D', 180, false},
        {'E', 20, false},
        {'F', 90, false},
        {'G', 150, false},
        {'H', 70, false},
        {'I', 80, false},
        {'J', 10, false},
        {'K', 160, false},
        {'L', 120, false},
        {'M', 40, false},
        {'N', 110, false}
    };
    
    int current_track = 90;
    
    fcfs(requests, 14, current_track);
    sstf(requests, 14, current_track);
    scan(requests, 14, current_track);
    
    return 0;
}