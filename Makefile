#============================================================================
# Make file for Profiler
#
# CS II Kent State University
#
# J. Maletic
# Copyright 2023 Kent State University. All rights reserved.
# Spring 2023
# srcML 1.0
#
# Modified by: Zaz Brown
#


###############################################################
# Variables
CPP      = clang++
CPP_OPTS = -g -Wall -W -Wunused -Wuninitialized -Wshadow -std=c++17

###############################################################
# The first rule is run if only make is typed
msg:
	@echo 'Targets are:'
	@echo '  profiler  - Compile all profiler code.'
	@echo '  p-simple  - Compile p-simple          '
	@echo '  sort      - Compile sort code.        '
	@echo '  p-sort    - Compile p-sort code.      '
	@echo '  clean     - Remove executables and .o.'
	@echo '  test      - Compile tests.'
	@echo '  check     - Tests for memory leaks using Valgrind.'

###############################################################
profiler: main.o profileFiles.o ASTree.o
	$(CPP) $(CPP_OPTS) -o profiler main.o profileFiles.o ASTree.o

test: test.o profileFiles.o ASTree.o
	$(CPP) $(CPP_OPTS) -o test test.o profileFiles.o ASTree.o

main.o: main.cpp ASTree.hpp
	$(CPP) $(CPP_OPTS) -c main.cpp

test.o: test.cpp ASTree.hpp
	$(CPP) $(CPP_OPTS) -c test.cpp

profileFiles.o: profileFiles.cpp profileFiles.hpp
	$(CPP) $(CPP_OPTS) -c profileFiles.cpp

ASTree.o: ASTree.hpp ASTree.cpp
	$(CPP) $(CPP_OPTS) -c ASTree.cpp



#==============================================================
# Compile profile.cpp
profile.o: profile.hpp profile.cpp
	$(CPP) $(CPP_OPTS) -c profile.cpp


#==============================================================
# p-simple
p-simple: p-simple.o profile.o
	$(CPP) $(CPP_OPTS) -o p-simple p-simple.o profile.o

p-simple.o: p-simple.cpp profile.hpp
	$(CPP) $(CPP_OPTS) -c p-simple.cpp



#==============================================================
# sort
sort: sort.o sort_lib.o
	$(CPP) $(CPP_OPTS) -o sort sort.o sort_lib.o

sort.o: sort_lib.h sort.cpp
	$(CPP) $(CPP_OPTS) -c sort.cpp

sort_lib.o: sort_lib.h sort_lib.cpp
	$(CPP) $(CPP_OPTS) -c sort_lib.cpp

#==============================================================
# p-sort
# p-sort.cpp
# p-sort_lib.cpp

p-sort: profile.o p-sort.o p-sort_lib.o
	$(CPP) $(CPP_OPTS) -o p-sort profile.o p-sort.o p-sort_lib.o

p-sort.o: profile.hpp sort_lib.h p-sort.cpp
	$(CPP) $(CPP_OPTS) -c p-sort.cpp

p-sort_lib.o: profile.hpp sort_lib.h p-sort_lib.cpp
	$(CPP) $(CPP_OPTS) -c p-sort_lib.cpp


###############################################################
#This will clean up everything via "make clean"
clean:
	rm -f profiler
	rm -f test
	rm -f sort
	rm -f *.o
	rm -f p-*

check:
	make test
	valgrind --leak-check=full --error-exitcode=1 --quiet \
	         --show-possibly-lost=no ./test
	make profiler
	valgrind --leak-check=full --error-exitcode=1 --quiet \
	         --show-possibly-lost=no ./profiler simple.cpp.xml >/dev/null
