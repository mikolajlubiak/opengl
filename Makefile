LDFLAGS := -lglfw

debug:
	g++ *.cpp *.c $(LDFLAGS) --debug -o opengl

clean:
	rm a.out
