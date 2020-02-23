FROM ubuntu:16.04 AS build-env

RUN apt-get update && apt-get install -y \
    automake \
    autoconf \
    autoconf-archive \
    binutils-dev \
    build-essential \
    cmake \
    git-core \
    g++ \
    libboost-all-dev \
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
    libsodium-dev \
    libssl-dev \
    libsqlite3-dev \
    libtool \
    make \
    pkg-config \
    python \
    sqlite3 \
    zlib1g-dev && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /api
COPY third-party ./third-party
COPY deps.sh .
RUN bash deps.sh

COPY cmake ./cmake
COPY src ./src
COPY tests ./tests
COPY CMakeLists.txt .
WORKDIR /api/build
RUN cmake .. \
  && make rustla2_api

FROM ubuntu:16.04
RUN useradd -m rustla
USER rustla
WORKDIR /api
COPY --from=build-env /api/build/rustla2_api .

ENTRYPOINT ["./rustla2_api"]
