.PHONY: default clang clean cleanup mrproper opti parallel

### definitions ###
CC = g++
CFLAGS = -pg -g -Wall -Werror -Wextra -Wno-error=unused-parameter -pedantic -std=c++14
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
parallel: CFLAGS = -O2 -pedantic -std=c++14 -pg
parallel: CFLAGS += -DPARALLEL_BUILD
#parallel: CFLAGS += -DOUTPUT_DEBUG
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

# main
main.o: main.cpp
	${CC} ${CFLAGS} -c $< ${LFLAGS}


# particle
particle.o: particle.cpp particle.hpp physics.hpp
	${CC} ${CFLAGS} -c $< ${LFLAGS}


# simulator
simulator.o: simulator.cpp simulator.hpp physics.hpp
	${CC} ${CFLAGS} -c $< ${LFLAGS}



### Cleanup utils ###
clean:
	rm -f *.x

cleanup:
	rm -f *.x *.lis *.simdat

mrproper:
	rm -f *.o *.x *.lis *.simdat

help:
	@echo 'Compilation targets:'
	@echo '  default     - Compile the basic sequentiel version without optimizations'
	@echo '  debug       - Like default, but with more debugging output'
	@echo '  opti        - Compile sequentiel version with -O3 optimizations and without -W flags'
	@echo '  clang       - Default target, but with clang instead of g++'
	@echo '  lazy        - only use -g and -std flags'
	@echo '  parallel    - Compile the parallel version, with -O2 optimizations'
	@echo ''
	@echo 'Cleanup targets:'
	@echo '  clean       - remove the compiled binary'
	@echo '  cleanup     - remove binary and simulated data files'
	@echo '  mrproper    - remove binary, simulated data and object files'
