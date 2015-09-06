.PHONY: default clang clean cleanup mrproper opti parallel

### definitions ###
CC = mpic++
CFLAGS = -g -Wall -Werror -Wextra -Wno-error=unused-parameter -pedantic -std=c++14
LFLAGS = -lboost_mpi -lboost_serialization
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

#lazy target without any restrictions
lazy: CFLAGS = -g -std=c++14
lazy: default

## Final executable ##
Simulation.x: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o ${EXECUTABLE} ${LFLAGS}

### Object files ###
# TODO: throw all these into one target
# Octree
#Octree.o: Octree/Octree.cpp Octree/Octree.hpp Octree/OctreePoint.hpp
#	${CC} ${CFLAGS} -c $<

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
