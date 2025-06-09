// process.c
#include <stdlib.h>
#include "process.h"

Process* create_process(int id, int priority, int computing_time, int arrival) {
    Process* p = (Process*)malloc(sizeof(Process));
    p->process_id = id;
    p->priority = priority;
    p->computing_time = computing_time;
    p->remaining_time = computing_time;
    p->arrival_time = arrival;  // 내부 처리용
    p->finished_time = -1;
    p->turn_around_time = 0;
    return p;
}
