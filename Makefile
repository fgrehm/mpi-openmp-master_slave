all: single_process mpi_master_slave

mpi_master_slave: mpi_master_slave.c
	ladcomp -env mpicc mpi_master_slave.c -o mpi_master_slave

hack:
	ladalloc -c atlantica -n 2 -t 3 -s
	# ladrun -np 10 -net ether mpi_master_slave
