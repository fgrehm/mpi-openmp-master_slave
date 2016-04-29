#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mpi.h"

#define MASTER 0
#define TAG_JOB_INDEX      1
#define TAG_JOB_PAYLOAD    2
#define TAG_RESULT_INDEX   3
#define TAG_RESULT_PAYLOAD 4
#define TAG_DIE            5

#define TOTAL_ARRAYS  10
#define TOTAL_NUMBERS 10
#define MAX_NUMBER    TOTAL_ARRAYS * TOTAL_NUMBERS

#define T_NUMBER int

int myrank;

void master();
void slave();
void master_send_job(T_NUMBER **numbers, int job_index, int dest);
int master_receive_result();
void slave_receive_job();
void slave_send_result();

void debug_all_numbers(T_NUMBER **numbers);
void debug_numbers(T_NUMBER *numbers);
void my_log(char *fmt, ...);

int cmpfunc(const void * a, const void * b);

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

  if (myrank == MASTER) {
    master();
  } else {
    slave();
  }
  my_log("FINALIZING");
  MPI_Finalize();
  my_log("FINALIZed");
  return 0;
}

void master() {
  int ntasks, rank, i, n;

  T_NUMBER** numbers = calloc(TOTAL_ARRAYS, sizeof(T_NUMBER *));
  if (numbers == NULL) { fprintf(stderr, "calloc failed\n"); return; }

  printf("Preparing arrays...\n");
  for (i = 0; i < TOTAL_ARRAYS; i++) {
    numbers[i] = calloc(TOTAL_NUMBERS, sizeof(T_NUMBER));
    if (numbers[i] == NULL) { fprintf(stderr, "calloc failed\n"); return; }

    for (n = 0; n < TOTAL_NUMBERS; n++)
      numbers[i][n] = MAX_NUMBER - i * TOTAL_NUMBERS - n;
  }
  printf("DONE\n");

  debug_all_numbers(numbers);

  MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

  my_log("Seeding slaves");
  for (rank = 1; rank < ntasks; ++rank) {
    master_send_job(numbers, rank - 1, rank);
  }

  my_log("Sending remaining jobs");
  for (i = ntasks-1; i < TOTAL_ARRAYS; i++) {
    int source = master_receive_result(numbers);
    master_send_job(numbers, i, source);
  }

  my_log("Done sending jobs, waiting to be completed");

  for (rank = 1; rank < ntasks; ++rank)
    master_receive_result(numbers);

  my_log("Killing slaves");
  for (rank = 1; rank < ntasks; ++rank) {
    MPI_Send(&rank, 1, MPI_INT, rank, TAG_DIE, MPI_COMM_WORLD);
  }
  my_log("DONE");

  debug_all_numbers(numbers);
}

void master_send_job(T_NUMBER **numbers, int job_index, int dest) {
  my_log("Sending job %d to %d", job_index, dest);
  MPI_Send(&job_index, 1, MPI_INT, dest, TAG_JOB_INDEX, MPI_COMM_WORLD);

  T_NUMBER *payload = numbers[job_index];
  MPI_Send(payload, TOTAL_NUMBERS, MPI_INT, dest, TAG_JOB_PAYLOAD, MPI_COMM_WORLD);
}

int master_receive_result(T_NUMBER **numbers) {
  MPI_Status status;
  int job_index, source;

  my_log("RECV");
  MPI_Recv(&job_index, 1, MPI_INT, MPI_ANY_SOURCE, TAG_RESULT_INDEX, MPI_COMM_WORLD, &status);
  source = status.MPI_SOURCE;

  T_NUMBER *result = numbers[job_index];
  MPI_Recv(result, TOTAL_NUMBERS, MPI_INT, source, TAG_RESULT_PAYLOAD, MPI_COMM_WORLD, &status);
  my_log("Received from %d the payload of %d", source, job_index);

  return source;
}

void slave() {
  MPI_Status status;
  int job_index;
  T_NUMBER *payload = calloc(TOTAL_NUMBERS, sizeof(T_NUMBER));

  srand(getpid());

  for (;;) {
    MPI_Recv(&job_index, 1, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    if (status.MPI_TAG == TAG_DIE) {
      my_log("Exiting");
      break;
    }
    // Tag should be TAG_JOB_INDEX at this point
    MPI_Recv(payload, TOTAL_NUMBERS*2, MPI_INT, MASTER, TAG_JOB_PAYLOAD, MPI_COMM_WORLD, &status);

    // my_log("BEFORE");
    // debug_numbers(payload);
    qsort(payload, TOTAL_NUMBERS, sizeof(T_NUMBER), cmpfunc);
    // my_log("AFTER");
    // debug_numbers(payload);

    my_log("Begin sending back result for job %d...", job_index);
    MPI_Send(&job_index, 1, MPI_INT, MASTER, TAG_RESULT_INDEX, MPI_COMM_WORLD);
    MPI_Send(payload, TOTAL_NUMBERS, MPI_INT, MASTER, TAG_RESULT_PAYLOAD, MPI_COMM_WORLD);
    my_log("DONE");
  }
}

void debug_all_numbers(T_NUMBER **numbers) {
  int i;
  printf("First 5 arrays:\n");
  for (i = 0; i < 5; i++) {
    debug_numbers(numbers[i]);
  }
  printf(" ...\n");
  for (i = TOTAL_ARRAYS - 5; i < TOTAL_ARRAYS; i++) {
    debug_numbers(numbers[i]);
  }
}

void debug_numbers(T_NUMBER* numbers) {
  int n;
  printf("[%d] [ ", myrank);
  for (n = 0; n < 3; n++) {
    printf("%07d ", numbers[n]);
  }
  printf(" ... ");
  for (n = TOTAL_NUMBERS - 3; n < TOTAL_NUMBERS; n++) {
    printf("%07d ", numbers[n]);
  }
  printf("]\n");
}

void my_log(char *fmt, ...) {
  va_list printfargs;
  printf("[%d] ", myrank);

  va_start(printfargs, fmt);
  vprintf(fmt, printfargs);
  va_end(printfargs);

  printf("\n");
}

int cmpfunc (const void * a, const void * b) {
  return ( *(T_NUMBER*)a - *(T_NUMBER*)b );
}
