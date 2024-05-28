LDFLAGS := -lglfw

opengl:
	g++ *.cpp *.c $(LDFLAGS)

clean:
	rm a.out
