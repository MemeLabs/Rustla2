#! /bin/bash

# Set the `JOBS` environment variable to configure how many `make` jobs can be
# executed simultaneously. If `JOBS` is not set, then the default is the number
# of cores on the system.
nproc=$(getconf _NPROCESSORS_ONLN)
JOBS=${JOBS:-$nproc}

BASE_DIR="$(/bin/pwd)/third-party"

set -e

apt-get install -y \
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
    zlib1g-dev

echo "installing openssl"

cd "$BASE_DIR/openssl"
./config
make -j $JOBS
make install

echo "installing gtest"

rm -rf "$BASE_DIR/googletest/build"
mkdir "$BASE_DIR/googletest/build"
cd "$BASE_DIR/googletest/build"
cmake ..
make -j $JOBS
make install

echo "installing folly"

cd "$BASE_DIR/folly/folly"
autoreconf --install
./configure --with-openssl="$BASE_DIR/openssl"
make -j $JOBS
make install

echo "installing wangle"

cd "$BASE_DIR/wangle/wangle"
cmake .
make -j $JOBS
make install

echo "installing jansson"

rm -rf "$BASE_DIR/jansson/build"
mkdir "$BASE_DIR/jansson/build"
cd "$BASE_DIR/jansson/build"
cmake ..
make -j $JOBS
make install

echo "installing jwt-cpp"

rm -rf "$BASE_DIR/jwt-cpp/build"
mkdir "$BASE_DIR/jwt-cpp/build"
cd "$BASE_DIR/jwt-cpp/build"
cmake ..
make -j $JOBS
make install

echo "installing rapidjson"

rm -rf "$BASE_DIR/rapidjson/build"
mkdir "$BASE_DIR/rapidjson/build"
cd "$BASE_DIR/rapidjson/build"
cmake ..
make -j $JOBS
make install

echo "installing sqlite_modern_cpp"

cd "$BASE_DIR/sqlite_modern_cpp"
./configure
make -j $JOBS
make install

echo "installing uWebSocket"

cd "$BASE_DIR/uWebSockets"
make
make install
