LDFLAGS := -lglfw -lassimp

debug:
	g++ *.cpp *.c $(LDFLAGS) --debug -o opengl

clean:
	rm -f opengl
