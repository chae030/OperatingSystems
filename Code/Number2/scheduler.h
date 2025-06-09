// scheduler.h
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

void run_scheduler(Process* processes[], int count);
void print_results(Process* processes[], int count);
void print_input_fields(Process* processes[], int count);

#endif