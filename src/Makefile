#Makefile

rmdup: bin rmdup.o listrf.o sortrf.o readrf.o rfent.o compfiles.o
	gcc -o bin/rmdup rmdup.o listrf.o sortrf.o readrf.o rfent.o compfiles.o -Wall
 
rmdup.o: rmdup.c listrf.h sortrf.h readrf.h sizelim.h
	gcc -o rmdup.o -c rmdup.c -Wall

listrf.o: lsdir listrf.c listrf.h sizelim.h
	gcc -o listrf.o -c listrf.c -Wall
	
sortrf.o: sortrf.c sortrf.h rfent.h
	gcc -o sortrf.o -c sortrf.c -Wall
	
readrf.o: readrf.c readrf.h rfent.h sizelim.h
	gcc -o readrf.o -c readrf.c -Wall
	
rfent.o: rfent.c rfent.h compfiles.h sizelim.h
	gcc -o rfent.o -c rfent.c -Wall
  
compfiles.o: compfiles.c compfiles.h
	gcc -o compfiles.o -c compfiles.c -Wall
  
lsdir: bin lsdir.o
	gcc -o bin/lsdir lsdir.o -Wall
	
lsdir.o: bin lsdir.c rfent.h sizelim.h
	gcc -o lsdir.o -c lsdir.c -Wall
  
bin:
	mkdir bin