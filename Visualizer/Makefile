.PHONY: clang clean default lazy opti
SHELL = /bin/sh

### definitions ###
CC = g++
CFLAGS = -g -Wall -Werror -Wextra -Wno-error=unused-parameter -pedantic -std=c++14
LFLAGS = -lboost_serialization
OBJS = main.o visualizer.o camera.o 
EXECUTABLE = Visualizer.x

## SDL stuff ##
SDL2_LIBS = $(shell sdl2-config --libs --cflags) -lSDL2_image -lSDL2_ttf
# append SDL2 flags to CFLAGS
LFLAGS += ${SDL2_LIBS}

### targets ###
# normal
default: Visualizer.x

# optimized
opti: CFLAGS = -O3 -pedantic -std=c++14
opti: default

# lazy
lazy: CFLAGS = -g -std=c++14
lazy: default

# clang
clang: CC = clang++
clang: default

## Final executable ##
Visualizer.x: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o ${EXECUTABLE} ${LFLAGS}

### Object files ###
# all in one
%.o: %.cpp
	${CC} ${CFLAGS} -c $< ${LFLAGS}
	

### clean targets ###
clean:
	rm -f *.x

mrproper:
	rm -f *.o *.x

help:
	@echo 'Compilation targets:'
	@echo '  default   - compile with regular flags'
	@echo '  opti      - compile with -O3 optimizations'
	@echo '  lazy      - compile without any restricting flags'
	@echo '  clang     - like default, but use clang instead of gcc'
	@echo ''
	@echo 'Cleaning targets:'
	@echo '  clean     - remove binary'
	@echo '  mrproper  - remove binary as well as object files'
