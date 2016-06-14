CC ?= ladcomp -env mpicc

all: mpi_master_slave

mpi_master_slave: mpi_master_slave.c
	$(CC) mpi_master_slave.c -fopenmp -o mpi_master_slave

hack:
	docker build -t mpi-openmp-master-slave . && docker run -ti -e CC=mpicc --rm -v `pwd`:/code mpi-openmp-master-slave

clean:
	rm -rf *.o mpi_master_slave

ladhack:
	ladalloc -c atlantica -n 4 -t 3 -s
	# ladrun -np 10 -net ether mpi_master_slave
