.PHONY: default clean mrproper opti parallel

### definitions ###
CC = g++
CFLAGS = -g -Wall -Werror -Wextra -Wno-error=unused-parameter -pedantic -std=c++14
OBJS = main.o particle.o simulator.o

### targets ###
# default target to compile executable
default: Simulation.x

# our executable requires a bunch of objects
Simulation.x: ${OBJS} Octree.o
	${CC} ${CFLAGS} ${OBJS} Octree.o -o Simulation.x

# Octree
Octree.o: Octree/Octree.hpp Octree/Octree.cpp Octree/OctreePoint.hpp
	${CC} ${CFLAGS} -c Octree/Octree.cpp

# main
main.o: main.cpp
	${CC} ${CFLAGS} -c main.cpp

# particle
particle.o: particle.cpp
	${CC} ${CFLAGS} -c particle.cpp

# simulator
simulator.o: simulator.cpp
	${CC} ${CFLAGS} -c simulator.cpp



clean:
	rm *.o *.x

mrproper:
	rm -f *.o *.x *.simdat
