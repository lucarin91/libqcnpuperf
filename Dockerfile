# syntax=docker/dockerfile:1
FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update -y && apt-get install -y software-properties-common \
    && add-apt-repository -y ppa:ubuntu-qcom-iot/qcom-ppa \
    && apt-get update -y && apt-get install -y \
    build-essential cmake git \
    autoconf automake libtool m4 pkg-config \
    libyaml-dev libbsd-dev libsystemd-dev \
    libncurses-dev \
    qcom-fastrpc-dev qcom-dspservices-headers-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .
RUN cmake -B build -DCMAKE_BUILD_TYPE=Release -DQCNPU_PERF_BUILD_CLI=ON \
    && cmake --build build --target qcnpuperf_cli -j$(nproc)

# Export stage – extracts only the binary to the host
FROM scratch AS export
COPY --from=builder /src/build/qcnpuperf_cli /qcnpuperf_cli
