# where to look for .h files
INC_DIRS = ../include
# where to put .o files
OBJ_DIR = ./obj
# where to put executables
BIN_DIR = ../bin
 
# make sure the output directories exists...
$(shell mkdir -p $(OBJ_DIR))
$(shell mkdir -p $(BIN_DIR))

$(BIN_DIR)/driver: $(OBJ_DIR)/driver.o $(OBJ_DIR)/menuUtil.o $(OBJ_DIR)/ppmIO.o $(OBJ_DIR)/imageManip.o
	gcc -g -lm $(OBJ_DIR)/driver.o $(OBJ_DIR)/menuUtil.o $(OBJ_DIR)/ppmIO.o $(OBJ_DIR)/imageManip.o -o $(BIN_DIR)/driver

$(OBJ_DIR)/driver.o: driver.c 
	gcc -std=c99 -Wall -Wextra -pedantic -g -I../include/ -c driver.c -o $(OBJ_DIR)/driver.o

$(OBJ_DIR)/menuUtil.o: menuUtil.c $(INC_DIRS)/menuUtil.h $(INC_DIRS)/imageManip.h $(INC_DIRS)/ppmIO.h
	gcc -std=c99 -Wall -Wextra -pedantic -g -I../include/ -c menuUtil.c -o $(OBJ_DIR)/menuUtil.o

$(OBJ_DIR)/ppmIO.o: ppmIO.c $(INC_DIRS)/ppmIO.h
	gcc -std=c99 -Wall -Wextra -pedantic -g -I../include/ -c ppmIO.c -o $(OBJ_DIR)/ppmIO.o

$(OBJ_DIR)/imageManip.o: imageManip.c $(INC_DIRS)/imageManip.h
	gcc -std=c99 -Wall -Wextra -pedantic -g -I../include/ -c imageManip.c -o $(OBJ_DIR)/imageManip.o

$(OBJ_DIR):

.PHONY: clean a6

a6: $(BIN_DIR)/a6
# clean up generated files
clean:
	@echo removing generated files...
	@rm -f $(OBJ_DIR)/*.o 
