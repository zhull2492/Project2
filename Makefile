CC = g++ -g3 -lpthread

all: googleServer googleTest receiver

communication.o: communication.cpp
	$(CC) -c communication.cpp

googlehelp.o: googlehelp.cpp
	$(CC) -c googlehelp.cpp

googleServer: googleServer.o communication.o googlehelp.o
	$(CC) -o googleServer googleServer.o communication.o googlehelp.o

googleServer.o: googleServer.cpp
	$(CC) -c googleServer.cpp

googleTest.o: googleTest.cpp
	$(CC) -c googleTest.cpp

googleTest: googleTest.o communication.o googlehelp.o
	$(CC) -o googleTest googleTest.o communication.o googlehelp.o

receiver.o: receiver.cpp
	$(CC) -c receiver.cpp

receiver: receiver.o communication.o googlehelp.o index.o reducer.o
	$(CC) -o receiver receiver.o communication.o googlehelp.o index.o reducer.o

reducer.o: reducer.cpp
	$(CC) -c reducer.cpp

clean:
	$(RM) googleServer googleTest receiver *.o *~
