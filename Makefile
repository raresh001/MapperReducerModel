.PHONY: build clean

GCC = g++

build:
	make tema1

tema1: main.o Application.o FileInfo.o Mapper.o Reducer.o pthreadWrappers/Barrier.o \
		pthreadWrappers/Mutex.o pthreadWrappers/Semaphore.o pthreadWrappers/Thread.o
	$(GCC) -o tema1 $^

%.o: %.c
	$(GCC) -o $@ $<

clean:
	rm -rf *.o pthreadWrappers/*.o tema1
