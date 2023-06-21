all:
	gcc main_normal.c -o main_normal
	gcc main_roubo.c -o main_roubo
	gcc lento.c -o lento
	gcc normal.c -o normal
	gcc rapido.c -o rapido

clean:
	rm -f main_normal
	rm -f main_roubo
	rm -f lento
	rm -f normal
	rm -f rapido