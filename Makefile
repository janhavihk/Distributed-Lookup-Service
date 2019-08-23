#compiling source codes

.PHONY: clean all serverA serverB serverC aws monitor

all: clean compile

compile: client.c aws.c monitor.c serverA.c serverB.c serverC.c
		gcc client.c -o client
		gcc aws.c -o aws
		gcc monitor.c -o monitor
		gcc	serverA.c -o serverA
		gcc serverB.c -o serverB
		gcc serverC.c -o serverC

serverA:
		./serverA

serverB:
		./serverB

serverC:
		./serverC

aws:
		./aws

monitor:
		./monitor

clean:
		rm -rf *.o serverA serverB serverC monitor aws client

