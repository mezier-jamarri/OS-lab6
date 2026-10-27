// C program for implementation of Simulation 
#include<stdio.h> 
#include<limits.h>
#include<stdlib.h>
#include<stdbool.h>
#include "process.h"
#include "util.h"


// Function to find the waiting time for all processes (Round Robin)
void findWaitingTimeRR(ProcessType plist[], int n, int quantum) 
{ 
    int rem_bt[n];
    for (int i = 0 ; i < n ; i++) {
        rem_bt[i] = plist[i].bt;
        plist[i].wt = 0;
    }

    int t = 0; 

    while (1) {
        bool done = true;
        bool idle = true; // Tracks if CPU is idling waiting for a process to arrive

        for (int i = 0 ; i < n; i++) {
            // Only process if it has remaining burst time AND it has arrived
            if (rem_bt[i] > 0) {
                done = false; 
                if (plist[i].art <= t) {
                    idle = false;
                    if (rem_bt[i] > quantum) {
                        t += quantum;
                        rem_bt[i] -= quantum;
                    } else {
                        t += rem_bt[i];
                        // Wait time = completion time (t) - burst time - arrival time
                        plist[i].wt = t - plist[i].bt - plist[i].art; 
                        rem_bt[i] = 0;
                    }
                }
            }
        }
        if (done == true) {
            break;
        }
        
        // If all remaining processes have an arrival time > current time, advance time
        if (idle == true) {
            t++;
        }
    }
}
// Function to find the waiting time for all  
// processes 
void findWaitingTimeSJF(ProcessType plist[], int n)
{ 
    int rem_bt[n];
    for (int i = 0; i < n; i++)
        rem_bt[i] = plist[i].bt;

    int complete = 0, t = 0, minm = INT_MAX;
    int shortest = 0, finish_time;
    bool check = false;

    while (complete != n) {
        for (int j = 0; j < n; j++) {
            if ((plist[j].art <= t) && (rem_bt[j] < minm) && rem_bt[j] > 0) {
                minm = rem_bt[j];
                shortest = j;
                check = true;
            }
        }

        if (check == false) {
            t++;
            continue;
        }

        rem_bt[shortest]--;
        minm = rem_bt[shortest];
        if (minm == 0)
            minm = INT_MAX;

        if (rem_bt[shortest] == 0) {
            complete++;
            check = false;
            finish_time = t + 1;
            plist[shortest].wt = finish_time - plist[shortest].bt - plist[shortest].art;

            if (plist[shortest].wt < 0)
                plist[shortest].wt = 0;
        }
        t++;
    }
} 

// Function to find the waiting time for all processes (FCFS)
void findWaitingTime(ProcessType plist[], int n)
{ 
    int current_time = 0;
    for (int i = 0; i < n; i++) {
        // If the CPU is idle before this process arrives, jump forward to its arrival time
        if (current_time < plist[i].art) {
            current_time = plist[i].art;
        }
        // Wait time is the time execution starts minus the arrival time
        plist[i].wt = current_time - plist[i].art; 
        current_time += plist[i].bt;
    }
}  
// Function to calculate turn around time 
void findTurnAroundTime( ProcessType plist[], int n)
{ 
    // calculating turnaround time by adding bt[i] + wt[i] 
    for (int  i = 0; i < n ; i++) 
        plist[i].tat = plist[i].bt + plist[i].wt; 
} 
  
// Function to sort the Process acc. to priority
int my_comparer(const void *this, const void *that)
{ 
    ProcessType *p1 = (ProcessType *)this;
    ProcessType *p2 = (ProcessType *)that;
    
    // return 1 if this->pri < that->pri (higher number means lower priority in sorting to front, adjust if necessary based on reference implementation)
    if (p1->pri > p2->pri) return 1;
    if (p1->pri < p2->pri) return -1;
    return 0;
} 

//Function to calculate average time 
void findavgTimeFCFS( ProcessType plist[], int n) 
{ 
    findWaitingTime(plist, n); 
    findTurnAroundTime(plist, n); 
    printf("\n*********\nFCFS\n");
}

//Function to calculate average time 
void findavgTimeSJF( ProcessType plist[], int n) 
{ 
    findWaitingTimeSJF(plist, n); 
    findTurnAroundTime(plist, n); 
    printf("\n*********\nSJF\n");
}

//Function to calculate average time 
void findavgTimeRR( ProcessType plist[], int n, int quantum) 
{ 
    findWaitingTimeRR(plist, n, quantum); 
    findTurnAroundTime(plist, n); 
    printf("\n*********\nRR Quantum = %d\n", quantum);
}

//Function to calculate average time 
void findavgTimePriority( ProcessType plist[], int n) 
{ 
    qsort(plist, n, sizeof(ProcessType), my_comparer);
    findWaitingTime(plist, n); 
    findTurnAroundTime(plist, n); 
    printf("\n*********\nPriority\n");
}

void printMetrics(ProcessType plist[], int n)
{
    int total_wt = 0, total_tat = 0; 
    float awt, att;
    
    printf("\tProcesses\tBurst time\tWaiting time\tTurn around time\n"); 
  
    for (int  i=0; i<n; i++) 
    { 
        total_wt = total_wt + plist[i].wt; 
        total_tat = total_tat + plist[i].tat; 
        printf("\t%d\t\t%d\t\t%d\t\t%d\n", plist[i].pid, plist[i].bt, plist[i].wt, plist[i].tat); 
    } 
  
    awt = ((float)total_wt / (float)n);
    att = ((float)total_tat / (float)n);
    
    printf("\nAverage waiting time = %.2f", awt); 
    printf("\nAverage turn around time = %.2f\n", att); 
} 

ProcessType * initProc(char *filename, int *n) 
{
  	FILE *input_file = fopen(filename, "r");
	  if (!input_file) {
		    fprintf(stderr, "Error: Invalid filepath\n");
		    fflush(stdout);
		    exit(0);
	  }

    ProcessType *plist = parse_file(input_file, n);
  
    fclose(input_file);
  
    return plist;
}
  
// Driver code 
int main(int argc, char *argv[]) 
{ 
    int n; 
    int quantum = 2;

    if (argc < 2) {
        fprintf(stderr, "Usage: ./schedsim <input-file-path>\n");
        fflush(stdout);
        return 1;
    }
    
    // Read the file exactly once to prevent redundant I/O operations
    ProcessType *base_proc_list = initProc(argv[1], &n);
    ProcessType proc_list[n]; 
    
    // FCFS
    for(int i = 0; i < n; i++) proc_list[i] = base_proc_list[i]; // Copy array
    findavgTimeFCFS(proc_list, n);
    printMetrics(proc_list, n);
  
    // SJF
    for(int i = 0; i < n; i++) proc_list[i] = base_proc_list[i]; 
    findavgTimeSJF(proc_list, n); 
    printMetrics(proc_list, n);
  
    // Priority
    for(int i = 0; i < n; i++) proc_list[i] = base_proc_list[i]; 
    findavgTimePriority(proc_list, n); 
    printMetrics(proc_list, n);
    
    // RR
    for(int i = 0; i < n; i++) proc_list[i] = base_proc_list[i]; 
    findavgTimeRR(proc_list, n, quantum); 
    printMetrics(proc_list, n);
    
    free(base_proc_list);
    return 0; 
}