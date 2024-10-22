CC = clang
TARGET = server

# Добавляем новый объектный файл math_operations.o
$(TARGET): X.o main.o math_operations.o
	$(CC) $^ -o $(TARGET)

main.o: main.c
	$(CC) -c -Wall -Wextra -o $@ $^

X.o: X.c
	$(CC) -c -Wall -Wextra -o $@ $^

# Добавляем правило для компиляции библиотеки math_operations
math_operations.o: math_operations.c
	$(CC) -c -Wall -Wextra -o $@ $^

.PHONY: clean cleanall

clean:
	rm -rf *.o

cleanall: clean
	rm -rf $(TARGET)
