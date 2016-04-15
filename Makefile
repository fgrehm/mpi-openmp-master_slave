default:
	@echo 'No default task'

build: bin/mpi_master_slave

bin/mpi_master_slave: mpi_master_slave.c
	mkdir -p bin
	ladcomp -env mpicc mpi_master_slave.c -o bin/mpi_master_slave

bin/single_process: single_process.c
	mkdir -p bin
	gcc single_process.c -o bin/single_process

hack:
	ladalloc -c atlantica -n 2 -t 3 -s
