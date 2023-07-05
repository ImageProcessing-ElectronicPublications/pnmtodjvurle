PROJECT = pnmtodjvurle
CXX = g++
CFLAGS = -Wall -O3 -Isrc
LDFLAGS = -s
RM = rm -f
SRCS = src/pbmtodjvurle.cpp src/pgmtodjvurle.cpp src/ppmtodjvurle.cpp src/pnmtodjvurle.cpp

all: $(PROJECT)

$(PROJECT): $(SRCS)
	$(CXX) $(CFLAGS) $^ $(LDFLAGS) -o $@

clean:
	$(RM) $(PROJECT)
