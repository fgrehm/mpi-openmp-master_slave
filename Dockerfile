FROM ubuntu:14.04
RUN apt-get update \
    && apt-get install -y build-essential curl \
    && rm -rf /var/lib/apt/lists/*
RUN cd /tmp \
    && curl -L https://www.open-mpi.org/software/ompi/v1.10/downloads/openmpi-1.10.2.tar.gz | tar xz \
    && cd openmpi-1.10.2 \
    && ./configure --prefix=/usr/local \
    && make all install \
    && rm -rf openmpi-1.10.2
RUN groupadd -r developer && useradd -r -g developer developer
USER developer
ENV LD_LIBRARY_PATH=/usr/local/lib
WORKDIR /code
