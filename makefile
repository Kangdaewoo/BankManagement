CFLAGS="-Wall"

console: bank.c bank.h utils.o state.o bank.o
	gcc $(CFLAGS) -o console console.c utils.o state.o bank.o

state.o: state.c state.h
	gcc $(CFLAGS) -c state.c

bank.o: bank.c bank.h
	gcc $(CFLAGS) -c bank.c

utils.o: utils.c utils.h
	gcc $(CFLAGS) -c utils.c

.PHONY: clean
clean:
	rm -f *.o console
