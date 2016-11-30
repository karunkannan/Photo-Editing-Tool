#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "ppmIO.h"


Image *readPPMImage(char *filename) {
  Image *im = malloc(sizeof(Image));
  if (im) {
    im->data = readPPM(&(im->rows), &(im->cols), &(im->colors), filename);
    if (!im->data) {
      free(im);
      im = NULL;
    }
  }
  return im;
}


Pixel *readPPM(int *rows, int *cols, int * colors, char *filename) {
  FILE *fp = fopen(filename, "r");

  if (fp) {
    Pixel *im = readPPMFile(rows, cols, colors, fp);
    fclose(fp);
    return im;
  }

  fprintf(stderr, "Error:ppmIO - failed to open \"%s\" for reading\n", filename);
  return NULL;
}


int readNum(FILE *fp) {
  assert(fp);

  char ch;
  while((ch = fgetc(fp)) == '#') { // # marks a comment line
    while( ((ch=fgetc(fp)) != '\n') && ch != EOF ) {
    }//removes any comments
  }
  ungetc(ch, fp); //return last char

  int val;
  if (fscanf(fp, "%d", &val) == 1) { 
    while(isspace(ch = fgetc(fp))) {
     
    }
    ungetc(ch, fp);
    return val; 
  } else {
    fprintf(stderr, "Error:ppmIO - failed to read number from file\n");
    return -1;
  }
}


Pixel *readPPMFile(int *rows, int *cols, int * colors, FILE *fp) {
  
  *rows = *cols = *colors = -1;

  
  if (!fp) {
    fprintf(stderr, "Error:ppmIO - given a bad filehandle\n");
    return NULL;
  }

  
  char tag[20];
  tag[19]='\0';
  fscanf(fp, "%19s\n", tag);
  if (strncmp(tag, "P6", 20)) {
    fprintf(stderr, "Error:ppmIO - not a PPM (bad tag)\n");
    return NULL;
  }

  //read dimensions
  *cols = readNum(fp); 
  *rows = readNum(fp);
  *colors = readNum(fp);

  //check dimesnions
  if (*cols > 0 && *rows > 0 && *colors == 255) {
    //allocate space
    Pixel *image = malloc(sizeof(Pixel) * (*rows) * (*cols));
    if (image) { 
      if (fread(image, sizeof(Pixel), (*rows) * (*cols), fp) != (size_t)((*rows) * (*cols))) {
        fprintf(stderr, "Error:ppmIO - failed to read data from file!\n");
        free(image);
        return NULL;
      }
      return image;
    } else {
      fprintf(stderr, "Error:ppmIO - failed to allocate space for image!\n");
      return NULL;
    }
  } else {
    fprintf(stderr, "Error:ppmIO - bad dimensions for image\n");
    return NULL;
  }

  assert(0); // should never get here
  return NULL;
}


int writePPMImage (Image *im, char *filename) {
  if (!filename || !im) {
    fprintf(stderr, "Error:ppmIO - null pointer passed to writePPMImage\n");
    return 0;
  }

  return writePPM(im->data, im->rows, im->cols, im->colors, filename);
}


int writePPM(Pixel *image, int rows, int cols, int colors, char *filename) {

  if (!filename || !image) {
    fprintf(stderr, "Error:ppmIO - null pointer passed to writePPM\n");
    return 0;
  }
  FILE *fp = fopen(filename, "w");

  int written = 0;

  if (fp) {
    written = writePPMFile(image, rows, cols, colors, fp);
    fclose(fp);
  } else {
    fprintf(stderr, "Error:ppmIO - failed to open \"%s\" for writing\n", filename);
  }
  return written;
}


int writePPMFile(Pixel *image, int rows, int cols, int colors, FILE *fp) {
  if (!fp) {
    fprintf(stderr, "Error:ppmIO - writePPM given a bad filehandle\n");
    return 0;
  }

  //tage 
  fprintf(fp, "P6\n");
  //dimensions
  fprintf(fp, "%d %d\n%d\n", cols, rows, colors);
  //pixels
  int written = fwrite(image, sizeof(Pixel), rows * cols, fp);
  if (written != (rows * cols)) {
    fprintf(stderr, "Error:ppmIO - failed to write data to file!\n");
  }

  return written;
}


void freeImage(Image **im) {
  if (*im) {
    free((*im)->data);
  }
  free(*im);
  *im = NULL;
}




Image *makeImage (int rows, int cols) {
  if (rows <= 0 || cols <= 0) { // make sure dimensions are valid
    return NULL;
  }
  // allocate space
  Image *im = malloc(sizeof(Image));
  if (!im || rows <= 0 || cols <= 0) { 
    return NULL;
  }
  // set size 
  im->rows = rows;
  im->cols = cols;
  im->colors = 255;

  // allocate pixel array
  im->data = malloc((im->rows * im->cols) * sizeof(Pixel));
  if (!im->data) {
    free(im);
    return NULL;
  }

  return im;
}



Image *makeCopy (Image *orig) {
  // allocate space
  Image *copy = makeImage(orig->rows, orig->cols);


  if (copy) {
    memcpy(copy->data, orig->data, (copy->rows * copy->cols) * sizeof(Pixel));
  }

  return copy;
}

