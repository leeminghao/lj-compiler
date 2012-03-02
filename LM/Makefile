# The file compile the LJ Machine

CC      = g++
CFLAGS  = -Wall
OBJECTS = main.o ljmachine.o

LM : $(OBJECTS)
	$(CC) $(CFLAGS) -o LM $(OBJECTS)

main.o : main.cpp ljmachine.cpp ljmachine.h
	$(CC) $(CFLAGS) -c main.cpp ljmachine.cpp

ljmachine.o : ljmachine.cpp ljmachine.h
	$(CC) $(CFLAGS) -c ljmachine.cpp

.PHONY: cleanall cleanobj cleanexe
cleanall: cleanobj cleanexe

cleanobj:
	$(RM) *.o

cleanexe:
	$(RM) LM