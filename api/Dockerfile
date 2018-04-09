FROM rustla2-api-base:latest
LABEL maintainer="Overrustle"

RUN useradd -m rustla

COPY . ./rustla2_api

RUN rm -rf rustla2_api/build && \
    mkdir rustla2_api/build && \
    cd rustla2_api/build && \
    cmake .. && \
    make rustla2_api && \
    cp rustla2_api /usr/local/bin/rustla2_api

USER rustla

ENTRYPOINT ["rustla2_api"]
