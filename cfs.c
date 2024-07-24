#include "cfs_header.h"

// Global array of tasks
struct task_struct tasks[MAX_TASKS];

struct task_struct empty_task;

// Ready queues for each consumer 
struct queue_t ready_queues[NUM_CONSUMERS];

struct task_struct empty_queue[1000];

// Declare buffer globally
struct task_struct buffer[NUM_CONSUMERS][NUM_ITEMS_PER_CONSUMER];

// Function to determine what is the processing type of the task
enum sched_policy get_process_type(const char *policy_str) {
    if (strcmp(policy_str, "NORMAL") == 0) {
        return NORMAL;
    } else if (strcmp(policy_str, "RR") == 0) {
        return RR;
    } else if (strcmp(policy_str, "FIFO") == 0) {
        return FIFO;
    } else {
        // Default to NORMAL if the string doesn't match any known type
        return NORMAL;
    }
}

// Checks if the task is initialized
// In other words if the index in the queue is occupied by a task
bool not_initialized(struct task_struct task_data){
    return !task_data.initialized;
}

// Print out and display the task
int print_task(struct task_struct *task){
    if (task->pid != 0) { // if there is a task exist
        if (task->process_type == 0) { // NORMAL
            printf("    PID: %d, Process Type: %s, \n   Priority: %d, Execution Time: %d, CPU Affinity: %d\n",
                   task->pid, "NORMAL", task->static_priority,
                   task->remain_time, task->CPU_AFFINITY);
        } else if (task->process_type == 1) { // RR (Round Robin)
            printf("    PID: %d, Process Type: %s,  \n  Priority: %d, Execution Time: %d, CPU Affinity: %d\n",
                   task->pid, "RR", task->static_priority,
                   task->remain_time, task->CPU_AFFINITY);
        } else if (task->process_type == 2) { // FIFO (First In First Out)
            printf("    PID: %d, Process Type: %s,  \n  Priority: %d, Execution Time: %d, CPU Affinity: %d\n",
                   task->pid, "FIFO", task->static_priority,
                   task->remain_time, task->CPU_AFFINITY);
        }
    }
    return 0;
}

// Print and display the queue
void print_queue(struct task_struct queue[], int size) {
    for (int i = 0; i < size; i++) {
        if (not_initialized(queue[i])) { // if element at i is empty
            continue;
        } else { // else print out the task and location in queue
            printf("{ Position %d:\n", i);
            print_task(&queue[i]);
            printf("}\n");
        }
    }
}

// insertion sort to organize the ready queues
void insertion_sort(struct task_struct queue[], int n) {
    int i, j;
    struct task_struct key;
    for (i = 1; i < n; i++) {
        key = queue[i];
        j = i - 1;
        while (j >= 0 && queue[j].static_priority < key.static_priority) {
            queue[j + 1] = queue[j];
            j = j - 1;
        }
        queue[j + 1] = key;
    }
}

// Add task to one of the ready queues, n represents which CPU (consumer) to enter
int add_to_queue(struct task_struct new_task, int n){
    pthread_mutex_lock(&mutex); // aquire mutex
    if (0 <= new_task.static_priority && new_task.static_priority < 100){
        // enter ready queue 0
        for (int i = 0; i < READY_QUEUE_SIZE; i++){
            if (not_initialized(ready_queues[n].RQ0[i])){ // element in queue is empty
                ready_queues[n].RQ0[i] = new_task; // insert task into space
                ready_queues[n].RQ0[i].initialized = 1; // initialze and confirm space is occupied
                insertion_sort(ready_queues[n].RQ0, i+1); // insertion sort to organize ready queue from highest priority to lowest
                pthread_mutex_unlock(&mutex); // release mutex
                return 0;
            }
        }

    } else if (100 <= new_task.static_priority && new_task.static_priority < 130){
        // enter ready queue 1
        for (int i = 0; i < READY_QUEUE_SIZE; i++){
            if (not_initialized(ready_queues[n].RQ1[i])){ // element in queue is empty
                ready_queues[n].RQ1[i] = new_task; // insert task into space
                ready_queues[n].RQ1[i].initialized = 1; // initialze and confirm space is occupied
                insertion_sort(ready_queues[n].RQ1, i+1); // insertion sort to organize ready queue from highest priority to lowest
                pthread_mutex_unlock(&mutex); // release mutex
                return 1;
            }
        }

    } else if (130 <= new_task.static_priority && new_task.static_priority < 140) {
        // enter ready queue 2
        for (int i = 0; i < READY_QUEUE_SIZE; i++){
            if (not_initialized(ready_queues[n].RQ2[i])){ // element in queue is empty
                ready_queues[n].RQ2[i] = new_task; // insert task into space
                ready_queues[n].RQ2[i].initialized = 1; // initialze and confirm space is occupied
                insertion_sort(ready_queues[n].RQ2, i+1); // insertion sort to organize ready queue from highest priority to lowest
                pthread_mutex_unlock(&mutex); // release mutex
                return 2;
            }
        }
    }

    pthread_mutex_unlock(&mutex); // release mutex if program never enters any if statement
    return -1;
}

