main.exe:  CondVar.o cpu.o mutex.o process.o MutexQueue.o Keyboard.o main.c
	gcc CondVar.o cpu.o mutex.o process.o MutexQueue.o Keyboard.o main.c -o main.exe

CondVar.o: CondVar.c
	gcc -c CondVar.c
	
cpu.o: cpu.c
	gcc -c cpu.c
	
MutexQueue.o: MutexQueue.c
	gcc -c MutexQueue.c
	
mutex.o: mutex.c
	gcc -c mutex.c
	
process.o: process.c
	gcc -c process.c
	
Keyboard.o: Keyboard.c
	gcc -c Keyboard.c