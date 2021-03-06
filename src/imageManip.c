#include <stdlib.h>
#include "imageManip.h"
#include "math.h"

Image* crop(Image* orig, Point start, Point end) {
  // Check pointer to make sure we have an image
  if (!orig || !orig->data) {
    fprintf(stderr, "Error:imageManip - crop given a bad image pointer\n");
    return NULL;
  }
  // Check boundaries, to make sure they're valid
  if (start.x < 0 || start.x >= orig->cols-1 || 
      start.y < 0 || start.y >= orig->rows-1 ||
      end.x < 0 || end.x >= orig->cols-1 || 
      end.y < 0 || end.y >= orig->rows-1 ||
      end.x < start.x || end.y < start.y) {
    fprintf(stderr, "Error:imageManip - crop given a bad range\n");
    return NULL;
  }


  // allocate new image
  Image *new = malloc(sizeof(Image));
  if (!new) {
    return NULL;
  }

  // calculate size of new image
  int rowSpan = end.y - start.y + 1;
  int colSpan = end.x - start.x + 1;
  new->rows = rowSpan;
  new->cols = colSpan;
  new->colors = orig->colors;

  // allocate new pixel array
  new->data = malloc((new->rows * new->cols) * sizeof(Pixel));
  if (!new->data) {
    free(new);
    return NULL;
  }

  // copy from old image to new image
  for (int r=0; r<rowSpan+1; r++) {
    for (int c=0; c<colSpan+1; c++) {
      new->data[(r*colSpan)+c] = orig->data[ ((r+start.y)*(orig->cols)) + (c+start.x) ];
    }
  }

  return new;
}




/* invert the intensity of each channel */
void invert (Image* im) {
  if (!im || !im->data) {
    fprintf(stderr, "Error:imageManip - invert given a bad image pointer\n");
    return;
  }

  for (int r=0; r<im->rows; r++){
    for (int c=0; c<im->cols; c++){
      im->data[(r*im->cols)+c].r = 255 - im->data[(r*im->cols)+c].r;
      im->data[(r*im->cols)+c].g = 255 - im->data[(r*im->cols)+c].g;
      im->data[(r*im->cols)+c].b = 255 - im->data[(r*im->cols)+c].b;
    }
  }
}


/* swap color channels of an image */
void swapRGB (Image* im) {
  if (!im || !im->data) {
    fprintf(stderr, "Error:imageManip - swapRGB given a bad image pointer\n");
    return;
  }

  for (int r=0; r<im->rows; r++){
    for (int c=0; c<im->cols; c++){
      unsigned char tmp = im->data[(r*im->cols)+c].r;
      im->data[(r*im->cols)+c].r = im->data[(r*im->cols)+c].g;
      im->data[(r*im->cols)+c].g = im->data[(r*im->cols)+c].b;
      im->data[(r*im->cols)+c].b = tmp;
    }
  }
}

void grayscale(Image* im){
  if(!im || !im->data){
    fprintf(stderr, "Error:imageManip - grayscale given a bad image pointer\n");
    return;
  }

  for(int i=0;i<im->rows;i++){
    for(int j=0;j<im->cols;j++){
      float red = 0.30*im->data[(i*im->cols)+j].r;
      float green = 0.59*im->data[(i*im->cols)+j].g;
      float blue = 0.11*im->data[(i*im->cols)+j].b;
      float val = red + green + blue;
      im->data[(i*im->cols)+j].r = (unsigned char)val;
      im->data[(i*im->cols)+j].b = (unsigned char)val;
      im->data[(i*im->cols)+j].g = (unsigned char)val;
    }
  }
}

Image* blur(Image* im, float sigma){
  if(!im || !im->data){
    printf(stderr, "Error:imageManip - blur was given a bad image pointer\n");
    return im;
  }
  double * matrix=buildMatrix(sigma); 
  Image* blurIm = malloc(sizeof(Image));
  blurIm->data = malloc(im->cols*im->rows*sizeof(Pixel));
  blurIm->cols = im->cols;
  blurIm->rows = im->rows;
  blurIm->colors = im->colors; 
  for(int i=0;i<im->rows;i++){
    for(int j=0;j<im->cols;j++){
      blurPixel(im, blurIm, matrix, i, j,sigma);
    }
  }
  free(im->data);
  free(im);
  free(matrix);
  return blurIm;
}

double* buildMatrix(float sigma){
  double* matrix = NULL;
  int dimension = 2*floor(5*sigma) + 1;
  matrix = malloc(dimension*dimension*sizeof(double));
  int center = floor(dimension/2);
  for(int i = 0; i<dimension; i++){
    for(int j = 0; j<dimension; j++){
      double g = (1.0 / (2.0 * PI * sq(sigma))) * exp( -(sq(center-i) + sq(center-j)) / (2 * sq(sigma)));
      matrix[(i*dimension)+j] = g;
      
    }
  }
  return matrix;
}

