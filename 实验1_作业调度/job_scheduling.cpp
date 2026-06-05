#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;

struct Job {
    char name;
    int arrival_time;
    int run_time;
    int start_time;
    int finish_time;
    int turnaround_time;
    double weighted_turnaround_time;
    bool completed;
    int remaining_time;
};

void fcfs(vector<Job> jobs) {
    int n = jobs.size();
    int current_time = 0;
    int completed_count = 0;
    vector<char> schedule_order;
    
    while (completed_count < n) {
        vector<int> ready;
        for (int i = 0; i < n; i++) {
            if (!jobs[i].completed && jobs[i].arrival_time <= current_time) {
                ready.push_back(i);
            }
        }
        
        if (!ready.empty()) {
            int idx = ready[0];
            jobs[idx].start_time = current_time;
            jobs[idx].finish_time = current_time + jobs[idx].run_time;
            jobs[idx].turnaround_time = jobs[idx].finish_time - jobs[idx].arrival_time;
            jobs[idx].weighted_turnaround_time = (double)jobs[idx].turnaround_time / jobs[idx].run_time;
            jobs[idx].completed = true;
            schedule_order.push_back(jobs[idx].name);
            current_time = jobs[idx].finish_time;
            completed_count++;
        } else {
            current_time++;
        }
    }
    
    cout << "=== FCFS 先来先服务 ===" << endl;
    cout << "调度顺序: ";
    for (char c : schedule_order) cout << c << " ";
    cout << endl;
    
    double total_turnaround = 0, total_weighted = 0;
    cout << "作业\t到达时间\t运行时间\t完成时间\t周转时间\t带权周转时间" << endl;
    for (auto& job : jobs) {
        total_turnaround += job.turnaround_time;
        total_weighted += job.weighted_turnaround_time;
        cout << job.name << "\t" << job.arrival_time << "\t\t" << job.run_time << "\t\t" 
             << job.finish_time << "\t\t" << job.turnaround_time << "\t\t" 
             << fixed << setprecision(2) << job.weighted_turnaround_time << endl;
    }
    cout << "平均周转时间: " << fixed << setprecision(2) << total_turnaround / n << endl;
    cout << "平均带权周转时间: " << fixed << setprecision(2) << total_weighted / n << endl << endl;
}

void sjf(vector<Job> jobs) {
    int n = jobs.size();
    int current_time = 0;
    int completed_count = 0;
    vector<char> schedule_order;
    
    while (completed_count < n) {
        vector<int> ready;
        for (int i = 0; i < n; i++) {
            if (!jobs[i].completed && jobs[i].arrival_time <= current_time) {
                ready.push_back(i);
            }
        }
        
        if (!ready.empty()) {
            int idx = ready[0];
            for (int i : ready) {
                if (jobs[i].run_time < jobs[idx].run_time) {
                    idx = i;
                }
            }
            jobs[idx].start_time = current_time;
            jobs[idx].finish_time = current_time + jobs[idx].run_time;
            jobs[idx].turnaround_time = jobs[idx].finish_time - jobs[idx].arrival_time;
            jobs[idx].weighted_turnaround_time = (double)jobs[idx].turnaround_time / jobs[idx].run_time;
            jobs[idx].completed = true;
            schedule_order.push_back(jobs[idx].name);
            current_time = jobs[idx].finish_time;
            completed_count++;
        } else {
            current_time++;
        }
    }
    
    cout << "=== SJF 短作业优先 ===" << endl;
    cout << "调度顺序: ";
    for (char c : schedule_order) cout << c << " ";
    cout << endl;
    
    double total_turnaround = 0, total_weighted = 0;
    cout << "作业\t到达时间\t运行时间\t完成时间\t周转时间\t带权周转时间" << endl;
    for (auto& job : jobs) {
        total_turnaround += job.turnaround_time;
        total_weighted += job.weighted_turnaround_time;
        cout << job.name << "\t" << job.arrival_time << "\t\t" << job.run_time << "\t\t" 
             << job.finish_time << "\t\t" << job.turnaround_time << "\t\t" 
             << fixed << setprecision(2) << job.weighted_turnaround_time << endl;
    }
    cout << "平均周转时间: " << fixed << setprecision(2) << total_turnaround / n << endl;
    cout << "平均带权周转时间: " << fixed << setprecision(2) << total_weighted / n << endl << endl;
}

