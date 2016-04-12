default:
	ladcomp -env mpicc main.c -o mpi_master_slave
	@echo; echo; echo 'RUN ladalloc -c atlantica -n 2 -t 3 -s'; echo; echo
	ladalloc -c atlantica -n 2 -t 3 -s
