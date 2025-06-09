// process.h
#ifndef PROCESS_H
#define PROCESS_H

#define MAX_PROCESSES 100

typedef struct {
    int process_id;
    int priority;
    int computing_time;
    int remaining_time;
    int arrival_time;
    int finished_time;
    int turn_around_time;
} Process;

Process* create_process(int id, int priority, int computing_time, int arrival);

#endif