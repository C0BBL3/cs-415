pseudo-shell: main.o command.o string_parser.o string_alteration.o
	gcc -o pseudo-shell -W -Wall main.c  command.c string_parser.c string_alteration.c

main.o: main.c main.h
	gcc -c $^
	
command.o: command.c command.h
	gcc -c $^

string_parser.o: string_parser.c string_parser.h
	gcc -c $^

string_alteration.o: string_alteration.c string_alteration.h
	gcc -c $^
	
clean:
	rm -f core *.o *.h.gch pseudo-shell