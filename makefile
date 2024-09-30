.PHONY: run clean


ALL = main

run:
	@gcc $(ALL).c -o $(ALL).o -lpthread
	@./$(ALL).o
	@make clean

clean:
	@rm *.o