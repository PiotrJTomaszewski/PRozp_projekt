CC = mpicc
CFLAGS = -lpthread
CWARNINGFLAGS = -Wall -Wextra -Wpedantic

SOURCEDIR = src
OUTDIR = build

_DEPS = communication_loop.h cond_signal.h debug.h globals.h main_loop.h messages.h shared_var.h stack.h system_info.h tourist.h 
DEPS = $(patsubst %,$(SOURCEDIR)/%,$(_DEPS))

_OBJ = communication_loop.o cond_signal.o debug.o main_loop.o messages.o shared_var.o stack.o system_info.o tourist.o main.o
OBJ = $(patsubst %,$(OUTDIR)/%,$(_OBJ))


$(OUTDIR)/%.o: $(SOURCEDIR)/%.c $(DEPS)
	$(CC) $(CFLAGS) $(CWARNINGFLAGS) -c -o $@ $<

nanozombie: $(OBJ)
	$(CC) $(CFLAGS) $(CWARNINGFLAGS) -o $@ $^ 


.PHONY: clean
clean:
	rm $(OUTDIR)/*
	rm logs/*