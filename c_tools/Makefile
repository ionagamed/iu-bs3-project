CC=cc
CFLAGS=-g -Wall -DLOG_USE_COLOR
LIBS=-lcrypto

default: all

all: crack_ec_linear crack_ec_pollard generate_24bit_curve

output_directory:
	mkdir -p ./bin

clean:
	rm -rf ./bin

logger: output_directory lib/log.c
	$(CC) $(CFLAGS) -c -o bin/log.o lib/log.c

common: output_directory lib/common.c
	$(CC) $(CFLAGS) -c -o bin/common.o lib/common.c

crack_ec_linear: output_directory logger common src/crack_ec_linear.c
	$(CC) $(CFLAGS) -o bin/crack_ec_linear src/crack_ec_linear.c bin/log.o bin/common.o $(LIBS)

crack_ec_pollard: output_directory logger common src/crack_ec_pollard.c
	$(CC) $(CFLAGS) -o bin/crack_ec_pollard src/crack_ec_pollard.c bin/log.o bin/common.o $(LIBS)

generate_24bit_curve: output_directory logger common src/generate_24bit_curve.c
	$(CC) $(CFLAGS) -o bin/generate_24bit_curve src/generate_24bit_curve.c bin/log.o bin/common.o $(LIBS)
