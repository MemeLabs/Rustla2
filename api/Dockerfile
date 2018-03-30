FROM alpine:edge AS build-env

# How many `make` jobs to run in parallel.
ARG jobs=1

# Install tools and libraries needed to build dependencies.
RUN apk add --no-cache autoconf automake boost-dev cmake curl-dev \
  g++ jansson-dev libevent-dev make openssl-dev rapidjson-dev; \
  # glog-dev is in the testing repository.
  apk add --no-cache \
    --repository http://dl-cdn.alpinelinux.org/alpine/edge/testing \
    glog-dev

# Build and install dependencies.
WORKDIR /api
COPY third-party ./third-party

# Build double-conversion (dependency of folly).
WORKDIR /api/third-party/double-conversion/build
RUN cmake ..; \
  make -j${jobs}; \
  make install

# Build gflags (dependency of folly).
WORKDIR /api/third-party/gflags/build
RUN cmake ..; \
  make -j${jobs}; \
  make install

# Build folly.
WORKDIR /api/third-party/folly/_build
RUN cmake configure ..; \
  make -j${jobs}; \
  make install

# Build jwt-cpp.
WORKDIR /api/third-party/jwt-cpp/build
RUN cmake ..; \
  make -j${jobs}; \
  make install

# Build gtest.
WORKDIR /api/third-party/googletest/build
RUN cmake ..; \
  make -j${jobs}; \
  make install

# Build wangle.
# WORKDIR /api/third-party/wangle/wangle
# RUN cmake .; \
#   make -j${jobs}; \
#   make install

# Build uWebSockets.
WORKDIR /api/third-party/uWebSockets
RUN make -j${jobs}; \
  make install

# Build libmagic.
WORKDIR /api/third-party/file
RUN autoreconf -f -i; \
  ./configure; \
  make -j${jobs}; \
  make install

# Build application.
WORKDIR /api
COPY cmake ./cmake
COPY src ./src
COPY CMakeLists.txt .
WORKDIR /api/build
RUN cmake ..; \
  make -j${jobs} rustla2_api

# Build runtime image.
FROM scratch
WORKDIR /api
COPY --from=build-env /api/build/rustal2_api .
ENTRYPOINT ["rustla2_api"]
