#   --- BUILD stage ---
FROM ubuntu:latest AS build

ARG DEBIAN_FRONTEND=noninteractive
RUN apt update -y && \
    apt install -y \
        software-properties-common \
        build-essential \
        zlib1g-dev\
        libexpat-dev\
        libssl-dev\
        ninja-build \
        cmake \
        python3-venv \
        python3-dev \
        python3 

WORKDIR /arte
ENV PYTHONDONTWRITEBYTECODE=1
ENV PYTHONUNBUFFERED=1
ENV PIP_DISABLE_PIP_VERSION_CHECK=1
ENV VIRTUAL_ENV=/opt/venv
RUN python3 -m venv $VIRTUAL_ENV
ENV PATH="$VIRTUAL_ENV/bin:$PATH"

# install python dependencies
COPY requirements.txt .
RUN pip install -r requirements.txt

# copy app source code for building
COPY src/ ./src/
COPY include/ ./include/

COPY CMakeLists.txt .

# run build commands
RUN mkdir -p build && mkdir -p build/bin
RUN cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-fPIC -no-pie" -GNinja -S . -B ./build/ && \
    cd build && ninja -j8 && cd ..

#   --- FINAL stage ---
FROM ubuntu:latest
EXPOSE 55555/tcp

RUN apt update -y && \
    apt install -y \
        python3-pip\
        python3-venv \
        python3 \
        && rm -rf /var/lib/apt/lists/*

ENV PYTHONDONTWRITEBYTECODE=1
ENV PYTHONUNBUFFERED=1
ENV PIP_DISABLE_PIP_VERSION_CHECK=1
ENV VIRTUAL_ENV=/opt/venv
ENV PATH="$VIRTUAL_ENV/bin:$PATH"

COPY --from=build /opt/venv /opt/venv

RUN adduser --system --group arte
USER arte

# copy over builded app
COPY --chown=arte:arte --from=build ./arte/build/arte ./app/arte
# copy over scripts folder
COPY --chown=arte:arte scripts/ ./scripts/
# copy over type definitions
COPY --chown=arte:arte typedef.yaml .

ENTRYPOINT [ "./app/arte" ]