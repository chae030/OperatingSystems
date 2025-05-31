#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 100
#define TIME_QUANTUM 20

typedef struct {
    int pid;
    int priority;
    int computing_time;
    int remaining_time;
    int arrival_time;
    int completion_time;
    int turnaround_time;
    char queue_id[10];
} Process;

Process rr_queue[MAX_PROCESSES]; int rr_front = 0, rr_rear = 0;
Process fcfs_queue[MAX_PROCESSES]; int fcfs_front = 0, fcfs_rear = 0;
Process completed[MAX_PROCESSES]; int completed_count = 0;
Process input[MAX_PROCESSES]; int total = 0;
int current_time = 0, logical_time = 0;

void enqueue_rr(Process p) { rr_queue[rr_rear++] = p; }
Process dequeue_rr(void) { return rr_queue[rr_front++]; }
int is_rr_empty(void) { return rr_front == rr_rear; }

void enqueue_fcfs(Process p) { fcfs_queue[fcfs_rear++] = p; }
Process dequeue_fcfs(void) { return fcfs_queue[fcfs_front++]; }
int is_fcfs_empty(void) { return fcfs_front == fcfs_rear; }

void simulate(void) {
    int arrived[MAX_PROCESSES] = {0};
    while (1) {
        for (int i = 0; i < total; i++) {
            if (!arrived[i] && input[i].arrival_time <= current_time) {
                input[i].remaining_time = input[i].computing_time;
                enqueue_rr(input[i]);
                arrived[i] = 1;
            }
        }

        if (!is_rr_empty()) {
            Process p = dequeue_rr();
            int exec = (p.remaining_time > TIME_QUANTUM) ? TIME_QUANTUM : p.remaining_time;
            current_time += exec;
            p.remaining_time -= exec;

            for (int i = 0; i < total; i++) {
                if (!arrived[i] && input[i].arrival_time <= current_time) {
                    input[i].remaining_time = input[i].computing_time;
                    enqueue_rr(input[i]);
                    arrived[i] = 1;
                }
            }

            if (p.remaining_time > 0) {
                enqueue_fcfs(p);
            } else {
                p.completion_time = current_time;
                p.turnaround_time = p.completion_time - p.arrival_time;
                strcpy(p.queue_id, "RR");
                completed[completed_count++] = p;
            }
        } else if (!is_fcfs_empty()) {
            Process p = dequeue_fcfs();
            while (p.remaining_time > 0) {
                current_time++;
                p.remaining_time--;

                for (int i = 0; i < total; i++) {
                    if (!arrived[i] && input[i].arrival_time <= current_time) {
                        input[i].remaining_time = input[i].computing_time;
                        enqueue_rr(input[i]);
                        arrived[i] = 1;
                    }
                }

                if (!is_rr_empty()) {
                    enqueue_fcfs(p); // 다시 FCFS 큐로 넣고 RR 먼저 처리
                    break;
                }

                if (p.remaining_time == 0) {
                    p.completion_time = current_time;
                    p.turnaround_time = p.completion_time - p.arrival_time;
                    strcpy(p.queue_id, "FCFS");
                    completed[completed_count++] = p;
                }
            }
        } else {
            int remain = 0;
            for (int i = 0; i < total; i++) {
                if (!arrived[i]) {
                    remain = 1;
                    current_time++;
                    break;
                }
            }
            if (!remain) break;
        }
    }
}

int main(void) {
    char line[100];
    int type;
    while (fgets(line, sizeof(line), stdin)) {
        if (sscanf(line, "%d", &type) != 1) continue;
        if (type == -1) break;

        if (type == 0) {
            int pid, prio, time;
            if (sscanf(line, "%d %d %d %d", &type, &pid, &prio, &time) == 4) {
                Process p = {pid, prio, time, time, logical_time, 0, 0, ""};
                input[total++] = p;
            }
        } else if (type == 1) {
            logical_time += TIME_QUANTUM;
        }
    }

    simulate();

    printf("Process_id Queue_id priority computing_time turn_around time\n");
    for (int i = 0; i < completed_count; i++) {
        printf("%d %s %d %d %d\n",
        completed[i].pid,
        completed[i].queue_id,
        completed[i].priority,
        completed[i].computing_time,
        completed[i].turnaround_time);
    }
    return 0;
}
