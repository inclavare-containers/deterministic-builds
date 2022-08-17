all: modify_time.so modify_f4ke.so

modify_time.so: modify_time.c
	gcc -shared -fPIC modify_time.c -o modify_time.so

modify_f4ke.so: modify_f4ke.c
	gcc -shared -fPIC modify_f4ke.c -o modify_f4ke.so

clean: 
	rm -rf modify_time.so modify_f4ke.so
