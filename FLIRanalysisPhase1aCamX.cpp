
#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <math.h>
#include <sstream>

#define CROP_YI 15
#define CROP_YF 1275
#define CROP_XI 10
#define CROP_XF 580

using namespace std;

int Ret[2] = { 505, 85 };
int diffs[3] = { 0, 0, 0 };
int width, height;

unsigned char* readBMP(char* filename)
{
  int i;
  FILE* f = fopen(filename, "rb");
  unsigned char info[54];

  // read the 54-byte header
  fread(info, sizeof(unsigned char), 54, f);

  // extract image height and width from header
  width = *(int*)&info[18];
  height = *(int*)&info[22];
  //cerr << width << " " << height << endl;

  // allocate three bytes per pixel
  int size = 3 * width * height;
  unsigned char* data = new unsigned char[size];

  // read the rest of the data at once
  fread(data, sizeof(unsigned char), size, f);
  fclose(f);

  for(i = 0; i < size; i += 3)
    {
      // flip the order of every 3 bytes
      unsigned char tmp = data[i];
      data[i] = data[i+2]; data[i+2] = tmp;
    }

  return data;

}

int main ( int argc, char** argv ) {
  
  unsigned long start = (unsigned long)atof(argv[1]); unsigned long end = start + (unsigned long)atof(argv[2]); bool CC = atoi(argv[3]);
  
  for ( unsigned long k = start; k < end; ++k ) {
    
    char* fileNameNew;
    stringstream temp; //stringstream().swap(temp);
    if ( k < 9 ) temp << "bitmaps/thumb00000" << (k+1) << ".bmp";
    else if ( k < 99 ) temp << "bitmaps/thumb0000" << (k+1) << ".bmp";
    else if ( k < 999 ) temp << "bitmaps/thumb000" << (k+1) << ".bmp";
    else if ( k < 9999 ) temp << "bitmaps/thumb00" << (k+1) << ".bmp";
    else if ( k < 99999 ) temp << "bitmaps/thumb0" << (k+1) << ".bmp";
    else temp << "bitmaps/thumb" << (k+1) << ".bmp";
    string name; temp >> name;
    fileNameNew = &name[0];
    unsigned char* dataNew = readBMP ( fileNameNew );
    unsigned char* dataOld;
    
    if ( k > start ) {
       char* fileNameOld;
      stringstream temp2; //stringstream().swap(temp);
      if ( k < 10 ) temp2 << "bitmaps/thumb00000" << k << ".bmp";
      else if ( k < 100 ) temp2 << "bitmaps/thumb0000" << k << ".bmp";
      else if ( k < 1000 ) temp2 << "bitmaps/thumb000" << k << ".bmp";
      else if ( k < 10000 ) temp2 << "bitmaps/thumb00" << k << ".bmp";
      else if ( k < 100000 ) temp2 << "bitmaps/thumb0" << k << ".bmp";
      else temp2 << "bitmaps/thumb" << k << ".bmp";
      name.clear(); temp2 >> name;
      fileNameOld = &name[0];
      dataOld = readBMP ( fileNameOld );
    }
    
    int maximum[3] = {-1,-1,-1}; int minimum[3] = {256,256,256}; int maxLoc[3][2] = {0}; int minLoc[3][2] = {0}; int NumPixAbvThr[3][2] = {0}; int NumPixAbvSubThrSum = 0, CloudCover = 100;
    
    for ( int i = (height-1-CROP_XI); i >= (height-CROP_XF); --i ) {
      for ( int j = CROP_YI; j < CROP_YF; ++j ) {
	
	int rgbColorNew[3], rgbColorOld[3]; unsigned short MinThr = 40, SubThr = 20;
	
	if ( k == start ) {
	  rgbColorOld[0] = 0;
          rgbColorOld[1] = 0;
          rgbColorOld[2] = 0;
        }
	else {
	  rgbColorOld[0] = (int)dataOld[3 * (i * width + j) + 0];
	  rgbColorOld[1] = (int)dataOld[3 * (i * width + j) + 1];
	  rgbColorOld[2] = (int)dataOld[3 * (i * width + j) + 2];
	}
	
	rgbColorNew[0] = (int)dataNew[3 * (i * width + j) + 0];
        rgbColorNew[1] = (int)dataNew[3 * (i * width + j) + 1];
        rgbColorNew[2] = (int)dataNew[3 * (i * width + j) + 2];

	diffs[0] = rgbColorNew[0]-rgbColorOld[0]; diffs[1] = rgbColorNew[1]-rgbColorOld[1]; diffs[2] = rgbColorNew[2]-rgbColorOld[2];
	int ii = height - 1 - i; int jj = j;
	
	bool ExclusionZone = false;
	
	//if ( (abs(jj-650) < 340 && abs(ii-518) < 73) || abs(jj-1123) < 48 || (abs(jj-323) < 278 && abs(ii-38) < 37) || ( jj > 1025 && ii > 565 ) || (abs(jj-1058) < 163 && abs(ii-50) < 50) ||
	//   ((jj-649)*(jj-649)+(ii-362)*(ii-362)) < 6400 || (abs(jj-1123) < 98 && abs(ii-143) < 33) ||
	//   ((jj-1175)*(jj-1175)+(ii-370)*(ii-370)) < 2025 || ((jj-885)*(jj-885)+(ii-205)*(ii-205)) < 900 || ((jj-408)*(jj-408)+(ii-205)*(ii-205)) < 529 )
	if ( abs(jj-1123) < 98 || (abs(jj-650) < 340 && abs(ii-Ret[0]) < Ret[1]) || (abs(jj-323) < 278 && abs(ii-38) < 37) || ( jj > 1025 && ii > 565 ) || (abs(jj-1058) < 163 && abs(ii-50) < 50) ||
	     ((jj-649)*(jj-649)+(ii-362)*(ii-362)) < 6400 || ((jj-885)*(jj-885)+(ii-205)*(ii-205)) < 900 || ((jj-408)*(jj-408)+(ii-205)*(ii-205)) < 529 )
	  ExclusionZone = true;
	
	if ( !ExclusionZone || k == start ) {
	  if ( diffs[0] > maximum[0] )
	    { maximum[0] = diffs[0]; maxLoc[0][0] = ii; maxLoc[0][1] = jj; if ( maximum[0] > MinThr ) ++NumPixAbvThr[0][0]; if ( maximum[0] > SubThr ) ++NumPixAbvSubThrSum; }
	  if ( diffs[1] > maximum[1] )
	    { maximum[1] = diffs[1]; maxLoc[1][0] = ii; maxLoc[1][1] = jj; if ( maximum[1] > MinThr ) ++NumPixAbvThr[1][0]; if ( maximum[1] > SubThr ) ++NumPixAbvSubThrSum; }
	  if ( diffs[2] > maximum[2] )
	    { maximum[2] = diffs[2]; maxLoc[2][0] = ii; maxLoc[2][1] = jj; if ( maximum[2] > MinThr ) ++NumPixAbvThr[2][0]; if ( maximum[2] > SubThr ) ++NumPixAbvSubThrSum; }
	  if ( diffs[0] < minimum[0] )
	    { minimum[0] = diffs[0]; minLoc[0][0] = ii; minLoc[0][1] = jj; if ( minimum[0] <-MinThr ) ++NumPixAbvThr[0][1]; if ( minimum[0] <-SubThr ) ++NumPixAbvSubThrSum; }
	  if ( diffs[1] < minimum[1] )
	    { minimum[1] = diffs[1]; minLoc[1][0] = ii; minLoc[1][1] = jj; if ( minimum[1] <-MinThr ) ++NumPixAbvThr[1][1]; if ( minimum[1] <-SubThr ) ++NumPixAbvSubThrSum; }
	  if ( diffs[2] < minimum[2] )
	    { minimum[2] = diffs[2]; minLoc[2][0] = ii; minLoc[2][1] = jj; if ( minimum[2] <-MinThr ) ++NumPixAbvThr[2][1]; if ( minimum[2] <-SubThr ) ++NumPixAbvSubThrSum; }
	}
	
      }
    }
    
    if ( k > start )
      fprintf(stdout,"%lu\t\t%i  %i %i\t%i  %i %i\t%i  %i %i\t\t%i  %i %i\t%i  %i %i\t%i  %i %i\t\t%i %i %i\t%i %i %i\t\t%d\n",k,
	     maximum[0],maxLoc[0][1],maxLoc[0][0],maximum[1],maxLoc[1][1],maxLoc[1][0],maximum[2],maxLoc[2][1],maxLoc[2][0],
	     minimum[0],minLoc[0][1],minLoc[0][0],minimum[1],minLoc[1][1],minLoc[1][0],minimum[2],minLoc[2][1],minLoc[2][0],
	     NumPixAbvThr[0][0],NumPixAbvThr[1][0],NumPixAbvThr[2][0],
	     NumPixAbvThr[0][1],NumPixAbvThr[1][1],NumPixAbvThr[2][1],NumPixAbvSubThrSum);
    else {
      /*fprintf(stderr,
	      "The absolute (no subt) details from the first frame:\n%lu\t\t%i  %i %i\t%i  %i %i\t%i  %i %i\t\t%i  %i %i\t%i  %i %i\t%i  %i %i\t\t%i %i %i\t%i %i %i\t\t%d\n",k,
             maximum[0],maxLoc[0][1],maxLoc[0][0],maximum[1],maxLoc[1][1],maxLoc[1][0],maximum[2],maxLoc[2][1],maxLoc[2][0],
             minimum[0],minLoc[0][1],minLoc[0][0],minimum[1],minLoc[1][1],minLoc[1][0],minimum[2],minLoc[2][1],minLoc[2][0],
             NumPixAbvThr[0][0],NumPixAbvThr[1][0],NumPixAbvThr[2][0],
             NumPixAbvThr[0][1],NumPixAbvThr[1][1],NumPixAbvThr[2][1],NumPixAbvSubThrSum);*/
      cerr << NumPixAbvThr[0][0]+NumPixAbvThr[1][0]+NumPixAbvThr[2][0] << endl;
      if ( (NumPixAbvThr[0][0]+NumPixAbvThr[1][0]+NumPixAbvThr[2][0]) > CloudCover || CC ) { Ret[0] = 360; Ret[1] = 230; }
    }
    
  }
  
  return 0;
  
}
