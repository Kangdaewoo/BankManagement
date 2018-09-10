CFLAGS="-Wall"

console: bank.c bank.h utils.o bank.o state.o server.o
	gcc $(CFLAGS) -pthread -o console console.c state.o bank.o utils.o server.o

server.o: server.c server.h
	gcc $(CFLAGS) -c server.c

state.o: state.c state.h
	gcc $(CFLAGS) -c state.c

bank.o: bank.c bank.h
	gcc $(CFLAGS) -c bank.c

utils.o: utils.c utils.h
	gcc $(CFLAGS) -c utils.c

.PHONY: clean
clean:
	rm -f *.o console client
