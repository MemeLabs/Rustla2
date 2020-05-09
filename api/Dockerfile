FROM ubuntu:16.04 AS thirdParty

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

FROM thirdParty as rustla2Build

COPY cmake ./cmake
COPY src ./src
COPY tests ./tests
COPY CMakeLists.txt .

WORKDIR /api/build
RUN cmake .. \
  && make rustla2_api

FROM ubuntu:16.04

WORKDIR /lib/x86_64-linux-gnu/
COPY --from=rustla2Build   /lib/x86_64-linux-gnu/libc.so.6 .
COPY --from=rustla2Build   /lib/x86_64-linux-gnu/libcom_err.so.2 .
COPY --from=rustla2Build   /lib/x86_64-linux-gnu/libcrypt.so.1 .
COPY --from=rustla2Build   /lib/x86_64-linux-gnu/libcrypto.so.1.0.0 .
COPY --from=rustla2Build   /lib/x86_64-linux-gnu/libdl.so.2 .
COPY --from=rustla2Build   /lib/x86_64-linux-gnu/libgcc_s.so.1 .
COPY --from=rustla2Build   /lib/x86_64-linux-gnu/libkeyutils.so.1 .
COPY --from=rustla2Build   /lib/x86_64-linux-gnu/liblzma.so.5 .
COPY --from=rustla2Build   /lib/x86_64-linux-gnu/libm.so.6 .
COPY --from=rustla2Build   /lib/x86_64-linux-gnu/libpthread.so.0 .
COPY --from=rustla2Build   /lib/x86_64-linux-gnu/libresolv.so.2 .
COPY --from=rustla2Build   /lib/x86_64-linux-gnu/libssl.so.1.0.0 .
COPY --from=rustla2Build   /lib/x86_64-linux-gnu/libz.so.1 .

WORKDIR /usr/lib/
COPY --from=rustla2Build   /usr/lib/libuWS.so .

WORKDIR /usr/lib/x86_64-linux-gnu/
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libasn1.so.8 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libcurl-gnutls.so.4 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libdouble-conversion.so.1 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libffi.so.6 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libgflags.so.2 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libglog.so.0 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libgmp.so.10 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libgnutls.so.30 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libgssapi_krb5.so.2 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libgssapi.so.3 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libhcrypto.so.4 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libheimbase.so.1 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libheimntlm.so.0 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libhogweed.so.4 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libhx509.so.5 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libidn.so.11 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libk5crypto.so.3 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libkrb5.so.26 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libkrb5.so.3 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libkrb5support.so.0 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/liblber-2.4.so.2 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libldap_r-2.4.so.2 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libnettle.so.6 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libp11-kit.so.0 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libroken.so.18 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/librtmp.so.1 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libsasl2.so.2 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libsqlite3.so.0 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libstdc++.so.6 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libtasn1.so.6 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libunwind.so.8 .
COPY --from=rustla2Build   /usr/lib/x86_64-linux-gnu/libwind.so.0 .

WORKDIR /api
COPY --from=rustla2Build /api/build/rustla2_api .

RUN apt-get update && apt-get install -y \
    libmagic-dev

RUN useradd -m rustla
USER rustla

ENV extraArgs ""
ENTRYPOINT ["/bin/bash", "-c", "/api/rustla2_api ${extraArgs} && sleep infinity"]
