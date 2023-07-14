# Remote docker enviroment for development purpose
#
# Build and run:
#   docker build -t alpile-cpp:1.0 -f dev.Dockerfile .
#   docker run -d --cap-add sys_ptrace --privileged -p127.0.0.1:2222:22 alpile-cpp:1.0
#   ssh-keygen -f "$HOME/.ssh/known_hosts" -R "[127.0.0.1]:2222"
#
# stop:
#   docker stop alpile-cpp
#
# ssh credentials (test user):
#   root@root

FROM alpine:latest

RUN apk add --no-cache cmake --repository=http://dl-cdn.alpinelinux.org/alpine/edge/main

RUN apk add --update openssh\
    gcc\
    g++\
    clang\
    gdb\
    cmake\
    ninja\
    autoconf\
    automake\
    dos2unix\
    tar\
    rsync\
    python3\
    git\
    build-base \
    curl \
    pkgconfig \
    unzip \
    zip \
    openrc \
    && rm  -rf /tmp.cpp/* /var/cache/apk/*


RUN echo "root:root" | chpasswd

RUN ( \
    echo 'PermitRootLogin yes'; \
    echo 'PasswordAuthentication yes'; \
    ) > /etc/ssh/sshd_config

RUN /usr/bin/ssh-keygen -A

#clone code and build


# run ssh server
CMD ["/usr/sbin/sshd", "-D"]

EXPOSE 22