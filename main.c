#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mpi.h"

#define MS_WORK_TAG 1
#define MS_DIE_TAG  2

int myrank;

void master();
void slave();
void my_log(char *fmt, ...);

void main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

  if (myrank == 0) {
    master();
  } else {
    slave();
  }
  MPI_Finalize();
}

void master() {
  int        ntasks, rank, njobs, i;
  int*       work;
  int        result;
  MPI_Status status;

  njobs = 100;
  work = (int*)calloc(njobs, sizeof(int));
  for (i = 0; i < njobs; i++) {
    work[i] = i;
  }

  MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

  my_log("Seeding slaves");
  for (rank = 1; rank < ntasks; ++rank) {
    int *current_task = work + (rank - 1);
    MPI_Send(current_task, 1, MPI_INT, rank, MS_WORK_TAG, MPI_COMM_WORLD);
  }

  my_log("Sending remaining jobs");
  for (i = ntasks-1; i < njobs; i++) {
    MPI_Recv(&result, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    my_log("Received %d from %d", result, status.MPI_SOURCE);

    int *next_job = work + i;
    my_log("Sending %d to %d", *next_job, status.MPI_SOURCE);
    MPI_Send(next_job, 1, MPI_INT, status.MPI_SOURCE, MS_WORK_TAG, MPI_COMM_WORLD);
  }

  my_log("Done sending jobs, waiting to be completed");

  for (rank = 1; rank < ntasks; ++rank) {
    MPI_Recv(&result, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    my_log("Received %d from %d", result, status.MPI_SOURCE);
  }

  my_log("Killing slaves");
  for (rank = 1; rank < ntasks; ++rank) {
    MPI_Send(0, 0, MPI_INT, rank, MS_DIE_TAG, MPI_COMM_WORLD);
  }
}

void slave() {
  int        work, result;
  MPI_Status status;

  srand(getpid());

  for (;;) {
    MPI_Recv(&work, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    if (status.MPI_TAG == MS_DIE_TAG) {
      my_log("Exiting");
      return;
    }
    my_log("Processing %d...", work);
    sleep(rand() % 2);
    result = work * 10 * myrank;
    MPI_Send(&result, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
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
