CC=gcc
CFLAGS=-O0 -I /usr/local -static
VAULT_CPU=2
THIEF_CPU=3

TARGETS=thief
UTILS=util.o
CACHE=cache.o
LINKED_LIST=linked_list.o
.PHONY:	clean all run_vault run_thief

-include thief.d util.d cache.d	linked_list.d

all: $(TARGETS)

$(UTILS): %.o: %.c %.h
	$(CC) $(CFLAGS) -MMD -c $<

$(CACHE): %.o: %.c %.h
	$(CC) $(CFLAGS) -MMD -c $<

$(LINKED_LIST): %.o: %.c %.h
	$(CC) $(CFLAGS) -MMD -c $<

%.o: %.c util.h
	$(CC) $(CFLAGS) -MMD  -c $< 

$(TARGETS): %:%.o util.o cache.o linked_list.o
	$(CC) $(CFLAGS) $^ -o $@

run_vault: vault
	taskset -c $(VAULT_CPU) ./vault

run_thief: thief 
	taskset -c $(THIEF_CPU) ./thief

clean:
	$(RM) *.o *.d  $(HELPERS) $(TARGETS)
# /media/psf/courses/Secure Computer Systems /lab/lab1/lab1_15793/prime_probe
# make -f "/media/psf/courses/Secure Computer Systems /lab/lab1/lab1_15793/prime_probe/Makefile" -C "/media/psf/courses/Secure Computer Systems /lab/lab1/lab1_15793/prime_probe_build_lin" all
