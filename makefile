CFLAGS =

default: client

client:
	gcc -o chatclient chatclient.c $(CFLAGS)

server:
	chmod +x chatserve.py

clean:
	rm chatclient
