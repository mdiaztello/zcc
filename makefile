

#------OUTPUTS-------#
COMPONENT_NAME = mcc
#Set this to keep the makefile quiet
SILENCE = @

#-----INPUTS---------#
PROJECT_HOME_DIR = .
CPPUTEST_HOME = /home/zorak/projects/CppUTest

CPP_PLATFORM = Gcc

SRC_DIRS = src
TEST_SRC_DIRS = tests
MOCKS_SRC_DIRS = mocks

INCLUDE_DIRS =\
  .\
  include \
  $(CPPUTEST_HOME)/include/ \
  $(CPPUTEST_HOME)/include/Platforms/Gcc\
  mocks

CPPUTEST_WARNINGFLAGS = -Wall -Wswitch-default -Wno-write-strings

# Need to turn off memory leak detection in CppUTest for now because there are
# memory leaks in the tests as they are currently implemented, and I don't feel
# it is necessary to fix them right now.
CPPUTEST_USE_MEM_LEAK_DETECTION = N

include $(CPPUTEST_HOME)/build/MakefileWorker.mk







# Simple makefile that will compile all the .c files in the current directory
# and make an executable called "main.exe" in the same directory
# to build the project, type "make" in the terminal
# to remove all the non-source files, type "make clean" in the terminal
CC := gcc

# catch issue additional warnings and make them into hard errors
# CFLAGS := -Wall -Wextra -Werror -std=c99
CFLAGS := -Wall -Wextra -std=c99


mcc: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)


.PHONY: my_clean

my_clean:
	rm mcc

ctags:
	ctags src/*.c include/*.h

