#variables-------------------------------------------------------
CC=gcc

CFLAGS=-Wall -std=c99
CFLAGSDBG=-Wall -g -std=c99

LLIB=-lm

client=cliente
server=server
pap=pap
psp=psp

executables=$(client) $(server) $(pap) 
#----------------------------------------------------------------
all: $(client) $(server) $(pap) 

debug: $(client)dbg $(server)dbg

#Normal compilation
$(client):
	$(CC) $(CFLAGS) cliente.c error_handlers.c -o $@

$(server): 
	$(CC) $(CFLAGS) server.c error_handlers.c -o $@
#Complementary processes
pap:
	$(CC) $(CFLAGS) $(pap).c -o $@
psp:
	$(CC) $(CFLAGS) $(psp).c -o $@

#Compilation for debug
$(client)dbg:
	$(CC) $(CFLAGSDBG) cliente.c error_handlers.c -o $(client)
$(server)dbg: $(pap)dbg 
	$(CC) $(CFLAGSDBG) server.c error_handlers.c -o $(server)
#Complementary processes
$(pap)dbg:
	$(CC) $(CFLAGS) $(pap).c -o $(pap) 
$(psp)dbg:
	$(CC) $(CFLAGS) $(psp).c -o $(psp)

#Run sever
runs:
	./$(server)

#Run client
runc:
	./$(client)

#Remove executable files
clean:
	rm -f $(executables)
