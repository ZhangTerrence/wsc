compile:
	@gcc example.c ../src/server.c ../src/route.c ../src/http.c -o example

execute: compile
	@./example

execute_mem: compile
	@valgrind ./example

clean:
	@rm ./example vgcore.*