ARG APP_ENV=partial

FROM ghcr.io/memelabs/rustla2/rustla2-api:thirdPartyBase as partial-build

FROM ubuntu:16.04 as full-build

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

FROM ${APP_ENV}-build as base

COPY cmake ./cmake
COPY src ./src
COPY tests ./tests
COPY CMakeLists.txt .

WORKDIR /api/build
RUN cmake .. \
  && make rustla2_api

FROM ubuntu:16.04

RUN useradd -m rustla \
  && apt-get update \
  && apt-get install -y libmagic-dev curl \
  && rm -rf /var/lib/apt/lists/*

COPY --from=base /api/build/rustla2_api /api/

COPY --from=base \
  /lib/x86_64-linux-gnu/libcrypto.so.1.0.0 \
  /lib/x86_64-linux-gnu/libgcc_s.so.1 \
  /lib/x86_64-linux-gnu/libkeyutils.so.1 \
  /lib/x86_64-linux-gnu/libssl.so.1.0.0 \
  /lib/x86_64-linux-gnu/

COPY --from=base /usr/lib/libuWS.so /usr/lib/

COPY --from=base \
  /usr/lib/x86_64-linux-gnu/libasn1.so.8 \
  /usr/lib/x86_64-linux-gnu/libcurl-gnutls.so.4 \
  /usr/lib/x86_64-linux-gnu/libdouble-conversion.so.1 \
  /usr/lib/x86_64-linux-gnu/libffi.so.6 \
  /usr/lib/x86_64-linux-gnu/libgflags.so.2 \
  /usr/lib/x86_64-linux-gnu/libglog.so.0 \
  /usr/lib/x86_64-linux-gnu/libgmp.so.10 \
  /usr/lib/x86_64-linux-gnu/libgnutls.so.30 \
  /usr/lib/x86_64-linux-gnu/libgssapi_krb5.so.2 \
  /usr/lib/x86_64-linux-gnu/libgssapi.so.3 \
  /usr/lib/x86_64-linux-gnu/libhcrypto.so.4 \
  /usr/lib/x86_64-linux-gnu/libheimbase.so.1 \
  /usr/lib/x86_64-linux-gnu/libheimntlm.so.0 \
  /usr/lib/x86_64-linux-gnu/libhogweed.so.4 \
  /usr/lib/x86_64-linux-gnu/libhx509.so.5 \
  /usr/lib/x86_64-linux-gnu/libidn.so.11 \
  /usr/lib/x86_64-linux-gnu/libk5crypto.so.3 \
  /usr/lib/x86_64-linux-gnu/libkrb5.so.26 \
  /usr/lib/x86_64-linux-gnu/libkrb5.so.3 \
  /usr/lib/x86_64-linux-gnu/libkrb5support.so.0 \
  /usr/lib/x86_64-linux-gnu/liblber-2.4.so.2 \
  /usr/lib/x86_64-linux-gnu/libldap_r-2.4.so.2 \
  /usr/lib/x86_64-linux-gnu/libnettle.so.6 \
  /usr/lib/x86_64-linux-gnu/libp11-kit.so.0 \
  /usr/lib/x86_64-linux-gnu/libroken.so.18 \
  /usr/lib/x86_64-linux-gnu/librtmp.so.1 \
  /usr/lib/x86_64-linux-gnu/libsasl2.so.2 \
  /usr/lib/x86_64-linux-gnu/libsqlite3.so.0 \
  /usr/lib/x86_64-linux-gnu/libtasn1.so.6 \
  /usr/lib/x86_64-linux-gnu/libunwind.so.8 \
  /usr/lib/x86_64-linux-gnu/libwind.so.0 \
  /usr/lib/x86_64-linux-gnu/

WORKDIR /api
USER rustla

ENV extraArgs ""
ENTRYPOINT ["/bin/bash", "-c", "/api/rustla2_api ${extraArgs} && sleep infinity"]
