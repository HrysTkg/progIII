LDLIBS := -lncurses

tetris: tetris.o

clean:
	$(RM) *.o
	$(RM) block
