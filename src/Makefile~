###################################################
# Sample Makefile for HW6
# Ben Mitchell
# 2015-09-24
#
# More advanced makefile than the one we used for HW4;
# files are split between multiple directories.
# Also uses generic rules and more advanced substitutions,
# as well as automatic dependency calculation
#
###################################################

###############################
# path variables
###############################
# where to look for .h files
INC_DIRS = ../include
# where to put .o files
OBJ_DIR = ./obj
# where to put executables
BIN_DIR = ../bin
 
# make sure the output directories exists...
$(shell mkdir -p $(OBJ_DIR))
$(shell mkdir -p $(BIN_DIR))

###############################
# targets 
###############################


$(BIN_DIR)/a6: $(OBJ_DIR)/a6.o $(OBJ_DIR)/menuUtil.o $(OBJ_DIR)/ppmIO.o $(OBJ_DIR)/imageManip.o
	gcc -g -lm $(OBJ_DIR)/a6.o $(OBJ_DIR)/menuUtil.o $(OBJ_DIR)/ppmIO.o $(OBJ_DIR)/imageManip.o -o $(BIN_DIR)/a6

$(OBJ_DIR)/a6.o: a6.c 
	gcc -std=c99 -Wall -Wextra -pedantic -g -I../include/ -c a6.c -o $(OBJ_DIR)/a6.o

$(OBJ_DIR)/menuUtil.o: menuUtil.c $(INC_DIRS)/menuUtil.h $(INC_DIRS)/imageManip.h $(INC_DIRS)/ppmIO.h
	gcc -std=c99 -Wall -Wextra -pedantic -g -I../include/ -c menuUtil.c -o $(OBJ_DIR)/menuUtil.o

$(OBJ_DIR)/ppmIO.o: ppmIO.c $(INC_DIRS)/ppmIO.h
	gcc -std=c99 -Wall -Wextra -pedantic -g -I../include/ -c ppmIO.c -o $(OBJ_DIR)/ppmIO.o

$(OBJ_DIR)/imageManip.o: imageManip.c $(INC_DIRS)/imageManip.h
	gcc -std=c99 -Wall -Wextra -pedantic -g -I../include/ -c imageManip.c -o $(OBJ_DIR)/imageManip.o

$(OBJ_DIR):


###############################
# "utility" rules
###############################
# the .PHONY line says these targets aren't expected to generate files named after the target
.PHONY: clean a6

# "shortcut" target (so we don't have to type the bin dir before our target name)
a6: $(BIN_DIR)/a6

# clean up generated files
clean:
	@echo removing generated files...
	@rm -f $(OBJ_DIR)/*.o 
