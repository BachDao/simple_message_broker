# Remote docker enviroment for development purpose
#
# Build and run:
#   docker build -t ubuntu-simple-broker:1.0 -f dev.Dockerfile .
#   docker run -d --cap-add sys_ptrace --privileged -p127.0.0.1:2222:22 ubuntu-simple-broker:1.0
#   ssh-keygen -f "$HOME/.ssh/known_hosts" -R "[127.0.0.1]:2222"
#
# stop:
#   docker stop ubuntu-simple-broker
#
# ssh credentials (test user):
#   root@root

FROM ubuntu:latest

RUN apt -y update && apt -y upgrade

RUN apt install -y gcc g++ cmake zip unzip tar git ninja-build curl pkg-config

# SSH stuff
RUN apt -y install openssh-server
ENTRYPOINT service ssh restart && bash

RUN echo "root:root" | chpasswd

RUN ( \
    echo 'PermitRootLogin yes'; \
    echo 'PasswordAuthentication yes'; \
    ) > /etc/ssh/sshd_config

RUN /usr/bin/ssh-keygen -A



#clone code and build
ARG CACHEBUST=1
RUN git clone https://github.com/BachDao/simple_message_broker
ARG CACHEBUST=0
RUN cd simple_message_broker


#CMake config & build

# For arm, s390x, ppc64le and riscv platforms
ENV VCPKG_FORCE_SYSTEM_BINARIES=1

RUN /usr/bin/cmake -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_COMPILER=/usr/bin/g++ \
    -DCMAKE_C_COMPILER=/usr/bin/gcc \
    -DCMAKE_MAKE_PROGRAM=/usr/bin/ninja \
    -G Ninja \
    -S /simple_message_broker \
    -B /simple_message_broker/cmake-build-debug

# Build examples
RUN /usr/bin/cmake --build /simple_message_broker/cmake-build-debug \
    --target client_1 -j 8

RUN /usr/bin/cmake --build /simple_message_broker/cmake-build-debug \
    --target client_2 -j 8

RUN /usr/bin/cmake --build /simple_message_broker/cmake-build-debug \
    --target server -j 8

# Run examples: start server, client_1 is producer, client_2 is consumer.
WORKDIR /simple_message_broker/cmake-build-debug/examples

RUN echo '#!/bin/bash' >> test_script.sh \
    && echo './server & ' >> test_script.sh \
    && echo 'sleep 3' >> test_script.sh \
    && echo './client_1 &' >> test_script.sh \
    && echo './client_2' >> test_script.sh \
    && chmod +x test_script.sh

# Run test
#CMD ["./test_script.sh"]

EXPOSE 22