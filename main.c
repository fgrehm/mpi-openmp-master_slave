#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mpi.h"

#define MS_WORK_TAG 1
#define MS_DIE_TAG  2

int myrank;

void master();
void slave();

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

  printf("Seeding slaves\n");
  for (rank = 1; rank < ntasks; ++rank) {
    int *current_task = work + (rank - 1);
    MPI_Send(current_task, 1, MPI_INT, rank, MS_WORK_TAG, MPI_COMM_WORLD);
  }

  printf("Sending remaining jobs\n");
  for (i = ntasks-1; i < njobs; i++) {
    MPI_Recv(&result, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    printf("Received %d from %d\n", result, status.MPI_SOURCE);

    int *next_job = work + i;
    printf("Sending %d to %d\n", *next_job, status.MPI_SOURCE);
    MPI_Send(next_job, 1, MPI_INT, status.MPI_SOURCE, MS_WORK_TAG, MPI_COMM_WORLD);
  }

  printf("Done sending jobs, waiting to be completed\n");

  for (rank = 1; rank < ntasks; ++rank) {
    MPI_Recv(&result, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    printf("Received %d from %d\n", result, status.MPI_SOURCE);
  }

  printf("Killing slaves\n");
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
      printf("Exiting from %d\n", myrank);
      return;
    }
    printf("[%d] Processing %d...\n", myrank, work);
    sleep(rand() % 2);
    result = work * 10 * myrank;
    MPI_Send(&result, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }
}
