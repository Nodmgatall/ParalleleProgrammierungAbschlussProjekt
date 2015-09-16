.PHONY: default clang clean cleanup mrproper opti parallel

### definitions ###
CC = g++
CFLAGS = -g -Wall -Werror -Wextra -Wno-error=unused-parameter -pedantic -std=c++14
OBJS = main.o particle.o simulator.o
EXECUTABLE = Simulation.x

### targets ###
# normal
default: Simulation.x

# debug
debug: CFLAGS += -DDEBUG_BUILD
debug: default

# optimized
opti: CFLAGS = -O3 -pedantic -std=c++14
opti: default

# parallelized
parallel: CC = mpic++
parallel: default

# clang target, why not
clang: CC = clang++
clang: default

#lazy target without any restrictions
lazy: CFLAGS = -g -std=c++14
lazy: default

## Final executable ##
Simulation.x: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o ${EXECUTABLE} ${LFLAGS}

### Object files ###
# TODO: throw all these into one target
# Octree
<<<<<<< HEAD
Octree.o: Octree/Octree.cpp Octree/Octree.hpp Octree/OctreePoint.hpp
	${CC} ${CFLAGS} -c $< ${LFLAGS}

=======
#Octree.o: Octree/Octree.cpp Octree/Octree.hpp Octree/OctreePoint.hpp
#	${CC} ${CFLAGS} -c $<
>>>>>>> 984a0b36a4111415cd3d28bc84ee347ed9003f88

# main
main.o: main.cpp
	${CC} ${CFLAGS} -c $< ${LFLAGS}


# particle
particle.o: particle.cpp particle.hpp
	${CC} ${CFLAGS} -c $< ${LFLAGS}


# simulator
simulator.o: simulator.cpp simulator.hpp
	${CC} ${CFLAGS} -c $< ${LFLAGS}



### Cleanup utils ###
clean:
	rm -f *.x

cleanup:
	rm -f *.x *.lis

mrproper:
	rm -f *.o *.x *.lis
