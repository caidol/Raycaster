SDL_CFLAGS = -g `sdl2-config --cflags`
SDL_LDFLAGS = `sdl2-config --libs` -lmingw32 -lSDL2main -lSDL2

INCLUDES = -Isdl2\i686-w64-mingw32\include
LIBS = -Lsdl2\i686-w64-mingw32\lib

COMPILER = gcc

OBJS = raycaster.o

BUILD_DIR = build/
BIN_NAME = raycaster

# top-rule to create program
all: $(BIN_NAME)

# compile other source files
%.o: src/%.c src/%.h
	$(COMPILER) $(SDL_CFLAGS) -c $<

# linking program
$(BIN_NAME): $(OBJS)
	mkdir -p $(BUILD_DIR)
	$(COMPILER) $(OBJS) -o $(BUILD_DIR)/$(BIN_NAME) $(INCLUDES) $(LIBS) $(SDL_LDFLAGS)
	rm *.o

# cleaning everything that can be automatically recreated
clean:
	rm $(BUILD_DIR)/$(BIN_NAME) *.o