#ifndef _MENUUTIL_H
#define _MENUUTIL_H

#include <stdio.h>

#define DEFAULT_BUF_LEN 16

typedef enum cmdType {QUIT, READ, WRITE, CROP, INVERT, SWAP, GRAYSCALE, BLUR, SHARPEN, BRIGHTEN, CONTRAST, NOOP, UNKNOWN} CommandType;

void menuLoop(FILE *fin, FILE *fout);

char* readWord(FILE *fp);

void printMenu(FILE *fp);

/* process command string, convert it to
 * a #defined value */
CommandType getCommand(FILE *fin);

#endif
