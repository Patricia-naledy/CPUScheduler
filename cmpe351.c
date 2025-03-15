#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// Node structure for linked list
struct Process {
    int id;
    int burst_time;
    int arrival_time;
    int priority;
    int completion_time;
    int turn_around_time;
    int waiting_time;
    int remaining_time;
    struct Process *next;
};

// Function prototypes for linked list operations and scheduling
struct Process *create_process(int id, int bt, int at, int pr);
struct Process *insert_process(struct Process *head, struct Process *new_process);
struct Process* insertProcess1(struct Process *head, struct Process *newProcess);
struct Process *find_shortest_process(struct Process *head, int current_time);
struct Process *get_next_process(struct Process *head, int current_time);
struct Process* findShortestJob(struct Process *head, int current_time);
void insert_process_sorted(struct Process **head, struct Process *new_process);
void calculateTATandWT(struct Process* head, float* total_wt, int total_processes);
void calculate_metrics(struct Process *head);
void insert_processs(struct Process** head, struct Process* new_process) ;
void calculate_FCFS(struct Process *head, FILE *outputFile);
void calculate_sjf_non_preemtive(struct Process *head, FILE *outputFile);
void calculate_SJF_preemptive(struct Process *head, FILE *outputFile);
float calculate_average_waiting_time(struct Process *head);
void priorityNonPreemtive(struct Process* head, FILE *outputFile);
void priorityPreemptiveScheduling(struct Process* head, FILE *outputFile);
void round_robin(struct Process *head, int quantum, FILE *outputFile);
//float calculate_average_waitingtime(struct Process *head);



int main() {
    struct Process *head = NULL;
    FILE *inputFile, *outputFile;
    int burst_time, arrival_time, priority, id = 1;

    // Open the input file
    inputFile = fopen("input.txt", "r");
    if (!inputFile) {
        printf("Error: Could not open input.txt\n");
        return 1;
    }

    // Read the process information from the input file
    while (fscanf(inputFile, "%d:%d:%d", &burst_time, &arrival_time, &priority) != EOF) {
        struct Process *new_process = create_process(id++, burst_time, arrival_time, priority);
        head = insert_process(head, new_process);
    }
    fclose(inputFile);

    // Open the output file
    outputFile = fopen("output.txt", "w");
    if (!outputFile) {
        printf("Error: Could not open output.txt\n");
        return 1;
    }
    calculate_FCFS(head, outputFile);  // For FCFS
    calculate_sjf_non_preemtive(head, outputFile);//for SJF non preemetive
    calculate_SJF_preemptive(head , outputFile);// For SJF Preemptive
    priorityNonPreemtive(head, outputFile); //for priotity non Preemtive
    priorityPreemptiveScheduling(head, outputFile);//for priotity Preemtive
    
    round_robin(head, 2, outputFile);//for round robin
     
    fclose(outputFile);


    struct Process *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
    printf("Scheduling complete. Results written to output.txt\n");
    // Free memory
    
    return 0;
}

// Function to create a new process node
struct Process *create_process(int id, int bt, int at, int pr) {
    struct Process *new_process = (struct Process *)malloc(sizeof(struct Process));
    if (!new_process) {
        printf("Error: Memory allocation failed\n");
        exit(1);
    }
    new_process->id = id;
    new_process->burst_time = bt;
    new_process->arrival_time = at;
    new_process->priority = pr;
    new_process->completion_time = 0;
    new_process->turn_around_time = 0;
    new_process->waiting_time = 0;
    new_process->remaining_time = bt;
    new_process->next = NULL;
    return new_process;
}


// Function to insert a process into the linked list
struct Process *insert_process(struct Process *head, struct Process *new_process) {
    if (!head) {
        return new_process;
    }
    struct Process *temp = head;
    while (temp->next) {
        temp = temp->next;
    }
    temp->next = new_process;
    return head;
}

// Function to calculate FCFS scheduling
void calculate_FCFS(struct Process *head, FILE *outputFile) {
    int current_time = 0;
    int total_waiting_time = 0;
    int process_count = 0;

    struct Process *current = head;
    fprintf(outputFile, "%d:" ,
                current->id);
    while (current) {
        if (current_time < current->arrival_time) {
            current_time = current->arrival_time;
        }
        current->completion_time = current_time + current->burst_time;
        current->turn_around_time = current->completion_time - current->arrival_time;
        current->waiting_time = current->turn_around_time - current->burst_time;

        total_waiting_time += current->waiting_time;
        process_count++;

        
        fprintf(outputFile, "%d:" ,
                current->waiting_time);

        current_time = current->completion_time;
        current = current->next;
    }

     fprintf(outputFile, "%.2f\n", (float)total_waiting_time / process_count);
}



