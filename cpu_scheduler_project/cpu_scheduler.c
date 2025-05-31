#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define TIME_QUANTUM 20

typedef struct process {
    int pid;
    int priority;
    int computing_time;
    int remaining_time;
    int arrival_time;
    int queue_id;
    struct process* next;
} process_t;

typedef struct out {
    int pid;
    char queue_id[16];
    int priority;
    int computing_time;
    int tat;
    struct out* next;
} out_t;

static int current_time = 0;
static int arrival_clock = 0;

static process_t* gen_head[5] = { NULL };
static process_t* gen_tail[5] = { NULL };
static process_t* rt_head = NULL;
static process_t* rt_tail = NULL;

static out_t* rt_out_head = NULL;
static out_t* rt_out_tail = NULL;
static out_t* gen_out_head = NULL;
static out_t* gen_out_tail = NULL;

int get_queue_id(int prio) {
    if (prio >= 1 && prio <= 10) return 1;
    if (prio <= 20) return 2;
    if (prio <= 30) return 3;
    return 4;
}

void enqueue_real_time(process_t* p) {
    p->next = NULL;
    if (!rt_head) {
        rt_head = rt_tail = p;
    } else {
        rt_tail->next = p;
        rt_tail = p;
    }
}

process_t* dequeue_real_time(void) {
    if (!rt_head) return NULL;
    process_t* p = rt_head;
    rt_head = rt_head->next;
    if (!rt_head) rt_tail = NULL;
    p->next = NULL;
    return p;
}

void enqueue_general(int qid, process_t* p) {
    p->next = NULL;
    if (!gen_head[qid]) {
        gen_head[qid] = gen_tail[qid] = p;
        return;
    }
    process_t* cur = gen_head[qid];
    process_t* prev = NULL;
    while (cur && cur->priority <= p->priority) {
        prev = cur;
        cur = cur->next;
    }
    if (!prev) {
        p->next = gen_head[qid];
        gen_head[qid] = p;
    } else {
        prev->next = p;
        p->next = cur;
        if (!cur) {
            gen_tail[qid] = p;
        }
    }
}

process_t* schedule_general_next(int* out_qid) {
    process_t* best = NULL;
    process_t* prev_best = NULL;
    int best_qid = 0;
    for (int q = 1; q <= 4; ++q) {
        process_t* prev = NULL;
        process_t* cur = gen_head[q];
        while (cur) {
            if (!best || cur->priority < best->priority) {
                best = cur;
                prev_best = prev;
                best_qid = q;
            }
            prev = cur;
            cur = cur->next;
        }
    }
    if (!best) return NULL;
    if (!prev_best) {
        gen_head[best_qid] = best->next;
    } else {
        prev_best->next = best->next;
    }
    if (best == gen_tail[best_qid]) {
        gen_tail[best_qid] = prev_best;
    }
    best->next = NULL;
    *out_qid = best_qid;
    return best;
}

static void store_output(out_t** head, out_t** tail,
    int pid, const char* qid,
    int prio, int ctime, int tat) {
    out_t* o = malloc(sizeof(out_t));
    o->pid = pid;
    strncpy(o->queue_id, qid, sizeof(o->queue_id));
    o->queue_id[sizeof(o->queue_id) - 1] = '\0';
    o->priority = prio;
    o->computing_time = ctime;
    o->tat = tat;
    o->next = NULL;
    if (!*head) {
        *head = *tail = o;
    } else {
        (*tail)->next = o;
        *tail = o;
    }
}

void run_one_quantum(void) {
    int quantum = TIME_QUANTUM;
    while (quantum > 0 && rt_head) {
        process_t* rt = dequeue_real_time();
        if (rt->remaining_time <= quantum) {
            current_time += rt->remaining_time;
            quantum -= rt->remaining_time;
            int tat = current_time - rt->arrival_time;
            store_output(&rt_out_head, &rt_out_tail,
                rt->pid, "real_time",
                rt->priority, rt->computing_time, tat);
            free(rt);
            return;
        } else {
            rt->remaining_time -= quantum;
            current_time += quantum;
            quantum = 0;
            enqueue_real_time(rt);
            return;
        }
    }

    if (quantum > 0) {
        int qid;
        process_t* gp = schedule_general_next(&qid);
        if (!gp) return;

        if (gp->remaining_time <= quantum) {
            current_time += gp->remaining_time;
            quantum -= gp->remaining_time;
            int tat = current_time - gp->arrival_time;
            char buf[16];
            sprintf(buf, "Q%d", gp->queue_id);
            store_output(&gen_out_head, &gen_out_tail,
                gp->pid, buf,
                gp->priority, gp->computing_time, tat);
            free(gp);
            return;
        } else {
            gp->remaining_time -= quantum;
            current_time += quantum;
            quantum = 0;
            gp->priority += 10;
            if (gp->priority > 30) gp->priority = 31;
            gp->queue_id = get_queue_id(gp->priority);
            enqueue_general(gp->queue_id, gp);
            return;
        }
    }
}

int main(void) {
    int type;

    while (scanf("%d", &type) == 1) {
        if (type == -1) break;

        if (type == 0) {
            int pid, prio, ctime;
            scanf("%d %d %d", &pid, &prio, &ctime);
            process_t* p = malloc(sizeof(process_t));
            p->pid = pid;
            p->priority = prio;
            p->computing_time = ctime;
            p->remaining_time = ctime;
            p->arrival_time = arrival_clock;

            if (prio < 0) {
                enqueue_real_time(p);
            } else {
                p->queue_id = get_queue_id(prio);
                enqueue_general(p->queue_id, p);
            }
        } else if (type == 1) {
            int d1, d2, d3;
            scanf("%d %d %d", &d1, &d2, &d3);
            run_one_quantum();
            arrival_clock += TIME_QUANTUM;
        }
    }

    while (rt_head || gen_head[1] || gen_head[2] || gen_head[3] || gen_head[4]) {
        run_one_quantum();
    }

    printf("Process_id  Queue_id   priority  computing_time  turn_around_time\n");
    for (out_t* p = rt_out_head; p; p = p->next) {
        printf("%-11d %-10s %-9d %-15d %d\n",
            p->pid, p->queue_id,
            p->priority, p->computing_time,
            p->tat);
    }
    for (out_t* p = gen_out_head; p; p = p->next) {
        printf("%-11d %-10s %-9d %-15d %d\n",
            p->pid, p->queue_id,
            p->priority, p->computing_time,
            p->tat);
    }

    return 0;
}
