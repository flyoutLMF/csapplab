#
# Students' Makefile for the Malloc Lab
#
TEAM = bovik
VERSION = 1
HANDINDIR = 刘慕梵19307130248_handin

CC = gcc
CFLAGS = -Wall -O2 -m32

OBJS = mdriver.o mm.o memlib.o fsecs.o fcyc.o clock.o ftimer.o

mdriver: $(OBJS)
	$(CC) $(CFLAGS) -o mdriver $(OBJS)

mdriver.o: mdriver.c fsecs.h fcyc.h clock.h memlib.h config.h mm.h
memlib.o: memlib.c memlib.h
mm.o: mm.c mm.h memlib.h
fsecs.o: fsecs.c fsecs.h config.h
fcyc.o: fcyc.c fcyc.h
ftimer.o: ftimer.c ftimer.h config.h
clock.o: clock.c clock.h

handin:
	cp mm.c ./$(HANDINDIR)
	cp 刘慕梵19307130248_malloclab_report.pdf ./$(HANDINDIR)
	tar cvf 刘慕梵19307130248_handin.tar 刘慕梵19307130248_handin/

clean:
	rm -f *~ *.o mdriver


