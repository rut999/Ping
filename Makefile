# Rutvik Parvataneni
# Email :: parvatanenirutvik@gmail.com
# Makefile to link the header & c file respictively


CC = gcc 
OBJS = main.o 
FILES = main.c my_ping.h Makefile 
EXEC = ping 

# builds the project
ping: $(OBJS)
	@$(CC) -o $(EXEC) $(OBJS) -w

# compiles main.c into its respective object (.o) file
main.o: main.c my_ping.h
	@$(CC) -c main.c -w
	@echo 'make (Rutvik_ping) : Done with creating the object file --> ping'
	@echo 'make (Rutvik_ping) : Please run the application with sudo permission'


# removes the .o and executable files from the source directory
clean:
	@rm -f *.o $(EXEC)
	@echo 'make clean (Rutvik_ping) : Removed the .o and executable files from the source directory'
