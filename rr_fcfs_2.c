#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 100
#define TIME_QUANTUM 4

typedef struct {
    int process_id;
    int priority;
    int computing_time;
    int remaining_time;
    int arrival_order;
    int turnaround_time;
    int queue_id; // 1 for RR, 2 for FCFS
    int finished;
} Process;

Process queue1[MAX_PROCESSES]; // RR Queue
Process queue2[MAX_PROCESSES]; // FCFS Queue
int q1_front = 0, q1_rear = 0;
int q2_front = 0, q2_rear = 0;

Process completed[MAX_PROCESSES];
int completed_count = 0;
int arrival_count = 0;
int current_time = 0;

// 큐 삽입
void enqueue(Process* queue, int* rear, Process p) {
    queue[(*rear)++] = p;
}

// RR 큐에서 프로세스 실행
void run_rr_queue(void) {
    if (q1_front >= q1_rear) return;
    Process p = queue1[q1_front++];
    int exec_time = (p.remaining_time <= TIME_QUANTUM) ? p.remaining_time : TIME_QUANTUM;
    current_time += exec_time;
    p.remaining_time -= exec_time;

    if (p.remaining_time <= 0) {
        p.turnaround_time = current_time;
        p.queue_id = 1;
        p.finished = 1;
        completed[completed_count++] = p;
    } else {
        enqueue(queue2, &q2_rear, p); // 큐2로 이동
    }
}

// FCFS 큐에서 프로세스 실행
void run_fcfs_queue(void) {
    if (q2_front >= q2_rear) return;
    Process p = queue2[q2_front++];
    current_time += p.remaining_time;
    p.remaining_time = 0;
    p.turnaround_time = current_time;
    p.queue_id = 2;
    p.finished = 1;
    completed[completed_count++] = p;
}

int main(void) {
    int type;
    Process temp;

    // 입력 처리
    while (scanf("%d", &type) == 1) {
        if (type == -1) break;
        if (type == 0) {
            scanf("%d %d %d", &temp.process_id, &temp.priority, &temp.computing_time);
            temp.remaining_time = temp.computing_time;
            temp.arrival_order = arrival_count++;
            temp.finished = 0;
            enqueue(queue1, &q1_rear, temp);
        } else if (type == 1) {
            run_rr_queue(); // time quantum 종료 → RR 프로세스 스케줄링
        }
    }

    // 큐1 먼저 다 돌고, 남은 건 큐2에서 FCFS 방식으로 실행
    while (q1_front < q1_rear) run_rr_queue();
    while (q2_front < q2_rear) run_fcfs_queue();

    // 출력
    printf("\nProcess_id Queue_id computing_time turnaround_time\n");
    for (int i = 0; i < completed_count; i++) {
        printf("%10d Q%d %14d %15d\n",
        completed[i].process_id,
        completed[i].queue_id,
        completed[i].computing_time,
        completed[i].turnaround_time);
    }

    return 0;
}
