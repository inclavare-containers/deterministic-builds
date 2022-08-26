all: f.out

%.o: %.cpp
	g++ -flto -o $@ -c $^

f.out: f1.o f2.o
	g++ -flto -o f.out $^

clean:
	rm *.out *.o
