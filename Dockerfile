FROM ubuntu:eoan

COPY . /code/

WORKDIR /code/

RUN apt-get update && \
	apt-get install -y build-essential git cmake autoconf libtool pkg-config libasio-dev nlohmann-json3-dev

RUN mkdir build/
WORKDIR /code/build/

RUN cmake ..
RUN make pikatanuki
CMD ["./pikatanuki/pikatanuki"]
