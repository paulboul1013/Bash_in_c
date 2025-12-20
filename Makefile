all:
	gcc -Wall -Wextra -o main main.c -lreadline

clean:
	rm -f main