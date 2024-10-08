FROM ubuntu:oracular AS builder

RUN apt-get update && \
	apt-get install -y build-essential git cmake autoconf libtool pkg-config libasio-dev nlohmann-json3-dev

FROM builder

COPY . /code/
RUN rm -rf /code/build/
RUN mkdir /code/build/
WORKDIR /code/build/

RUN cmake ..
RUN make
RUN make install

