OPENCV_INCLUDE=$(shell pkg-config --cflags opencv)
OPENCV_LIB=$(shell pkg-config --libs opencv)
CC=g++
CFLAGS=-g $(OPENCV_INCLUDE)
LDFLAGS = -L$(OPENCV_LIB)
TARGETS=test
all: $(TARGETS)
huffman_encoder.o:include/huffman.h src/huffman_encoder.cpp
	$(CC) -c $^ $(CFLAGS) $(LDFLAGS) -std=c++11
huffman_decoder.o:include/huffman.h src/huffman_decoder.cpp
	$(CC) -c $^ $(CFLAGS) $(LDFLAGS) -std=c++11
transform.o:include/transform.h src/transform.cpp
	$(CC) -c $^ $(CFLAGS) $(LDFLAGS) -std=c++11
test:main.cpp transform.o huffman_encoder.o huffman_decoder.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -std=c++11
clean:
	rm -rf $(TARGETS)
