# Version
VERSION = 0.0.1



Objects = mathew.o option.o error.o output.o table.o \
	tool.o parse.o lexer.o queue.o ffsets.o set.o \
	state.o string.o action.o generate.o map.o acttab.o

mathew:$(Objects)
	gcc -o mathew $(Objects)

$(Objects):mathew.h
 	
clean:
	rm  $(Objects)