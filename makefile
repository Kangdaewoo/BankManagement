CFLAGS="-Wall"

main: main.c utils.o bank.o state.o server.o
	gcc $(CFLAGS) -pthread -o $@ $< $^

server.o: server.c server.h
	gcc $(CFLAGS) -c $<

state.o: state.c state.h
	gcc $(CFLAGS) -c $<

bank.o: bank.c bank.h
	gcc $(CFLAGS) -c $<

utils.o: utils.c utils.h
	gcc $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm -f *.o main