// Function to insert a process into the linked list in sorted order based on burst time
void insert_process_sorted(struct Process **head, struct Process *new_process) {
    if (*head == NULL || (*head)->arrival_time > new_process->arrival_time || 
        ((*head)->arrival_time == new_process->arrival_time && (*head)->burst_time > new_process->burst_time)) {
        new_process->next = *head;
        *head = new_process;
    } else {
        struct Process *current = *head;
        while (current->next != NULL && (current->next->arrival_time < new_process->arrival_time || 
               (current->next->arrival_time == new_process->arrival_time && current->next->burst_time <= new_process->burst_time))) {
            current = current->next;
        }
        new_process->next = current->next;
        current->next = new_process;
    }
}

// Function to calculate and perform SJF scheduling
void calculate_sjf_non_preemtive(struct Process *head, FILE *outputFile) {
    int current_time = 0;
    struct Process *temp = head;
    int total_processes = 0;

    // Count the total number of processes
    while (temp != NULL) {
        total_processes++;
        temp = temp->next;
    }

    // Perform SJF scheduling
    temp = head;
    while (temp != NULL) {
        // If the process has arrived before or at the current time, schedule it
        if (temp->arrival_time <= current_time) {
            temp->completion_time = current_time + temp->burst_time;
            temp->turn_around_time = temp->completion_time - temp->arrival_time;
            temp->waiting_time = temp->turn_around_time - temp->burst_time;
            current_time = temp->completion_time; // Update current time
        }
        temp = temp->next;
    }

    // Write the results to the output file inside the SJF function
    temp = head;
    fprintf(outputFile, "2:");
    while (temp != NULL) {
        fprintf(outputFile, "%d:", 
                 temp->waiting_time);
        temp = temp->next;

    }
    float avg_waiting_time = calculate_average_waiting_time(head);
    fprintf(outputFile, "%.2f\n", avg_waiting_time);

}







// Function to find the process with the shortest remaining time
struct Process *find_shortest_process(struct Process *head, int current_time) {
    struct Process *shortest = NULL;
    int min_time = INT_MAX;

    while (head) {
        if (head->arrival_time <= current_time && head->remaining_time > 0 && head->remaining_time < min_time) {
            min_time = head->remaining_time;
            shortest = head;
        }
        head = head->next;
    }

    return shortest;
}


// Function to calculate SJF Preemptive scheduling
void calculate_SJF_preemptive(struct Process *head, FILE *outputFile) {
    int current_time = 0, completed = 0;
    int total_processes = 0;
    struct Process *temp = head;

    // Count total processes
    while (temp) {
        total_processes++;
        temp = temp->next;
    }

    while (completed < total_processes) {
        struct Process *shortest = find_shortest_process(head, current_time);

        if (!shortest) {
            current_time++;
            continue;
        }

        // Process the selected process
        shortest->remaining_time--;

        if (shortest->remaining_time == 0) {
            completed++;
            shortest->completion_time = current_time + 1;
        }

        current_time++;
    }

    calculate_metrics(head);

    // Write metrics to output file
    temp = head;
    float avg_waiting_time = calculate_average_waiting_time(head);
    fprintf(outputFile,"3:");
    while (temp) {
        fprintf(outputFile, "%d:",
                 temp->waiting_time);
        temp = temp->next;
    }
    fprintf(outputFile, "%.2f\n", avg_waiting_time);
}

// Function to calculate TAT and WT
void calculate_metrics(struct Process *head) {
    while (head) {
        head->turn_around_time = head->completion_time - head->arrival_time;
        head->waiting_time = head->turn_around_time - head->burst_time;
        head = head->next;
    }
}

// Function to calculate average waiting time of all processes
float calculate_average_waiting_time(struct Process *head) {
    int total_waiting_time = 0;
    int count = 0;
    struct Process *temp = head;

    while (temp) {
        total_waiting_time += temp->waiting_time;
        count++;
        temp = temp->next;
    }

    return (float)total_waiting_time / count;
}



// Function to insert a process into the linked list in order of priority non preemtive(ascending)
struct Process* insertProcess1(struct Process *head, struct Process *newProcess) {
    if (head == NULL || newProcess->priority < head->priority || (newProcess->priority == head->priority && newProcess->arrival_time < head->arrival_time)) {
        newProcess->next = head;
        return newProcess;
    }

    struct Process *current = head;
    while (current->next != NULL && (current->next->priority < newProcess->priority || (current->next->priority == newProcess->priority && current->next->arrival_time <= newProcess->arrival_time))) {
        current = current->next;
    }
    newProcess->next = current->next;
    current->next = newProcess;
    return head;
}
// Function to calculate Completion Time, Turnaround Time, and Waiting Time
void calculateTimes(struct Process *head) {
    int current_time = 0;
    struct Process *current = head;
    while (current != NULL) {
        if (current_time < current->arrival_time) {
            current_time = current->arrival_time;
        }
        current_time += current->burst_time;
        current->completion_time = current_time;
        current->turn_around_time = current->completion_time - current->arrival_time;
        current->waiting_time = current->turn_around_time - current->burst_time;
        current = current->next;
    }

}



