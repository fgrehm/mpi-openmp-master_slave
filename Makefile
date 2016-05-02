CC ?= ladcomp -env mpicc

all: single_process mpi_master_slave

mpi_master_slave: mpi_master_slave.c
	$(CC) mpi_master_slave.c -o mpi_master_slave

hack:
	docker build -t mpi-divide-and-conquer . && docker run -ti -e CC=mpicc --rm -v `pwd`:/code mpi-divide-and-conquer

clean:
	rm -rf *.o single_process mpi_master_slave

ladhack:
	ladalloc -c atlantica -n 2 -t 3 -s
	# ladrun -np 10 -net ether mpi_master_slave
