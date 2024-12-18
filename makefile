TARGET = sed_simplified
.PHONY: all clean run test test1 test2 test3 test4

all: $(TARGET)

clean:
	rm -f *.o $(TARGET)

run: $(TARGET)
	./$(TARGET) $(ARGS) 

main.o: main.c
	gcc -c main.c

lab2.o: lab2.c
	gcc -c lab2.c

$(TARGET): main.o lab2.o
	gcc -Wall main.o lab2.o -o $(TARGET)

test1: $(TARGET)
	cp a.txt test1.txt
	@echo "Running test1..."
	@cat test1.txt
	./$(TARGET) test1.txt 's/[mM]ister/senior/'
	@echo "After test1:"
	@cat test1.txt

test2: $(TARGET)
	cp a.txt test2.txt
	@echo "Running test2..."
	@cat test2.txt
	./$(TARGET) test2.txt '/[mM]ister/d'
	@echo "After test2:"
	@cat test2.txt

test3: $(TARGET)
	cp a.txt test3.txt
	@echo "Running test3..."
	@cat test3.txt
	./$(TARGET) test3.txt 's/^/Start of sentence:/'
	@echo "After test3:"
	@cat test3.txt

test4: $(TARGET)
	cp a.txt test4.txt
	@echo "Running test4..."
	@cat test4.txt
	./$(TARGET) test4.txt 's/$$/- End of sentence/'
	@echo "After test4:"
	@cat test4.txt

tests: test1 test2 test3 test4