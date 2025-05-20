#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 100
#define TIME_QUANTUM 4

typedef struct {
    char name[5];
    int arrival_time;
    int burst_time;
    int remaining_time;
    int completed_time;
    int waiting_time;
    int turnaround_time;
    int started;
} Process;

int time = 0;
int done = 0;

Process rr_queue[MAX_PROCESSES];
int rr_front = 0, rr_rear = 0;

Process fcfs_queue[MAX_PROCESSES];
int fcfs_front = 0, fcfs_rear = 0;

Process completed[MAX_PROCESSES];
int completed_count = 0;

void enqueue_rr(Process p) {
    rr_queue[rr_rear++] = p;
}

Process dequeue_rr(void) {
    return rr_queue[rr_front++];
}

void enqueue_fcfs(Process p) {
    fcfs_queue[fcfs_rear++] = p;
}

Process dequeue_fcfs(void) {
    return fcfs_queue[fcfs_front++];
}

int is_rr_empty(void) {
    return rr_front == rr_rear;
}

int is_fcfs_empty(void) {
    return fcfs_front == fcfs_rear;
}

void simulate(Process processes[], int n) {
    int index = 0;
    time = 0;

    while (done < n) {
        // 도착한 프로세스 RR 큐에 넣기
        while (index < n && processes[index].arrival_time <= time) {
            processes[index].remaining_time = processes[index].burst_time;
            enqueue_rr(processes[index]);
            index++;
        }

        // Round Robin 큐가 우선
        if (!is_rr_empty()) {
            Process p = dequeue_rr();

            if (!p.started) {
                p.started = 1;
            }

            int exec_time = (p.remaining_time < TIME_QUANTUM) ? p.remaining_time : TIME_QUANTUM;
            time += exec_time;
            p.remaining_time -= exec_time;

            // 도중에 도착한 프로세스는 즉시 RR 큐에 넣기
            while (index < n && processes[index].arrival_time <= time) {
                processes[index].remaining_time = processes[index].burst_time;
                enqueue_rr(processes[index]);
                index++;
            }

            if (p.remaining_time > 0) {
                // RR 큐에서 타임퀀텀 지나고도 안끝나면 FCFS로 이동
                enqueue_fcfs(p);
            } else {
                // 완료 처리
                p.completed_time = time;
                p.turnaround_time = p.completed_time - p.arrival_time;
                p.waiting_time = p.turnaround_time - p.burst_time;
                completed[completed_count++] = p;
                done++;
            }
        } else if (!is_fcfs_empty()) {
            // RR 큐가 비었으면 FCFS 큐 실행
            Process p = dequeue_fcfs();
            time += p.remaining_time;
            p.remaining_time = 0;
            p.completed_time = time;
            p.turnaround_time = p.completed_time - p.arrival_time;
            p.waiting_time = p.turnaround_time - p.burst_time;
            completed[completed_count++] = p;
            done++;
        } else {
            // 아무도 도착하지 않았다면 시간 증가
            time++;
        }
    }
}

void print_results(int n) {
    printf("\n--- 결과 ---\n");
    printf("프로세스\t도착\t버스트\t완료\t대기\t턴어라운드\n");

    double total_waiting = 0, total_turnaround = 0;
    for (int i = 0; i < n; i++) {
        Process p = completed[i];
        printf("%s\t\t%d\t%d\t%d\t%d\t%d\n", p.name, p.arrival_time, p.burst_time, p.completed_time, p.waiting_time, p.turnaround_time);
        total_waiting += p.waiting_time;
        total_turnaround += p.turnaround_time;
    }

    printf("\n평균 대기 시간: %.2f\n", total_waiting / n);
    printf("평균 턴어라운드 시간: %.2f\n", total_turnaround / n);
}

int main(void) {
    int n;
    printf("프로세스 개수 입력: ");
    scanf("%d", &n);

    Process processes[MAX_PROCESSES];

    for (int i = 0; i < n; i++) {
        printf("\n프로세스 이름 (예: P1): ");
        scanf("%s", processes[i].name);
        printf("도착 시간: ");
        scanf("%d", &processes[i].arrival_time);
        printf("버스트 시간: ");
        scanf("%d", &processes[i].burst_time);
        processes[i].started = 0;
    }

    // 도착 시간 기준 정렬
    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++)
            if (processes[i].arrival_time > processes[j].arrival_time) {
                Process temp = processes[i];
                processes[i] = processes[j];
                processes[j] = temp;
            }

    simulate(processes, n);
    print_results(n);

    return 0;
}
