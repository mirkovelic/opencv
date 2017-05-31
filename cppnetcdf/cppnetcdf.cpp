//version 2.9
//http://docs.opencv.org/2.4.9/doc/tutorials/imgproc/imgtrans/sobel_derivatives/sobel_derivatives.html?highlight=sobel
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <netcdf.h>

using namespace cv;
#define ERRCODE 2
#define ERR(e) {printf("Error in %s at line %d: %s\n", __func__, __LINE__, nc_strerror(e)); exit(ERRCODE);}
#define CHKERR(e) {if(e) ERR(e)}

/** @function main */
int main( int argc, char** argv )
{

  Mat img;
  Mat grad;
  char window_name[] = "Sobel Demo - Simple Edge Detector";
  int scale = 1;
  int delta = 0;
  int ddepth = CV_16S;
  int i,j,k;
  int idx;
  int ncid, varidr,varidg,varidb,varid;
  int height,width,step,channels;
  int  dimids[2];
  int err;
  int c;
  char *filename;
  uint16_t *data;
  
  /// Load an image
  if(argc<2){
    printf("Usage: main <image-file-name> [<output netcdf file>]\n\7");
    exit(0);
  }

  printf("%d\n",argc);
  img = imread( argv[1] , CV_LOAD_IMAGE_ANYDEPTH);
  if(argc==3)
    filename = argv[2];

  
  //if( !img.data )
  //{ return -1; }


  //exit(0);

   // get the image data
  //height    = img->height;
  //width     = img->width;
  //step      = img->widthStep;
  //channels  = img->nChannels;
  //data      = (uint16_t *)img->imageData;
  //http://docs.opencv.org/2.4/modules/core/doc/basic_structures.html for step[]
  for(i=0;i<2;i++){
    printf("step %lu\n",img.step[i]);
  }
  
  height=img.rows;
  width=img.cols;
  printf("Processing a %dx%d image with %d channels: step %lu\n",height,width,1,img.step[0]);
  if(argc==2){
    err = nc_create("test.nc", NC_CLOBBER|NC_NETCDF4, &ncid); CHKERR(err);
  }

  if(argc==3){
    err = nc_create(filename, NC_CLOBBER|NC_NETCDF4, &ncid); CHKERR(err);
  }
  
  err = nc_def_dim(ncid, "row", height, &dimids[0]);CHKERR(err);
  err = nc_def_dim(ncid, "col", width,  &dimids[1]); CHKERR(err);
  err = nc_def_var(ncid, "red", NC_USHORT, 2, dimids, &varidr); CHKERR(err);
  err = nc_def_var(ncid, "green", NC_USHORT, 2, dimids, &varidg); CHKERR(err);
  err = nc_def_var(ncid, "blue", NC_USHORT, 2, dimids, &varidb); CHKERR(err);
  err = nc_def_var(ncid, "avg", NC_FLOAT, 2, dimids, &varid); CHKERR(err);
  err = nc_enddef(ncid); CHKERR(err);

  err = nc_put_var_ushort(ncid, varidr, (unsigned short *)img.data); CHKERR(err);
  //imshow( window_name, grad );

  //waitKey(0);

  return 0;
  }

