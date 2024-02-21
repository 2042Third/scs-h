CC=gcc
CFLAGS=-O0 -I /usr/local -static
VAULT_CPU=0
THIEF_CPU=1

TARGETS=thief
UTILS=util.o 
.PHONY:	clean all run_vault run_thief

-include thief.d util.d

all: $(TARGETS)

$(UTILS): %.o: %.c %.h
	$(CC) $(CFLAGS) -MMD -c $<

%.o: %.c util.h
	$(CC) $(CFLAGS) -MMD  -c $< 

$(TARGETS): %:%.o util.o
	$(CC) $(CFLAGS) $^ -o $@

run_vault: vault
	taskset -c $(VAULT_CPU) ./vault

run_thief: thief 
	taskset -c $(THIEF_CPU) ./thief

clean:
	$(RM) *.o *.d  $(HELPERS) $(TARGETS)
# /media/psf/courses/Secure Computer Systems /lab/lab1/lab1_15793/prime_probe
# make -f "/media/psf/courses/Secure Computer Systems /lab/lab1/lab1_15793/prime_probe/Makefile" -C "/media/psf/courses/Secure Computer Systems /lab/lab1/lab1_15793/prime_probe_build_lin" all
