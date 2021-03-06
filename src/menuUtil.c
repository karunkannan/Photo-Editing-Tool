#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "menuUtil.h"
#include "ppmIO.h"
#include "imageManip.h"




char* readWord (FILE *fp) {
  if (!fp) {
    fprintf(stderr, "ERROR: readWord - got a bad pointer\n");
    return NULL;
  }

  int bufSize = DEFAULT_BUF_LEN;
  char *buf = malloc(bufSize * sizeof(char));
  int wordLen = 0;
  char ch;

  if (!buf) { // make sure we got space
    return NULL;
  }

  while ( (ch = fgetc(fp)) != EOF && !isspace(ch) ) {
    buf[wordLen++] = ch;
    //need to resize if needed
    if (wordLen >= bufSize-1) {
      bufSize *= 2;
      char *tmp = realloc(buf, bufSize * sizeof(char));
      if (!tmp) {
        free(buf);
        return NULL;
      }
      buf = tmp;
    }
  }
  buf[wordLen] = '\0';
  buf = realloc(buf, (wordLen+1) * sizeof(char));
  return buf;
}


void printMenu (FILE *fp) {
  if (!fp) {
    fprintf(stderr, "ERROR: printMenu - got a bad pointer\n");
    return;
  }

  fprintf(fp, "Main menu:\n");
  fprintf(fp, "\tr <filename> - read image from <filename>\n");
  fprintf(fp, "\tw <filename> - write image to <filename>\n");
  fprintf(fp, "\tc <x1> <y1> <x2> <y2> - crop image to the box with the given corners\n");
  fprintf(fp, "\ti - invert intensities\n");
  fprintf(fp, "\ts - swap color chanels\n");
  fprintf(fp, "\tg - convert to grayscale\n");
  fprintf(fp, "\tbl <sigma> - Gaussian blur with the given radius (sigma)\n");
  fprintf(fp, "\tsh <sigma> <amt> - sharpen by given amount (intensity), with radius (sigma)\n");
  fprintf(fp, "\tbr <amt> - change brightness (up or down) by the given amount\n");
  fprintf(fp, "\tcn <amt> - change contrast (up or down) by the given amount\n");
  fprintf(fp, "\tq - quit\n");
  fprintf(fp, "Enter choice: ");
  fflush(fp); //flush buffer to the screen 
}

CommandType getCommand(FILE *fin) {
  if (!fin) {
    fprintf(stderr, "ERROR: getCommand - got a bad pointer\n");
    return UNKNOWN;
  }
  char *command = readWord(fin);
  CommandType cmd = UNKNOWN;

  if (!command) { // if we didn't get a string back, return UNKNOWN
    return cmd;
  }

  // check the string
  if (strcmp(command, "q") == 0) {
    cmd = QUIT;
  } else if (strcmp(command, "") == 0) {
    cmd = NOOP;
  } else if (strcmp(command, "r") == 0) {
    cmd = READ;
  } else if (strcmp(command, "w") == 0) {
    cmd = WRITE;
  } else if (strcmp(command, "c") == 0) {
    cmd = CROP;
  } else if (strcmp(command, "i") == 0) {
    cmd = INVERT;
  } else if (strcmp(command, "s") == 0) {
    cmd = SWAP;
  } else if (strcmp(command, "bl") == 0) {
    cmd = BLUR;
  } else if (strcmp(command, "sh") == 0) {
    cmd = SHARPEN;
  } else if (strcmp(command, "br") == 0) {
    cmd = BRIGHTEN;
  } else if (strcmp(command, "cn") == 0) {
    cmd = CONTRAST;
  } else if (strcmp(command, "g") == 0) {
    cmd = GRAYSCALE;
  } 
  free(command);

  return cmd;
}


/* main loop to get/process input */
void menuLoop (FILE *fin, FILE *fout) {
  if (!fin || !fout) {
    fprintf(stderr, "ERROR: menuLoop - got a bad pointer\n");
    return;
  }

  bool done = false; // are we done yet?
  Image *im = NULL; // main image buffer
  Image *tmp = NULL; // secondary (temporary) image buffer
  char *buf = NULL; // string buffer
  Point start, end; // coordinates (for crop)
  float sigma = 0;
  float amt =0;
  double satLevel = 0;
  double contrastLevel =0;
  while (!done) {
    printMenu(fout); // print the menu
    CommandType cmd; // read a command
    while ( (cmd = getCommand(fin)) == NOOP) {
    }
    switch (cmd) {
      case QUIT:
        done = true;
        break;

      case READ:
        buf = readWord(fin); // try to get a filename
        if (!buf) {
          fprintf(fout, "Error: failed to get filename!\n");
          break;
        }
        fprintf(fout, "Reading from %s...\n", buf);
        if (im) {
          freeImage(&im);
        }
        im = readPPMImage(buf); // read new image
        free(buf); buf = NULL; // free string buffer
        break;

      case WRITE:
        buf = readWord(fin); // try to get a filename
        if (!buf) {
          fprintf(fout, "Error: failed to get filename!\n");
          break;
        }
        fprintf(fout, "Writing to %s...\n", buf);
        writePPMImage(im, buf); // write image
        free(buf); buf = NULL;
        break;

      case CROP:
        // try to get arguments
        if (fscanf(fin, "%d %d %d %d", &(start.x), &(start.y), &(end.x), &(end.y)) != 4) {
	  fprintf(fout, "Error: failed to get 4 integers!\n");
          break;
        }
        fprintf(fout, "Cropping region from (%d, %d) to (%d, %d)...\n", start.x, start.y, end.x, end.y);
        tmp = crop(im, start, end); // crop, store result in secondary buffer
        if (!tmp) {
          fprintf(fout, "Error: cropping failed, image unchanged\n");
        } else {
	  freeImage(&im);
          im = tmp;
          tmp = NULL; 
        }
        break;

      case INVERT:
        fprintf(fout, "Inverting intensity...\n");
        invert(im);
        break;

      case SWAP:
        fprintf(fout, "Swapping color channels...\n");
        swapRGB(im);
        break;

      case GRAYSCALE:
	fprintf(fout, "Implementing grayscale...\n");
        grayscale(im);
        break;

      case BLUR:
	if(!fscanf(fin,"%f",&sigma)){
	  fprintf(stderr,"Error: Invalid input for blur\n");
	  break;
	}
	fprintf(fout, "Blurring Image...\n");
	im=blur(im,sigma);
	break;

      case SHARPEN:
	if(!fscanf(fin,"%f %f",&sigma,&amt)){
	    fprintf(stderr, "Error: Invalid input for sharpen.\n");
	    break;
	}
	fprintf(fout, "Sharpening the image...\n");
	sharpen(im,sigma,amt);
        break;

      case BRIGHTEN:
	if (!fscanf(fin, "%lf", &satLevel)) {
	  fprintf(stderr, "Error: input was not valid\n");
	  break;
	}
	fprintf(fout, "Adjusting brightness\n");
	im=brightness(im,satLevel);
	break;

      case CONTRAST:
        if (!fscanf(fin, "%lf", &satLevel)) {
	  fprintf(stderr, "Error: input was not valid\n");
	  break;
	}
	fprintf(fout, "Adjusting contrast...\n");
	im = contrast(im, satLevel);
	break;

      case UNKNOWN:
      default:
        fprintf(fout, "Unknown command\n");
        break;
    }

  }
  
  fprintf(fout, "Goodbye!\n");

  freeImage(&im); 
 
}