void blurPixel(Image* im,Image* blurIm, double* matrix,int i, int j, float sigma){
  //define variables
  int dimension = 2*floor(5*sigma) +1;
  int center = floor(dimension/2);
  int startX = j - center;
  int maxX = j+center;
  int startY = i - center;
  int maxY = i+center;
  //determine start and end bounds in case spills over
  if(startX < 0){
    startX = 0;
  }
  if(maxX > im->cols){
    maxX = im->cols;
  }
  if(startY < 0){
    startY = 0;
  }
  if(maxY > im->rows){
    maxY = im->rows;
  }
  //declare sums
  double greenSum = 0.0;
  double redSum = 0.0;
  double blueSum = 0.0;
  for(int p=startY;p<maxY;p++){
    for(int q=startX;q<maxX;q++){
      greenSum += im->data[(p*im->cols)+q].g*matrix[((p-startY)*dimension)+(q-startX)];
      redSum += im->data[(p*im->cols)+q].r*matrix[((p-startY)*dimension)+(q-startX)];
      blueSum += im->data[(p*im->cols)+q].b*matrix[((p-startY)*dimension)+(q-startX)];
    }
  }
  //remove shadow by adding center point for edge cases
  int numPixels = 1;// (maxX - startX) * (maxY - startY);
  double green = greenSum/numPixels;
  double red = redSum/numPixels;
  double blue = blueSum/numPixels;
  blurIm->data[(i*blurIm->cols)+j].g = (unsigned char)green;
  blurIm->data[(i*blurIm->cols)+j].r = (unsigned char)red;
  blurIm->data[(i*blurIm->cols)+j].b = (unsigned char)blue;
}

void sharpen(Image* im, float sigma, float amt){
  if(!im || !im->data){
    printf(stderr, "Error:imageManip - blur was given a bad image pointer\n");
    return;
  }
  double * matrix=buildMatrix(sigma);
  Image* tempIm = malloc(sizeof(Image));
  tempIm->data = malloc(im->cols*im->rows*sizeof(Pixel));
  tempIm->cols = im->cols;
  tempIm->rows = im->rows;
  tempIm->colors = im->colors;
  for(int i=0;i<im->rows;i++){
    for(int j=0;j<im->cols;j++){
      blurPixel(im, tempIm, matrix,i,j,sigma);
      int red = (int)im->data[(i*im->cols)+j].r - (int)tempIm->data[(i*tempIm->cols)+j].r;
      int green = (int)im->data[(i*im->cols)+j].g - (int)tempIm->data[(i*tempIm->cols)+j].g;
      int blue = (int)im->data[(i*im->cols)+j].b - (int)tempIm->data[(i*tempIm->cols)+j].b;
      red = red*amt;
      green = green*amt;
      blue = blue*amt;
      im->data[(i*im->cols)+j].r = clamp(im->data[(i*im->cols)+j].r+red);
      im->data[(i*im->cols)+j].g = clamp(im->data[(i*im->cols)+j].g+green);
      im->data[(i*im->cols)+j].b = clamp(im->data[(i*im->cols)+j].b+blue);
    }
  }
  free(tempIm->data);
  free(tempIm);
  free(matrix);
}

unsigned char clamp(double colorValue) {
  if (colorValue < 0) {
    colorValue = 0;
  } else if(colorValue > 255 ) {
    colorValue = 255;
  }
  unsigned char newVal = (unsigned char) colorValue;
  return newVal;
}
  
Image* brightness(Image* im, double satLevel) {
  if (!im || !im->data) {
    fprintf(stderr, "Error:imageManip - swapRGB given a bad image pointer\n");
    return;
   }

  for (int r = 0; r < im->rows; r++) {
    for (int c = 0; c < im -> cols; c++) {
      double rtemp = (double) im->data[(r*im->cols)+c].r;
      rtemp = rtemp + satLevel;

      double gtemp = (double) im->data[(r*im->cols)+c].g;
      gtemp = gtemp + satLevel;
      // for blue
      double btemp = (double) im->data[(r*im->cols)+c].b;
      btemp = btemp + satLevel;

      im->data[(r*im->cols)+c].r = clamp(rtemp);
      im->data[(r*im->cols)+c].g = clamp(gtemp);
      im->data[(r*im->cols)+c].b = clamp(btemp);
      }
    }
  return im;
}
Image* contrast(Image* im, double contrastLevel) {
  double increment = 1.0/255.0;
  for (int r = 0; r < im->rows; r++) {
    for (int c = 0; c < im -> cols; c++) {
      // compute contrast for red
      double rtemp = (double) (im->data[(r*im->cols)+c].r);
      rtemp = ((rtemp*increment) - 0.5) * contrastLevel;
      rtemp = (rtemp + 0.5)/(increment);
      // for green
      double gtemp = (double) im->data[(r*im->cols)+c].g;
      gtemp = ((gtemp*increment) - 0.5) * contrastLevel;
      gtemp = (gtemp + 0.5)/(increment);
      // for blue 
      double btemp = (double) im->data[(r*im->cols)+c].b;
      btemp = ((btemp*increment) - 0.5) * contrastLevel;
      btemp = (btemp + 0.5)/(increment);
      
      im->data[(r*im->cols)+c].r = clamp(rtemp);
      im->data[(r*im->cols)+c].g = clamp(gtemp);
      im->data[(r*im->cols)+c].b = clamp(btemp);
    }
   }
 return im;
}
