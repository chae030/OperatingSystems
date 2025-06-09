// scheduler.c (3개 큐 기반 HRRN + RR 구현)
#include <stdio.h>
#include "scheduler.h"

#define TIME_QUANTUM 10
#define HIGH_PRIORITY_MAX 9
#define MID_PRIORITY_MAX 19

Process* queue0[MAX_PROCESSES];
int q0_size = 0;
Process* queue1[MAX_PROCESSES];
int q1_size = 0;
Process* queue2[MAX_PROCESSES];
int q2_size = 0;

float calculate_response_ratio(Process* p, int current_time) {
    int waiting_time = current_time - p->arrival_time;
    return (float)(waiting_time + p->computing_time) / p->computing_time;
}

void enqueue_by_priority(Process* p) {
    if (p->priority <= HIGH_PRIORITY_MAX) queue0[q0_size++] = p;
    else if (p->priority <= MID_PRIORITY_MAX) queue1[q1_size++] = p;
    else queue2[q2_size++] = p;
}

Process* select_process(Process* queue[], int size, int current_time) {
    float max_ratio = -1.0f;
    Process* selected = NULL;
    for (int i = 0; i < size; i++) {
        Process* p = queue[i];
        if (p->remaining_time > 0 && p->arrival_time <= current_time) {
            float rr = calculate_response_ratio(p, current_time);
            if (rr > max_ratio) {
                max_ratio = rr;
                selected = p;
            }
        }
    }
    return selected;
}

void run_scheduler(Process* processes[], int count) {
    int current_time = 0;
    int completed = 0;
    for (int i = 0; i < count; i++) enqueue_by_priority(processes[i]);

    while (completed < count) {
        Process* selected = NULL;

        selected = select_process(queue0, q0_size, current_time);
        if (!selected) selected = select_process(queue1, q1_size, current_time);
        if (!selected) selected = select_process(queue2, q2_size, current_time);

        if (!selected) {
            current_time++;
            continue;
        }

        int exec_time = (selected->remaining_time > TIME_QUANTUM) ? TIME_QUANTUM : selected->remaining_time;
        current_time += exec_time;
        selected->remaining_time -= exec_time;

        if (selected->remaining_time == 0) {
            selected->finished_time = current_time;
            selected->turn_around_time = selected->finished_time - selected->arrival_time;
            completed++;
        }
    }
}

void print_results(Process* processes[], int count) {
    printf("\n[결과 출력]\n");
    printf("Process_id\tPriority\tComputing_time\tTurnaround_time\tNormalized_TAT\n");

    // nTAT 합을 누적할 변수
    double sum_nTAT = 0.0;

    for (int i = 0; i < count; i++) {
        Process* p = processes[i];
        double nTAT = (double)p->turn_around_time / (double)p->computing_time;
        sum_nTAT += nTAT;

        printf("%-10d\t%-8d\t%-14d\t%-14d\t%.2f\n",
            p->process_id,
            p->priority,
            p->computing_time,
            p->turn_around_time,
            nTAT);
    }

    // 평균 nTAT 계산 및 출력
    double avg_nTAT = sum_nTAT / (double)count;
    printf("\nAverage normalized TAT = %.2f\n", avg_nTAT);
}


void print_input_fields(Process* processes[], int count) {
    printf("\n[입력 데이터]\n");
    printf("Process_id\tPriority\tComputing_time\n");
    for (int i = 0; i < count; i++) {
        printf("%d\t\t%d\t\t%d\n",
            processes[i]->process_id,
            processes[i]->priority,
            processes[i]->computing_time);
    }
}