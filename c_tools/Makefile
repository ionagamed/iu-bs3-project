CC=cc
CFLAGS=-g -Wall -DLOG_USE_COLOR
LIBS=-lcrypto

default: all

all: crack_ec_linear crack_ec_pollard generate_24bit_curve generate_curve_by_params crack_ec_smart

output_directory:
	mkdir -p ./bin

clean:
	rm -rf ./bin

logger: output_directory lib/log.c
	$(CC) $(CFLAGS) -c -o bin/log.o lib/log.c

common: output_directory lib/common.c
	$(CC) $(CFLAGS) -c -o bin/common.o lib/common.c

ec_common: output_directory lib/ec.c
	$(CC) $(CFLAGS) -c -o bin/ec.o lib/ec.c

crack_ec_linear: output_directory logger common src/crack_ec_linear.c
	$(CC) $(CFLAGS) -o bin/crack_ec_linear src/crack_ec_linear.c bin/log.o bin/common.o $(LIBS)

crack_ec_pollard: output_directory logger common src/crack_ec_pollard.c
	$(CC) $(CFLAGS) -o bin/crack_ec_pollard src/crack_ec_pollard.c bin/log.o bin/common.o $(LIBS)

generate_24bit_curve: output_directory logger common ec_common src/generate_24bit_curve.c
	$(CC) $(CFLAGS) -o bin/generate_24bit_curve src/generate_24bit_curve.c bin/log.o bin/common.o bin/ec.o $(LIBS)

generate_curve_by_params: output_directory logger common ec_common src/generate_24bit_curve.c
	$(CC) $(CFLAGS) -o bin/generate_curve_by_params src/generate_curve_by_params.c bin/log.o bin/common.o bin/ec.o $(LIBS)

crack_ec_smart: output_directory logger src/crack_ec_smart.c
	$(CC) $(CFLAGS) -o bin/crack_ec_smart src/crack_ec_smart.c bin/log.o $(LIBS)
