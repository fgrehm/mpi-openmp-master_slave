default:
	ladcomp -env mpicc main.c -o mpi_master_slave
	@echo; echo; echo 'RUN ladrun -np 10 -net ether mpi_master_slave'; echo; echo
	ladalloc -c atlantica -n 2 -t 3 -s
