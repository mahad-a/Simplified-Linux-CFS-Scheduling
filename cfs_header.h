#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <sched.h>
#include <string.h>
#include <unistd.h>

#define NUM_CONSUMERS 4
#define NUM_ITEMS_PER_CONSUMER 3
#define MAX_TASKS 256
#define MAX_SLEEP_AVG 10000
#define READY_QUEUE_SIZE 25

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

pthread_t consumer_threads[NUM_CONSUMERS]; // initialize the consumer threads
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // initialize the mutex

// Types of processing types
enum sched_policy {
    NORMAL,
    RR,
    FIFO
};

// Structure to store process information
struct task_struct {
    int pid;
    int dynamic_priority;
    int initialized;
    enum sched_policy process_type;
    int static_priority;
    int CPU_AFFINITY;
    int remain_time;
    int time_slice;
    int accu_time_slice;
    int last_cpu;
    int sleep_avg;
    bool is_blocked;
    time_t blocked_duration;
};

// Ready Queues
struct queue_t {
    struct task_struct RQ0[READY_QUEUE_SIZE];
    struct task_struct RQ1[READY_QUEUE_SIZE];
    struct task_struct RQ2[READY_QUEUE_SIZE];
};