void hrrn(vector<Job> jobs) {
    int n = jobs.size();
    int current_time = 0;
    int completed_count = 0;
    vector<char> schedule_order;
    
    while (completed_count < n) {
        vector<int> ready;
        for (int i = 0; i < n; i++) {
            if (!jobs[i].completed && jobs[i].arrival_time <= current_time) {
                ready.push_back(i);
            }
        }
        
        if (!ready.empty()) {
            int idx = ready[0];
            double highest_ratio = 0;
            for (int i : ready) {
                int wait_time = current_time - jobs[i].arrival_time;
                double ratio = (double)(wait_time + jobs[i].run_time) / jobs[i].run_time;
                if (ratio > highest_ratio) {
                    highest_ratio = ratio;
                    idx = i;
                }
            }
            jobs[idx].start_time = current_time;
            jobs[idx].finish_time = current_time + jobs[idx].run_time;
            jobs[idx].turnaround_time = jobs[idx].finish_time - jobs[idx].arrival_time;
            jobs[idx].weighted_turnaround_time = (double)jobs[idx].turnaround_time / jobs[idx].run_time;
            jobs[idx].completed = true;
            schedule_order.push_back(jobs[idx].name);
            current_time = jobs[idx].finish_time;
            completed_count++;
        } else {
            current_time++;
        }
    }
    
    cout << "=== HRRN 响应比高者优先 ===" << endl;
    cout << "调度顺序: ";
    for (char c : schedule_order) cout << c << " ";
    cout << endl;
    
    double total_turnaround = 0, total_weighted = 0;
    cout << "作业\t到达时间\t运行时间\t完成时间\t周转时间\t带权周转时间" << endl;
    for (auto& job : jobs) {
        total_turnaround += job.turnaround_time;
        total_weighted += job.weighted_turnaround_time;
        cout << job.name << "\t" << job.arrival_time << "\t\t" << job.run_time << "\t\t" 
             << job.finish_time << "\t\t" << job.turnaround_time << "\t\t" 
             << fixed << setprecision(2) << job.weighted_turnaround_time << endl;
    }
    cout << "平均周转时间: " << fixed << setprecision(2) << total_turnaround / n << endl;
    cout << "平均带权周转时间: " << fixed << setprecision(2) << total_weighted / n << endl << endl;
}

void fcfs_parallel(vector<Job> jobs) {
    int n = jobs.size();
    int current_time = 0;
    int completed_count = 0;
    vector<char> schedule_order;
    
    vector<int> running(2, -1);
    vector<int> remaining(2, 0);
    
    while (completed_count < n || running[0] != -1 || running[1] != -1) {
        for (int i = 0; i < 2; i++) {
            if (running[i] != -1) {
                remaining[i]--;
                if (remaining[i] == 0) {
                    int job_idx = running[i];
                    jobs[job_idx].finish_time = current_time + 1;
                    jobs[job_idx].turnaround_time = jobs[job_idx].finish_time - jobs[job_idx].arrival_time;
                    jobs[job_idx].weighted_turnaround_time = (double)jobs[job_idx].turnaround_time / jobs[job_idx].run_time;
                    jobs[job_idx].completed = true;
                    schedule_order.push_back(jobs[job_idx].name);
                    completed_count++;
                    running[i] = -1;
                }
            }
        }
        
        for (int i = 0; i < 2; i++) {
            if (running[i] == -1) {
                int selected = -1;
                for (int j = 0; j < n; j++) {
                    if (!jobs[j].completed && jobs[j].arrival_time <= current_time) {
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
                    remaining[i] = jobs[selected].run_time;
                    jobs[selected].start_time = current_time;
                }
            }
        }
        
        current_time++;
    }
    
    cout << "=== FCFS 先来先服务(双道作业) ===" << endl;
    cout << "调度顺序: ";
    for (char c : schedule_order) cout << c << " ";
    cout << endl;
    
    double total_turnaround = 0, total_weighted = 0;
    cout << "作业\t到达时间\t运行时间\t完成时间\t周转时间\t带权周转时间" << endl;
    for (auto& job : jobs) {
        total_turnaround += job.turnaround_time;
        total_weighted += job.weighted_turnaround_time;
        cout << job.name << "\t" << job.arrival_time << "\t\t" << job.run_time << "\t\t" 
             << job.finish_time << "\t\t" << job.turnaround_time << "\t\t" 
             << fixed << setprecision(2) << job.weighted_turnaround_time << endl;
    }
    cout << "平均周转时间: " << fixed << setprecision(2) << total_turnaround / n << endl;
    cout << "平均带权周转时间: " << fixed << setprecision(2) << total_weighted / n << endl << endl;
}

int main() {
    vector<Job> jobs = {
        {'A', 0, 7, 0, 0, 0, 0, false, 7},
        {'B', 2, 10, 0, 0, 0, 0, false, 10},
        {'C', 5, 20, 0, 0, 0, 0, false, 20},
        {'D', 7, 30, 0, 0, 0, 0, false, 30},
        {'E', 12, 40, 0, 0, 0, 0, false, 40},
        {'F', 15, 8, 0, 0, 0, 0, false, 8},
        {'G', 4, 8, 0, 0, 0, 0, false, 8},
        {'H', 6, 20, 0, 0, 0, 0, false, 20},
        {'I', 8, 10, 0, 0, 0, 0, false, 10},
        {'J', 10, 12, 0, 0, 0, 0, false, 12}
    };
    
    fcfs(jobs);
    sjf(jobs);
    hrrn(jobs);
    fcfs_parallel(jobs);
    
    return 0;
}