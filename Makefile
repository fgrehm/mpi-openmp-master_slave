default:
	ladcomp -env mpicc main.c -o mpi_master_slave
	ladrun -np 10 -net ether mpi_master_slave

hack:
	ladalloc -c atlantica -n 2 -t 3 -s
