# Random-Tools
#
# copyright 2019 Alec Hitchiner
#
# A set of tools for generating and using random numbers
#
# randomnum: Generates random numbers
# randomselect: Selects random items from a list
# shuffle: Randomly shuffles a list
# coinflip: Simulates random coin flips
#
# To build, run 'make clean' 'make'
# To install, run 'sudo make install'
# To remove, run 'sudo make remove'

CC=g++
CFLAGS=-O3 -Wall -Wextra -pedantic
LFLAGS=

BIN_DIR=bin
LIB_DIR=lib
OBJ_DIR=obj
SRC_DIR=src

OBJS=
BINS=$(BIN_DIR)/coinflip $(BIN_DIR)/randomselect $(BIN_DIR)/randomnum \
	 $(BIN_DIR)/shuffle

all: $(BINS) $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) $(LFLAGS) -I $(LIB_DIR) -c -o $@ $<

$(BIN_DIR)/%: $(SRC_DIR)/%.cpp $(OBJS)
	$(CC) $(CFLAGS) $(LFLAGS) -I $(LIB_DIR) -o $@ $<

.PHONY: clean install

clean:
	rm -f $(OBJS) $(BINS)

install: $(BINS)
	cp -n $(BINS) /bin/

remove:
	for bin in $(BINS); do \
		echo "removing /bin/$${bin/$(BIN_DIR)\//}"; \
		rm "/bin/$${bin/$(BIN_DIR)\//}"; \
	done
