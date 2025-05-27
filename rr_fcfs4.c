#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 100
#define TIME_QUANTUM 20
#define PRIORITY_LEVELS 4

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

Process queues[PRIORITY_LEVELS][MAX_PROCESSES];
int front[PRIORITY_LEVELS] = {0}, rear[PRIORITY_LEVELS] = {0};
Process completed[MAX_PROCESSES]; int completed_count = 0;
Process input[MAX_PROCESSES]; int total = 0;
int current_time = 0, logical_time = 0;

int get_queue_index(int priority) {
    if (priority < 10) return 0;  // Q1 (RR)
    if (priority < 20) return 1;  // Q2 (RR)
    if (priority < 30) return 2;  // Q3 (FCFS)
    return 3;                    // Q4 (FCFS)
}

void enqueue(int qid, Process p) { queues[qid][rear[qid]++] = p; }
Process dequeue(int qid) { return queues[qid][front[qid]++]; }
int is_empty(int qid) { return front[qid] == rear[qid]; }

void simulate(void) {
    int arrived[MAX_PROCESSES] = {0};
    while (1) {
        for (int i = 0; i < total; i++) {
            if (!arrived[i] && input[i].arrival_time <= current_time) {
                input[i].remaining_time = input[i].computing_time;
                int qid = get_queue_index(input[i].priority);
                enqueue(qid, input[i]);
                arrived[i] = 1;
            }
        }

        int found = 0;
        for (int qid = 0; qid < PRIORITY_LEVELS; qid++) {
            if (!is_empty(qid)) {
                Process p = dequeue(qid);
                if (qid <= 1) { // Q1, Q2 → RR with 1-second clock interrupt
                    int executed = 0;
                    while (p.remaining_time > 0 && executed < TIME_QUANTUM) {
                        p.remaining_time--;
                        current_time++;
                        executed++;

                        for (int i = 0; i < total; i++) {
                            if (!arrived[i] && input[i].arrival_time <= current_time) {
                                input[i].remaining_time = input[i].computing_time;
                                int q = get_queue_index(input[i].priority);
                                enqueue(q, input[i]);
                                arrived[i] = 1;
                            }
                        }

                        // 우선순위 높은 큐가 생기면 선점
                        int preempt = 0;
                        for (int h = 0; h < qid; h++) {
                            if (!is_empty(h)) {
                                preempt = 1;
                                break;
                            }
                        }
                        if (preempt) break;
                    }
                } else { // FCFS
                    while (p.remaining_time > 0) {
                        p.remaining_time--;
                        current_time++;

                        for (int i = 0; i < total; i++) {
                            if (!arrived[i] && input[i].arrival_time <= current_time) {
                                input[i].remaining_time = input[i].computing_time;
                                int q = get_queue_index(input[i].priority);
                                enqueue(q, input[i]);
                                arrived[i] = 1;
                            }
                        }

                        int preempt = 0;
                        for (int h = 0; h < qid; h++) {
                            if (!is_empty(h)) {
                                preempt = 1;
                                break;
                            }
                        }
                        if (preempt) {
                            enqueue(qid, p);
                            found = 1;
                            break;
                        }
                    }
                }

                if (p.remaining_time == 0) {
                    p.completion_time = current_time;
                    p.turnaround_time = current_time - p.arrival_time;
                    sprintf(p.queue_id, "Q%d", qid + 1);
                    completed[completed_count++] = p;
                } else if (qid <= 1 || found == 0) {
                    enqueue(qid, p);
                }
                found = 1;
                break;
            }
        }

        if (!found) {
            int still_remaining = 0;
            for (int i = 0; i < total; i++) {
                if (!arrived[i]) {
                    still_remaining = 1;
                    current_time++;
                    break;
                }
            }
            if (!still_remaining) break;
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
        printf("%-10d %-8s %-8d %-15d %-15d\n",
        completed[i].pid,
        completed[i].queue_id,
        completed[i].priority,
        completed[i].computing_time,
        completed[i].turnaround_time);
    }
    return 0;
}
