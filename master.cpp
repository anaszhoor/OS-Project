#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <iomanip>
#include <algorithm>

using namespace std;


struct Process {
    int pid;           
    int arrivalTime;    
    int burstTime;      
    int remainingTime;  
    int completionTime; 
    int waitingTime;    
    int turnaroundTime; 
};

// FCFS scheduling
void FCFS(vector<Process> processes) {
    cout << "\n--- First-Come First-Served (FCFS) Scheduling ---\n";
    int time = 0, n = processes.size();
    double totalWaiting = 0, totalTurnaround = 0;

    // Sort processes by arrival time
    sort(processes.begin(), processes.end(), [](Process a, Process b) {
        return a.arrivalTime < b.arrivalTime;
    });

    cout << "Gantt Chart: ";
    for (auto &p : processes) {
        if (time < p.arrivalTime) time = p.arrivalTime; 
        cout << "| P" << p.pid << " ";
        p.completionTime = time + p.burstTime;
        p.turnaroundTime = p.completionTime - p.arrivalTime;
        p.waitingTime = p.turnaroundTime - p.burstTime;
        totalWaiting += p.waitingTime;
        totalTurnaround += p.turnaroundTime;
        time = p.completionTime;
    }
    cout << "|\n";

    cout << "\nP_ID\tArrival\tBurst\tFinish\tTurnaround\tWaiting\n";
    for (auto &p : processes) {
        cout << p.pid << "\t" << p.arrivalTime << "\t" << p.burstTime << "\t" 
             << p.completionTime << "\t" << p.turnaroundTime << "\t\t" << p.waitingTime << "\n";
    }

    cout << "\nAverage Waiting Time: " << totalWaiting / n;
    cout << "\nAverage Turnaround Time: " << totalTurnaround / n;
    cout << "\nCPU Utilization: 100%\n";
}

//(SRT) Scheduling
void SRT(vector<Process> processes) {
    cout << "\n--- Shortest Remaining Time (SRT) Scheduling ---\n";
    int time = 0, completed = 0, n = processes.size();
    double totalWaiting = 0, totalTurnaround = 0;
    vector<int> gantt;

    for (auto &p : processes) p.remainingTime = p.burstTime;

    while (completed < n) {
        int idx = -1, minTime = INT_MAX;


        for (int i = 0; i < n; i++) {
            if (processes[i].arrivalTime <= time && processes[i].remainingTime > 0 && processes[i].remainingTime < minTime) {
                idx = i;
                minTime = processes[i].remainingTime;
            }
        }

        if (idx == -1) {
            time++;
        } else {
            if (gantt.empty() || gantt.back() != processes[idx].pid) {
                gantt.push_back(processes[idx].pid); 
            }

            processes[idx].remainingTime--;
            time++;

            if (processes[idx].remainingTime == 0) {
                completed++;
                processes[idx].completionTime = time;
                processes[idx].turnaroundTime = processes[idx].completionTime - processes[idx].arrivalTime;
                processes[idx].waitingTime = processes[idx].turnaroundTime - processes[idx].burstTime;
                totalWaiting += processes[idx].waitingTime;
                totalTurnaround += processes[idx].turnaroundTime;
            }
        }
    }

    cout << "Gantt Chart: ";
    for (auto pid : gantt) cout << "| P" << pid << " ";
    cout << "|\n";

    cout << "\nP_ID\tArrival\tBurst\tFinish\tTurnaround\tWaiting\n";
    for (auto &p : processes) {
        cout << p.pid << "\t" << p.arrivalTime << "\t" << p.burstTime << "\t" 
             << p.completionTime << "\t" << p.turnaroundTime << "\t\t" << p.waitingTime << "\n";
    }

    cout << "\nAverage Waiting Time: " << totalWaiting / n;
    cout << "\nAverage Turnaround Time: " << totalTurnaround / n;
    cout << "\nCPU Utilization: 100%\n";
}

//Round Robin (RR) Scheduling
void RR(vector<Process> processes, int quantum) {
    cout << "\n--- Round Robin (RR) Scheduling ---\n";
    queue<int> q;
    int time = 0, n = processes.size(), completed = 0;
    double totalWaiting = 0, totalTurnaround = 0;
    vector<int> gantt;

    for (auto &p : processes) p.remainingTime = p.burstTime;

    vector<bool> inQueue(n, false);
    for (int i = 0; i < n; i++) {
        if (processes[i].arrivalTime == 0) {
            q.push(i);
            inQueue[i] = true;
        }
    }

    while (completed < n) {
        if (q.empty()) {
            time++;
            continue;
        }
        int idx = q.front();
        q.pop();
        gantt.push_back(processes[idx].pid);

        if (time == 0 && idx != 0) continue;

        int execTime = min(quantum, processes[idx].remainingTime);
        processes[idx].remainingTime -= execTime;
        time += execTime;

        for (int i = 0; i < n; i++) {
            if (!inQueue[i] && processes[i].arrivalTime <= time && processes[i].remainingTime > 0) {
                q.push(i);
                inQueue[i] = true;
            }
        }

        if (processes[idx].remainingTime == 0) {
            completed++;
            processes[idx].completionTime = time;
            processes[idx].turnaroundTime = processes[idx].completionTime - processes[idx].arrivalTime;
            processes[idx].waitingTime = processes[idx].turnaroundTime - processes[idx].burstTime;
            totalWaiting += processes[idx].waitingTime;
            totalTurnaround += processes[idx].turnaroundTime;
        } else {
            q.push(idx);
        }
    }

    cout << "Gantt Chart: ";
    for (auto pid : gantt) cout << "| P" << pid << " ";
    cout << "|\n";

    cout << "\nP_ID\tArrival\tBurst\tFinish\tTurnaround\tWaiting\n";
    for (auto &p : processes) {
        cout << p.pid << "\t" << p.arrivalTime << "\t" << p.burstTime << "\t" 
             << p.completionTime << "\t" << p.turnaroundTime << "\t\t" << p.waitingTime << "\n";
    }

    cout << "\nAverage Waiting Time: " << totalWaiting / n;
    cout << "\nAverage Turnaround Time: " << totalTurnaround / n;
    cout << "\nCPU Utilization: 100%\n";
}

int main() {
    ifstream file("/Users/m1/Documents/2024:2025/First Simaster/Operating Systems/HW#4/input.txt");
    if (!file) {
        cerr << "Error opening file!\n";
        return 1;
    }

    int n, quantum;
    file >> n;

    vector<Process> processes(n);
    for (int i = 0; i < n; i++) {
        file >> processes[i].pid >> processes[i].arrivalTime >> processes[i].burstTime;
    }
    file >> quantum;

    FCFS(processes);
    SRT(processes);
    RR(processes, quantum);

    return 0;
}

// 5
// 1 0 16
// 2 5 3
// 3 8 5
// 4 4 6
// 5 6 11
// 5

//RR and FCFS
// 3
// 1 0 24 
// 2 0 3 
// 3 0 3  
// 4

//SRT
// 4
// 1 0 8 
// 2 1 4
// 3 2 9 
// 4 3 5
// 2