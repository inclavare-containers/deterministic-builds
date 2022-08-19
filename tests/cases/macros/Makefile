all: macros file files date

macros: macros.c
	gcc macros.c -o macros_$(shell date +%Y%m%d%H%M%S).out

files: file_a.c file_b.c file_folder/file_c.c
	gcc file_a.c -o file_a_$(shell date +%Y%m%d%H%M%S).out
	gcc file_b.c -o file_b_$(shell date +%Y%m%d%H%M%S).out
	gcc file_folder/file_c.c -o file_c_$(shell date +%Y%m%d%H%M%S).out

file: file.c
	gcc file.c -o file_$(shell date +%Y%m%d%H%M%S).out

date: date.c
	gcc date.c -o date_$(shell date +%Y%m%d%H%M%S).out

timestamp: timestamp.c
	gcc timestamp.c -o timestamp_$(shell date +%Y%m%d%H%M%S).out

clean:
	rm -f *.out *.i
