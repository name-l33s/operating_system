#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

using namespace std;

struct Request {
    char name;
    int track;
    bool visited;
};

void fcfs(vector<Request> requests, int current_track) {
    int n = requests.size();
    int total_movement = 0;
    vector<char> access_order;
    
    for (auto& req : requests) {
        total_movement += abs(current_track - req.track);
        access_order.push_back(req.name);
        current_track = req.track;
    }
    
    cout << "=== FCFS 先来先服务 ===" << endl;
    cout << "访问顺序: ";
    for (char c : access_order) cout << c << " ";
    cout << endl;
    cout << "总移动道数: " << total_movement << endl;
    cout << "平均移动道数: " << fixed << setprecision(2) << (double)total_movement / n << endl << endl;
}

void sstf(vector<Request> requests, int current_track) {
    int n = requests.size();
    int total_movement = 0;
    vector<char> access_order;
    int completed = 0;
    
    while (completed < n) {
        int min_dist = -1;
        int selected = -1;
        
        for (int i = 0; i < n; i++) {
            if (!requests[i].visited) {
                int dist = abs(current_track - requests[i].track);
                if (selected == -1 || dist < min_dist) {
                    min_dist = dist;
                    selected = i;
                }
            }
        }
        
        if (selected != -1) {
            total_movement += abs(current_track - requests[selected].track);
            access_order.push_back(requests[selected].name);
            current_track = requests[selected].track;
            requests[selected].visited = true;
            completed++;
        }
    }
    
    cout << "=== SSTF 最短寻道优先 ===" << endl;
    cout << "访问顺序: ";
    for (char c : access_order) cout << c << " ";
    cout << endl;
    cout << "总移动道数: " << total_movement << endl;
    cout << "平均移动道数: " << fixed << setprecision(2) << (double)total_movement / n << endl << endl;
}

void scan(vector<Request> requests, int current_track) {
    int n = requests.size();
    int total_movement = 0;
    vector<char> access_order;
    bool direction_up = true;
    
    vector<Request> remaining = requests;
    
    while (!remaining.empty()) {
        if (direction_up) {
            int selected_idx = -1;
            int min_diff = -1;
            
            for (int i = 0; i < remaining.size(); i++) {
                if (remaining[i].track >= current_track) {
                    int diff = remaining[i].track - current_track;
                    if (selected_idx == -1 || diff < min_diff) {
                        min_diff = diff;
                        selected_idx = i;
                    }
                }
            }
            
            if (selected_idx != -1) {
                total_movement += abs(current_track - remaining[selected_idx].track);
                access_order.push_back(remaining[selected_idx].name);
                current_track = remaining[selected_idx].track;
                remaining.erase(remaining.begin() + selected_idx);
            } else {
                direction_up = false;
            }
        } else {
            int selected_idx = -1;
            int max_diff = -1;
            
            for (int i = 0; i < remaining.size(); i++) {
                if (remaining[i].track <= current_track) {
                    int diff = current_track - remaining[i].track;
                    if (selected_idx == -1 || diff > max_diff) {
                        max_diff = diff;
                        selected_idx = i;
                    }
                }
            }
            
            if (selected_idx != -1) {
                total_movement += abs(current_track - remaining[selected_idx].track);
                access_order.push_back(remaining[selected_idx].name);
                current_track = remaining[selected_idx].track;
                remaining.erase(remaining.begin() + selected_idx);
            } else {
                direction_up = true;
            }
        }
    }
    
    cout << "=== SCAN 电梯调度 ===" << endl;
    cout << "访问顺序: ";
    for (char c : access_order) cout << c << " ";
    cout << endl;
    cout << "总移动道数: " << total_movement << endl;
    cout << "平均移动道数: " << fixed << setprecision(2) << (double)total_movement / n << endl << endl;
}

int main() {
    vector<Request> requests = {
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
    
    fcfs(requests, current_track);
    sstf(requests, current_track);
    scan(requests, current_track);
    
    return 0;
}