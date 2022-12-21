all:myqueue Receiver.o Receiver Sender

Receiver:
	gcc -o Receiver Receiver.o myqueue.o -Wall

Receiver.o:
	gcc Receiver.c -c -Wall

Sender:
	gcc -o Sender Sender.c -Wall

myqueue:
	gcc myqueue.c -c -Wall 

clean:
	rm -f *.o Sender Receiver


