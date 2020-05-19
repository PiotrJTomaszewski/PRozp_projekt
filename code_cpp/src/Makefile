CC = mpic++
CFLAGS = -lpthread --std=c++17 -g -ggdb
CWARNINGFLAGS = -Wall -Wextra -Wpedantic

# OUTDIR = build
DEPS = communication_loop.h condition_var.h debug.h main_loop.h packet.h queues_submarine.h shared_var.h shared_vector.h system_info.h tourist.h

_OBJ = communication_loop.o condition_var.o debug.o main_loop.o packet.o queues_submarine.o system_info.o tourist.o main.o
# OBJ = $(patsubst %,$(OUTDIR)/%,$(_OBJ))
OBJ = $(_OBJ)

# $(OUTDIR)/%.o: %.cpp $(DEPS)
%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) $(CWARNINGFLAGS) -c -o $@ $<

nanozombie.out: $(OBJ)
	$(CC) $(CFLAGS) $(CWARNINGFLAGS) -o $@ $^

.PHONY: clean
clean:
	# rm $(OUTDIR)/*.o
	rm *.o
	rm nanozombie.out

