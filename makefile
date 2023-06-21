all:
	gcc main.c -o main
	gcc main_roubo.c -o main_roubo
	gcc lento.c -o lento
	gcc normal.c -o normal
	gcc rapido.c -o rapido

clean:
	rm -f main