void remove_from_queue(struct task_struct del_task, int n){
    pthread_mutex_lock(&mutex); // lock mutex
    if (0 <= del_task.static_priority && del_task.static_priority < 100){
        // enter ready queue 0
        for (int i = 0; i < READY_QUEUE_SIZE;i++){
            if (ready_queues[n].RQ0[i].pid == del_task.pid){ // check if element in queue is same as task we want to delete
                ready_queues[n].RQ0[i] = empty_task; // make element empty
                insertion_sort(ready_queues[n].RQ0, i+1); // re-sort ready queue
            }
        }
    } else if (100 <= del_task.static_priority && del_task.static_priority < 130){
        // enter ready queue 1
        for (int i = 0; i < READY_QUEUE_SIZE;i++){
            if (ready_queues[n].RQ1[i].pid == del_task.pid ){ // check if element in queue is same as task we want to delete
                ready_queues[n].RQ1[i] = empty_task; // make element empty
                insertion_sort(ready_queues[n].RQ1, i+1); // re-sort ready queue
            }
        }
    } else if (130 <= del_task.static_priority && del_task.static_priority < 140) {
        // enter ready queue 2
        for (int i = 0; i < READY_QUEUE_SIZE;i++){
            if (ready_queues[n].RQ2[i].pid == del_task.pid ){ // check if element in queue is same as task we want to delete
                ready_queues[n].RQ2[i] = empty_task; // make element empty
                insertion_sort(ready_queues[n].RQ2, i+1); // re-sort ready queue
            }
        }
    }

    pthread_mutex_unlock(&mutex); // unlock mutex
}

// Method for SCHED_NORMAL
void sched_normal(struct task_struct process){
    printf("NORMAL SCHEDULING--------------- PID: %d\n", process.pid);
    process.dynamic_priority = max(100, min(process.dynamic_priority - process.remain_time + 5, 130)); // Dynamic Priority calculation
    remove_from_queue(process, process.CPU_AFFINITY); // remove process from given consumer
    if (process.is_blocked == true && process.process_type == 0){ // 0 = NORMAL
        long int unblocked_time = time(NULL); // save time stamp for when its unblocked
        // check if sleep_avg reached max, if not then increment by time spent blocked (sleeping)
        if (process.sleep_avg < MAX_SLEEP_AVG) process.sleep_avg += unblocked_time - process.blocked_duration;
        printf("Task did not complete time slice. Returning to queue.\n");
        add_to_queue(process, process.CPU_AFFINITY); // re-add it back to queue
    }
    if (process.remain_time > process.time_slice){ // if execution exceeds to time slice
        process.is_blocked = true; // block process
        process.remain_time -= process.time_slice; // update run time
        process.sleep_avg--; // decrement sleep by 1 everytime process runs
        printf("Task completed within time slice.\n");
    }
}

// Method for SCHED_FIFO
void sched_fifo(struct task_struct process){
    printf("FIFO SCHEDULING --------------- PID: %d\n", process.pid);
    usleep(process.remain_time); // sleep for process time
    printf("Task completed: \n");
    print_task(&process); // showcase that task completed
    remove_from_queue(process, process.CPU_AFFINITY); // remove process from given consumer
}


// Method for SCHED_RR
void sched_rr(struct task_struct process){
    printf("RR SCHEDULING--------------- PID: %d\n", process.pid);
    usleep(process.time_slice); // sleep for process time
    //pthread_mutex_lock(&mutex); // lock mutex
    remove_from_queue(process, process.CPU_AFFINITY); // remove from queue
    if (process.remain_time > process.time_slice) { // process is longer than time slice
        process.remain_time -= process.time_slice; // decrement remaining execution time by the time used during time slice
        printf("Task did not complete time slice. Returning to queue.\n");
        add_to_queue(process, process.CPU_AFFINITY); // re-add it back to queue
    } else {
        process.time_slice = 0; // reset the time slice
        printf("Task completed within time slice:\n");
        print_task(&process); // showcase the task completed
    }
    //pthread_mutex_unlock(&mutex); // unlock mutex
}

