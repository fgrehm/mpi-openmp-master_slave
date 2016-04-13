#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mpi.h"

#define MASTER 0
#define TAG_JOB_INDEX     1
#define TAG_JOB_PAYLOAD    2
#define TAG_RESULT_INDEX   3
#define TAG_RESULT_PAYLOAD 4
#define TAG_DIE            5

int myrank;

void master();
void slave();
void master_send_job(int dest, int arr_index);
int master_receive_result();
void slave_receive_job();
void slave_send_result();
void my_log(char *fmt, ...);

void main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

  if (myrank == MASTER) {
    master();
  } else {
    slave();
  }
  MPI_Finalize();
}

void master() {
  int        ntasks, rank, njobs, i;
  int*       work;

  njobs = 100;
  work = (int*)calloc(njobs, sizeof(int));
  for (i = 0; i < njobs; i++) {
    work[i] = i;
  }

  MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

  my_log("Seeding slaves");
  for (rank = 1; rank < ntasks; ++rank) {
    master_send_job(rank, rank - 1);
  }

  my_log("Sending remaining jobs");
  for (i = ntasks-1; i < njobs; i++) {
    int source = master_receive_result();
    master_send_job(source, i);
  }

  my_log("Done sending jobs, waiting to be completed");

  for (rank = 1; rank < ntasks; ++rank)
    master_receive_result();

  my_log("Killing slaves");
  for (rank = 1; rank < ntasks; ++rank) {
    MPI_Send(0, 0, MPI_INT, rank, TAG_DIE, MPI_COMM_WORLD);
  }
}

void master_send_job(int dest, int arr_index) {
  int payload = arr_index * 10;

  my_log("Sending job %d to %d", arr_index, dest);
  MPI_Send(&arr_index, 1, MPI_INT, dest, TAG_JOB_INDEX, MPI_COMM_WORLD);
  my_log("Sending %d to %d", payload, dest);
  MPI_Send(&payload, 1, MPI_INT, dest, TAG_JOB_PAYLOAD, MPI_COMM_WORLD);
  // TODO: CHECK FOR ERRORS!
}

int master_receive_result() {
  MPI_Status status;
  int index, result, source;

  MPI_Recv(&index, 1, MPI_INT, MPI_ANY_SOURCE, TAG_RESULT_INDEX, MPI_COMM_WORLD, &status);
  source = status.MPI_SOURCE;

  MPI_Recv(&result, 1, MPI_INT, source, TAG_RESULT_PAYLOAD, MPI_COMM_WORLD, &status);
  my_log("Received %d from %d related to job %d", result, source, index);

  return source;
}

void slave() {
  MPI_Status status;
  int work, result, index;

  srand(getpid());

  for (;;) {
    MPI_Recv(&index, 1, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    if (status.MPI_TAG == TAG_DIE) {
      my_log("Exiting");
      return;
    }
    // Tag should be TAG_JOB_INDEX at this point
    MPI_Recv(&work, 1, MPI_INT, MASTER, TAG_JOB_PAYLOAD, MPI_COMM_WORLD, &status);
    my_log("Processing %d...", work);
    sleep(rand() % 2);
    result = work * 10 * myrank;
    my_log("Begin sending back result for %d...", index);
    MPI_Send(&index, 1, MPI_INT, MASTER, TAG_RESULT_INDEX, MPI_COMM_WORLD);
    my_log("Sending %d to master...", result, index);
    MPI_Send(&result, 1, MPI_INT, MASTER, TAG_RESULT_PAYLOAD, MPI_COMM_WORLD);
    my_log("DONE");
  }
}


void my_log(char *fmt, ...) {
  va_list printfargs;
  printf("[%d] ", myrank);

  va_start(printfargs, fmt);
  vprintf(fmt, printfargs);
  va_end(printfargs);

  printf("\n");
}