void insert_processs(struct Process** head, struct Process* new_process) {
    if (*head == NULL || (*head)->priority > new_process->priority) {
        new_process->next = *head;
        *head = new_process;
    } else {
        struct Process* current = *head;
        while (current->next != NULL && current->next->priority <= new_process->priority) {
            current = current->next;
        }
        new_process->next = current->next;
        current->next = new_process;
    }
}


// Function to calculate TAT and WT and generate the output
    void priorityNonPreemtive(struct Process* head, FILE* outputFile) {
    int current_time = 0;
    int total_waiting_time = 0;
    int process_count = 0;

    struct Process* temp = head;
    fprintf(outputFile, "4:");
    while (temp != NULL) {
        if (current_time < temp->arrival_time) {
            current_time = temp->arrival_time;
        }

        temp->completion_time = current_time + temp->burst_time;
        temp->turn_around_time = temp->completion_time - temp->arrival_time;
        temp->waiting_time = temp->turn_around_time - temp->burst_time;

        current_time = temp->completion_time;

        // Write to the output file in the specified format
        fprintf(outputFile, "%d:",
                temp->waiting_time);

        total_waiting_time += temp->waiting_time;
        process_count++;

        temp = temp->next;
    }

    // Calculate and write the average waiting time
    float average_waiting_time = (float)total_waiting_time / process_count;
    fprintf(outputFile, "%.2f\n", average_waiting_time);
}









// Function to find the next process to execute based on priority
struct Process *get_next_process(struct Process *head, int current_time) {
    struct Process *highest_priority_process = NULL;
    struct Process *temp = head;
    while (temp != NULL) {
        if (temp->arrival_time <= current_time && temp->burst_time > 0) {
            if (highest_priority_process == NULL || temp->priority < highest_priority_process->priority) {
                highest_priority_process = temp;
            }
        }
        temp = temp->next;
    }
    return highest_priority_process;
}

// Function to calculate scheduling
void priorityPreemptiveScheduling(struct Process *head, FILE *outputFile) {
    int current_time = 0;
    int total_waiting_time = 0;
    int total_processes = 0;
fprintf(outputFile, "5:");
    while (1) {
        struct Process *current_process = get_next_process(head, current_time);
        if (current_process == NULL) {
            int all_done = 1;
           struct Process *temp = head;
            while (temp != NULL) {
                if (temp->burst_time > 0) {
                    all_done = 0;
                    break;
                }
                temp = temp->next;
            }
            if (all_done) {
                break;
            } else {
                current_time++;
                continue;
            }
        }

        // Execute the process for one unit of time
        current_process->burst_time--;
        current_time++;

        // If the process completes
        if (current_process->burst_time == 0) {
            current_process->completion_time = current_time;
            current_process->turn_around_time = current_process->completion_time - current_process->arrival_time;
            current_process->waiting_time = current_process->turn_around_time - current_process->burst_time;
            total_waiting_time += current_process->waiting_time;
            total_processes++;

            // Write the process details to the output file
            fprintf(outputFile, "%d:",
                    current_process->waiting_time);
        }
    }

    // Calculate and write the average waiting time
    float average_waiting_time = (float)total_waiting_time / total_processes;
    fprintf(outputFile, "%.2f\n", average_waiting_time);
}


// Function to calculate Turnaround Time (TAT) and Waiting Time (WT)
void calculateTATandWT(struct Process* head, float* total_wt, int total_processes) {
    struct Process* temp = head;
    *total_wt = 0;
    while (temp != NULL) {
        temp->completion_time = temp->arrival_time + temp->burst_time;  // Initial completion time
        int tat = temp->completion_time - temp->arrival_time;
        int wt = tat - temp->burst_time;
        *total_wt += wt;
        temp = temp->next;
    }
}
// Function to perform Round Robin scheduling with a given quantum
void round_robin(struct Process* head, int quantum, FILE* outputFile) {
    struct Process* temp = head;
    int current_time = 0;
    float total_wt = 0;
    int total_processes = 0;

    // Calculate total number of processes
    temp = head;
    while (temp != NULL) {
        total_processes++;
        temp = temp->next;
    }

    // Run Round Robin
    while (1) {
        int all_done = 1;
        temp = head;
        while (temp != NULL) {
            if (temp->remaining_time > 0) {
                all_done = 0;
                if (temp->remaining_time <= quantum) {
                    current_time += temp->remaining_time;
                    temp->remaining_time = 0;
                } else {
                    current_time += quantum;
                    temp->remaining_time -= quantum;
                }
            }
            temp = temp->next;
        }

        if (all_done) break;  // If all processes are done, break the loop
    }

    // Calculate TAT and WT
    calculateTATandWT(head, &total_wt, total_processes);

    // Write to output file
    temp = head;
    fprintf(outputFile, "6:");
    while (temp != NULL) {
        fprintf(outputFile, "%d:",
           temp->waiting_time);
        temp = temp->next;
    }

    // Calculate and display average waiting time
    float average_wt = total_wt / total_processes;
    fprintf(outputFile, "%.2f\n", average_wt);
}