// Consumer function to process and update process information
void *consumer_function(void *arg) {
    int consumer_id = (intptr_t)arg; // the CPU in question

    struct queue_t *cpu = &ready_queues[consumer_id]; // pointer to the ready queues in the CPU given
    printf("Beginning to look at ready queues in CPU %d\n", consumer_id);
    printf("--------------------------------------------\n");

    printf("--------------In Ready Queue 0--------------\n");
    for (int i = 0; i < READY_QUEUE_SIZE; i++) {
        if (cpu->RQ0[i].pid != 0) { // check if element in queue is occupied or not
            if (cpu->RQ0[i].process_type == 0) { // process type is NORMAL
            	//pthread_mutex_lock(&mutex);
                printf("PID: %d, Processing with Scheduling type: NORMAL\n", cpu->RQ0[i].pid);
                sched_normal(cpu->RQ0[i]); // schedule the process with NORMAL function
                print_task(&cpu->RQ0[i]); // showcase the task
                //pthread_mutex_unlock(&mutex);
            } else if (cpu->RQ0[i].process_type == 1) {
            	//pthread_mutex_lock(&mutex);
                printf("PID: %d, Processing with Scheduling type: RR\n", cpu->RQ0[i].pid);
                sched_rr(cpu->RQ0[i]); // schedule the process with RR function
                print_task(&cpu->RQ0[i]); // showcase the task
                //pthread_mutex_unlock(&mutex);
            } else {
            	//pthread_mutex_lock(&mutex);
                printf("PID: %d, Processing with Scheduling type: FIFO\n", cpu->RQ0[i].pid);
                sched_fifo(cpu->RQ0[i]); // schedule the process with FIFO function
                print_task(&cpu->RQ0[i]); // showcase the task
                //pthread_mutex_unlock(&mutex);
            }
            printf("Scheduling completed. Moving to next element in queue.\n");
            printf("--------------------------------------------\n");
        } // keep looping until every element in queue is scheduled and executed
    }

    printf("--------------In Ready Queue 1--------------\n");
    for (int i = 0; i < READY_QUEUE_SIZE; i++) {
        if (cpu->RQ1[i].pid != 0) {
            if (cpu->RQ1[i].process_type == 0) {
            	//pthread_mutex_lock(&mutex);
                printf("PID: %d, Processing with Scheduling type: NORMAL\n", cpu->RQ1[i].pid);
                sched_normal(cpu->RQ1[i]);  // schedule the process with NORMAL function
                print_task(&cpu->RQ1[i]); // showcase the task
                //pthread_mutex_unlock(&mutex);
            } else if (cpu->RQ1[i].process_type == 1) {
            	//pthread_mutex_lock(&mutex);
                printf("PID: %d, Processing with Scheduling type: RR\n", cpu->RQ1[i].pid);
                sched_rr(cpu->RQ1[i]); // schedule the process with RR function
                print_task(&cpu->RQ1[i]); // showcase the task
                //pthread_mutex_unlock(&mutex);
            } else {
            	//pthread_mutex_lock(&mutex);
                printf("PID: %d, Processing with Scheduling type: FIFO\n", cpu->RQ1[i].pid);
                sched_fifo(cpu->RQ1[i]); // schedule the process with FIFO function
                print_task(&cpu->RQ1[i]); // showcase the task
                //pthread_mutex_unlock(&mutex);
            }
            printf("Scheduling completed. Moving to next element in queue.\n");
            printf("--------------------------------------------\n");
        } // keep looping until every element in queue is scheduled and executed

    }

    printf("--------------In Ready Queue 2--------------\n");
    for (int i = 0; i < READY_QUEUE_SIZE; i++) {
        if (cpu->RQ2[i].pid != 0) {
            if (cpu->RQ2[i].process_type == 0) {
            	//pthread_mutex_lock(&mutex);
                printf("PID: %d, Processing with Scheduling type: NORMAL\n", cpu->RQ2[i].pid);
                sched_normal(cpu->RQ2[i]);  // schedule the process with NORMAL function
                print_task(&cpu->RQ2[i]); // showcase the task
                //pthread_mutex_unlock(&mutex);
            } else if (cpu->RQ2[i].process_type == 1) {
            	//pthread_mutex_lock(&mutex);
                printf("PID: %d, Processing with Scheduling type: RR\n", cpu->RQ2[i].pid);
                sched_rr(cpu->RQ2[i]); // schedule the process with RR function
                print_task(&cpu->RQ2[i]); // showcase the task
                //pthread_mutex_unlock(&mutex);
            } else {
            	//pthread_mutex_lock(&mutex);
                printf("PID: %d, Processing with Scheduling type: FIFO\n", cpu->RQ2[i].pid);
                sched_fifo(cpu->RQ2[i]); // schedule the process with FIFO function
                print_task(&cpu->RQ2[i]); // showcase the task
                //pthread_mutex_unlock(&mutex);
            }
            printf("Scheduling completed. Moving to next element in queue.\n");
            printf("--------------------------------------------\n");
        } // keep looping until every element in queue is scheduled and executed

    }

    printf("All processes scheduled and dealt with.\n"); // processes are doen
    return NULL;
}

