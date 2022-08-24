all: f.out

f1.o:
	g++ -flto -c f1.cpp

f.out: f1.o f2.o
	g++ -flto -o f.out $^

%.o: %.cpp
	g++ -flto -c $^

clean:
	rm *.out *.o
