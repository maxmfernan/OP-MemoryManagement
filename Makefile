#variables-------------------------------------------------------
CC=gcc
ECFLAGS=-D_XOPEN_SOURCE=700
CFLAGS=-Wall -std=c99 
CFLAGSDBG=-Wall -g -std=c99 

LLIB=-lm

client=cliente
server=server
pap=pap
psp=psp
shutdown=shutdown

executables=$(client) $(server) $(psp) $(shutdown) 
#----------------------------------------------------------------
all: $(client) $(server) $(psp) $(shutdown)

debug: $(client)dbg $(server)dbg $(shutdown)dbg

#Normal compilation
$(client):
	$(CC) $(CFLAGS) cliente.c error_handlers.c -o $@

$(server): 
	$(CC) $(CFLAGS) server.c error_handlers.c -o $@
#Complementary processes
pap:
	$(CC) $(CFLAGS) $(pap).c -o $@
psp:
	$(CC) $(CFLAGS) $(psp).c error_handlers.c -o $@

$(shutdown):
	$(CC) $(CFLAGS) $(shutdown).c error_handlers.c -o $@

#Compilation for debug
$(client)dbg:
	$(CC) $(CFLAGSDBG) -DDEBUG cliente.c error_handlers.c -o $(client)
$(server)dbg: $(psp)dbg 
	$(CC) $(CFLAGSDBG) -DDEBUG server.c error_handlers.c -o $(server)
#Complementary processes
$(pap)dbg:
	$(CC) $(CFLAGS) $(pap).c -o $(pap) 
$(psp)dbg:
	$(CC) $(CFLAGS) $(psp).c error_handlers.c -o $(psp)

$(shutdown)dbg:
	$(CC) $(CFLAGS) -DDEBUG $(shutdown).c error_handlers.c -o $(shutdown)

#Run sever
runs:
	./$(server)

#Run client
runc:
	./$(client)

#Remove executable files
clean:
	rm -f $(executables)
