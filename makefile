all :
	cd src && gcc -g -o ../main main.c -lglut -lGL -lGLU

clean:
	rm -f main
