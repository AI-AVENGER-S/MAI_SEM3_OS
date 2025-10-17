FROM gcc:14

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    cmake git build-essential gdb libgtest-dev tree strace \
    valgrind clang clangd python3 python3-pip \
    && rm -rf /var/lib/apt/lists/*

RUN cd /usr/src/gtest && \
    cmake CMakeLists.txt && \
    make && \
    cp lib/*.a /usr/lib

WORKDIR /workspace

CMD ["bash"]