SDL2_LIBS = $(shell sdl2-config --libs --cflags) -lSDL2_image

all:
	g++ -g -Werror -pedantic -Wall -Wextra -Wno-error=unused-parameter -std=c++14 -o Simulation.x *.cpp $(SDL2_LIBS)
lazy:
	g++ -g -std=c++14 -o Simulation.x *.cpp $(SDL2_LIBS)
clean:
	rm *.x