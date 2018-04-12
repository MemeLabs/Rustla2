FROM ubuntu:16.04
LABEL maintainer="Overrustle"

RUN apt-get update && apt-get install -y \
    automake \
    autoconf \
    autoconf-archive \
    binutils-dev \
    build-essential \
    cmake \
    git-core \
    g++ \
    libboost-context-dev \
    libboost-filesystem-dev \
    libboost-iostreams-dev \
    libboost-thread-dev \
    libboost-program-options-dev \
    libboost-regex-dev \
    libboost-system-dev \
    libboost-chrono-dev \
    libcurl4-gnutls-dev \
    libevent-dev \
    libdouble-conversion-dev \
    libgoogle-glog-dev \
    libgflags-dev \
    libiberty-dev \
    libjemalloc-dev \
    liblz4-dev \
    liblzma-dev \
    libmagic-dev \
    libsnappy-dev \
    libsqlite3-dev \
    libtool \
    make \
    pkg-config \
    python \
    sqlite3 \
    zlib1g-dev && \
    rm -rf /var/lib/apt/lists/*

COPY . ./rustla2_api

RUN cd rustla2_api && \
    ./deps.sh && \
    rm -rf ./rustla2_api

ENTRYPOINT ["bash"]