// Main function
int main() {
    FILE *file = fopen("parse.txt", "r");  // open the text file in READ to parse
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    struct task_struct process_data; // task to save in
    char store[10]; // store the enum parsed here
    int queue_selector;
    srand(time(NULL)); // create a new seed for time to guarantee a true random number
    // without srand, the same number will be generated by rand() every execution

    while (fscanf(file, "%d,%9[^,],%d,%d,%d", &process_data.pid, store, &process_data.static_priority, &process_data.remain_time, &process_data.CPU_AFFINITY) == 5){
        // Successfully parsed a line, then store into a task
        process_data.process_type = get_process_type(store); // convert the ENUM into String
        if (process_data.CPU_AFFINITY == -1){ // if affinity is -1, generate a random number between 0-3
            queue_selector = rand() % NUM_CONSUMERS; // set random number for CPU affinity
        } else {
            queue_selector = process_data.CPU_AFFINITY; // no need to change
        }
        // Calculate quantum size or time slice
        if (process_data.static_priority < 120) {
            process_data.time_slice = (140 - process_data.static_priority) * 20;
        } else {
            process_data.time_slice = (140 - process_data.static_priority) * 5;
        }
        add_to_queue(process_data, queue_selector); // add to queue
    }
    // Display what processes are sorted in the first consumer
    printf("FIRST CONSUMER--------------------------------------------\n");
    printf("RQ0: \n");
    print_queue(ready_queues[0].RQ0, READY_QUEUE_SIZE);
    printf("RQ1: \n");
    print_queue(ready_queues[0].RQ1, READY_QUEUE_SIZE);
    printf("RQ2: \n");
    print_queue(ready_queues[0].RQ2, READY_QUEUE_SIZE);

    // Display what processes are sorted in the second consumer
    printf("SECOND CONSUMER--------------------------------------------\n");
    printf("RQ0: \n");
    print_queue(ready_queues[1].RQ0, READY_QUEUE_SIZE);
    printf("RQ1: \n");
    print_queue(ready_queues[1].RQ1, READY_QUEUE_SIZE);
    printf("RQ2: \n");
    print_queue(ready_queues[1].RQ2, READY_QUEUE_SIZE);

    // Display what processes are sorted in the third consumer
    printf("THIRD CONSUMER--------------------------------------------\n");
    printf("RQ0: \n");
    print_queue(ready_queues[2].RQ0, READY_QUEUE_SIZE);
    printf("RQ1: \n");
    print_queue(ready_queues[2].RQ1, READY_QUEUE_SIZE);
    printf("RQ2: \n");
    print_queue(ready_queues[2].RQ2, READY_QUEUE_SIZE);

    // Display what processes are sorted in the fourth consumer
    printf("FOURTH CONSUMER--------------------------------------------\n");
    printf("RQ0: \n");
    print_queue(ready_queues[3].RQ0, READY_QUEUE_SIZE);
    printf("RQ1: \n");
    print_queue(ready_queues[3].RQ1, READY_QUEUE_SIZE);
    printf("RQ2: \n");
    print_queue(ready_queues[3].RQ2, READY_QUEUE_SIZE);
    printf("\n\nDone debugging, now moving onto consumer thread creation...\n\n");


    // Create consumer threads
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        if (pthread_create(&consumer_threads[i], NULL, consumer_function, (void *)(intptr_t )i)!= 0) {
            perror("Error creating consumer thread");
            return 1;
        }
    }

    // Join consumer threads
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        if (pthread_join(consumer_threads[i], NULL) != 0) {
            perror("Error joining consumer thread");
            return 1;
        }
    }

    fclose(file); // close the file after everything is finished
    printf("File closed. All consumer ready queues are empty and executed. Goodnight.\n");
    return 0;
}
