FROM ubuntu

RUN apt-get update \
    && apt-get install -y build-essential parallel wget clang libelf1 libelf-dev zlib1g-dev
COPY . /root/

WORKDIR /root/tests/cases/kernel
RUN chmod +x init.sh && ./init.sh

WORKDIR /root/tests
RUN chmod +x init.sh && ./init.sh

WORKDIR /root/preload
RUN ./load_modify.sh

WORKDIR /root
