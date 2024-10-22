CC = clang
TARGET = server

$(TARGET): X.o main.o math_operations.o server_operations.o
	$(CC) $^ -o $(TARGET)

main.o: main.c
	$(CC) -c -Wall -Wextra -o $@ $^

server_operations.o: server_operations.c 
	$(CC) -c -Wall -Wextra -o $@ $^

X.o: X.c
	$(CC) -c -Wall -Wextra -o $@ $^

math_operations.o: math_operations.c 
	$(CC) -c -Wall -Wextra -o $@ $^

.PHONY: clean cleanall

clean:
	rm -rf *.o

cleanall: clean
	rm -rf $(TARGET)
