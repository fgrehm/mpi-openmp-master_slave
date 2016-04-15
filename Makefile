default:
	@echo 'No default task'

build: bin/mpi_master_slave

bin/mpi_master_slave: main.c
	ladcomp -env mpicc main.c -o mpi_master_slave

bin/single_process: single_process.c
	gcc single_process.c -o single_process

hack:
	ladalloc -c atlantica -n 2 -t 3 -s
