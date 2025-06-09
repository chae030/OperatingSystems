// main.c
#define CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "process.h"
#include "scheduler.h"

int main() {
    Process* processes[MAX_PROCESSES];
    int process_count = 0;

    printf("입력: type   process_id   priority   computing_time (-1 종료)\n");
    while (1) {
        int type;
        scanf_s("%d", &type);
        if (type == -1) break;

        if (type == 0) {
            int id, prio, time;
            scanf_s("%d %d %d", &id, &prio, &time);
            Process* p = create_process(id, prio, time, process_count); // arrival = 입력 순서
            processes[process_count++] = p;
        }

        if (type == 1) {
            int dummy1, dummy2, dummy3;
            scanf_s("%d %d %d", &dummy1, &dummy2, &dummy3);
            // 무시
            continue;
        }
    }

    print_input_fields(processes, process_count);
    run_scheduler(processes, process_count);
    print_results(processes, process_count);

    return 0;
}
