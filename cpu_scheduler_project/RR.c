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
    int time_in_current_slice;
    char queue_id[3];
} Process;

Process q1[MAX_PROCESSES], q2[MAX_PROCESSES];
int f1 = 0, r1 = 0, f2 = 0, r2 = 0;
Process input[MAX_PROCESSES]; int total = 0;
Process completed[MAX_PROCESSES]; int completed_count = 0;
int arrived[MAX_PROCESSES] = {0};
int current_time = 0, logical_time = 0;

void enqueue(Process* q, int* rear, Process p) {
    q[(*rear)++] = p;
}
Process dequeue(Process* q, int* front) {
    return q[(*front)++];
}
int is_empty(int front, int rear) {
    return front == rear;
}
int get_queue_id(int priority) {
    return (priority < 10) ? 1 : 2;
}

void simulate(void) {
    Process* running = NULL;

    while (1) {
        // 도착한 프로세스 삽입
        for (int i = 0; i < total; i++) {
            if (!arrived[i] && input[i].arrival_time <= current_time) {
                input[i].remaining_time = input[i].computing_time;
                input[i].time_in_current_slice = 0;
                int qid = get_queue_id(input[i].priority);
                if (qid == 1) enqueue(q1, &r1, input[i]);
                else enqueue(q2, &r2, input[i]);
                arrived[i] = 1;
            }
        }

        // 실행 중인 프로세스가 없으면 꺼내오기
        if (!running) {
            if (!is_empty(f1, r1)) {
                running = (Process*)malloc(sizeof(Process));
                *running = dequeue(q1, &f1);
                strcpy(running->queue_id, "Q1");
            } else if (!is_empty(f2, r2)) {
                running = (Process*)malloc(sizeof(Process));
                *running = dequeue(q2, &f2);
                strcpy(running->queue_id, "Q2");
            }
        }

        // 실행할 게 없고 도착할 프로세스도 없으면 종료
        if (!running && is_empty(f1, r1) && is_empty(f2, r2)) {
            int wait = 0;
            for (int i = 0; i < total; i++) {
                if (!arrived[i]) {
                    current_time++;
                    wait = 1;
                    break;
                }
            }
            if (!wait) break;
            continue;
        }

        if (running) {
            // 클럭 인터럽트: 1초 실행
            running->remaining_time--;
            running->time_in_current_slice++;
            current_time++;

            // 도착 확인
            for (int i = 0; i < total; i++) {
                if (!arrived[i] && input[i].arrival_time <= current_time) {
                    input[i].remaining_time = input[i].computing_time;
                    input[i].time_in_current_slice = 0;
                    int qid = get_queue_id(input[i].priority);
                    if (qid == 1) enqueue(q1, &r1, input[i]);
                    else enqueue(q2, &r2, input[i]);
                    arrived[i] = 1;
                }
            }

            // Q2 실행 중, Q1에 새 프로세스 도착 시 → 선점
            if (strcmp(running->queue_id, "Q2") == 0 && !is_empty(f1, r1)) {
                enqueue(q2, &r2, *running);
                free(running);
                running = NULL;
                continue;
            }

            // 프로세스 완료
            if (running->remaining_time == 0) {
                running->completion_time = current_time;
                running->turnaround_time = current_time - running->arrival_time;
                completed[completed_count++] = *running;
                free(running);
                running = NULL;
            }
            // 타임퀀텀 초과 시 → 큐 뒤로
            else if (running->time_in_current_slice == TIME_QUANTUM) {
                running->time_in_current_slice = 0;
                if (strcmp(running->queue_id, "Q1") == 0) enqueue(q1, &r1, *running);
                else enqueue(q2, &r2, *running);
                free(running);
                running = NULL;
            }
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
                Process p = {pid, prio, time, time, logical_time, 0, 0, 0, ""};
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
