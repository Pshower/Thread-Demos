.PHONY: run clean


ALL = main.c

run:
	@gcc $(ALL).c -o $(ALL).o
	@./$(ALL).o
	@make clean

clean:
	@rm *.o