PROJECT = pnmtodjvurle
CC = gcc
CFLAGS = -Wall -O3 -Isrc
LDFLAGS = -s
RM = rm -f
SRCS = src/pbmtodjvurle.c src/pgmtodjvurle.c src/ppmtodjvurle.c src/pnmtodjvurle.c

all: $(PROJECT)

$(PROJECT): $(SRCS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

clean:
	$(RM) $(PROJECT)
