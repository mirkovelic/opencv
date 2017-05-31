////////////////////////////////////////////////////////////////////////
//
// hello-world.cpp
//
// This is a simple, introductory OpenCV program. The program reads an
// image from a file, inverts it, and displays the result. 
//
////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <netcdf.h>
#define ERRCODE 2
#define ERR(e) {printf("Error in %s at line %d: %s\n", __func__, __LINE__, nc_strerror(e)); exit(ERRCODE);}
#define CHKERR(e) {if(e) ERR(e)}
uint16_t getmaxi(int n, uint16_t *r, int *i){
  uint16_t t=0;
  int ix;
  *i=0;

  for(ix=0;ix<n;ix++){
    if(r[ix] > t){
      t=r[ix];
      *i=ix;
    }
  }
  return t;
}

void filter(int n, uint16_t *r, uint16_t max){
  int i;
  for(i=0;i<n;i++){
    if(r[i] < max) r[i]=0;
  }
  
}

int main(int argc, char *argv[])
{
  IplImage* img = 0; 
  IplImage* img2 ;
  int height,width,step,channels;
  uint16_t *data;
  uint16_t *dataD;
  float *dataA;
  uint16_t max=0,tt;
  int i,j,k;
  int idx;
  int ncid, varidr,varidg,varidb,varid;
  int  dimids[2];
  int err;
  
  if(argc<2){
    printf("Usage: main <image-file-name>\n\7");
    exit(0);
  }


/* 8 bit, color or gray - deprecated, use CV_LOAD_IMAGE_ANYCOLOR */
//#define CV_LOAD_IMAGE_UNCHANGED  -1
/* 8 bit, gray */
//#define CV_LOAD_IMAGE_GRAYSCALE   0
/* 8 bit unless combined with CV_LOAD_IMAGE_ANYDEPTH, color */
//#define CV_LOAD_IMAGE_COLOR       1
/* any depth, if specified on its own gray */
//#define CV_LOAD_IMAGE_ANYDEPTH    2
/* by itself equivalent to CV_LOAD_IMAGE_UNCHANGED
   but can be modified with CV_LOAD_IMAGE_ANYDEPTH */
//#define CV_LOAD_IMAGE_ANYCOLOR    4
     
  // load an image  
  //img=cvLoadImage(argv[1], CV_LOAD_IMAGE_COLOR*0);
  img=cvLoadImage(argv[1],-1);
  
  if(!img){
    printf("Could not load image file: %s\n",argv[1]);
    exit(0);
  }
     
  // get the image data
  height    = img->height;
  width     = img->width;
  step      = img->widthStep;
  channels  = img->nChannels;
  data      = (uint16_t *)img->imageData;
  printf("Processing a %dx%d image with %d channels: step %d\n",height,width,channels,step);
  cvPyrUp(img, img2, CV_GAUSSIAN_5x5 );
  height=height/50;
  width=width/5;
  //height=230;
  //width= 270;
  uint16_t nbytes=step/img->width/channels;
  unsigned char a,b,c[2];

  printf("bytes %u\n",nbytes);  
  
  dataD=malloc(height*width*sizeof(uint16_t));
  dataA=malloc(height*width*sizeof(float));
  for(i=0;i<height;i++){
    for(j=0;j<width;j++){
      //memcpy(&dataD[(height-1-i)*width+j], &data[i*step+j*channels+0], step/(img->width*channels));
      //a=data[i*step+j*channels+0];
      //b=data[i*step+j*channels+1];
      //c[0]=b;
      //c[1]=a;
      step=img->width;
      //memcpy(&dataD[(height-1-i)*width+j], &c, 2);
      dataD[(height-1-i)*width+j] = data[i*step+j*channels+0];
      //memcpy(&dataD[(i)*width+j], &data[i*step+j*channels+0], nbytes);
      //max = data[i*step+j*channels+0];
      dataA[(height-1-i)*width+width-1-j] = dataD[(height-1-i)*width+width-1-j]/3.0;
    }
  }

  max=getmaxi(height*width, dataD, &idx);
  tt=198;
  uint16_t xx;
  xx=45756;
  printf("max=%u tt=%u xx=%u\n",max,tt,xx);
  max=getmaxi(height*width, data, &idx);
  printf("max=%u tt=%u xx=%u\n",max,tt,xx);

  max=0;
  //filter(height*width, dataD, max);

  err = nc_create("test.nc", NC_CLOBBER|NC_NETCDF4, &ncid); CHKERR(err);
  err = nc_def_dim(ncid, "row", height, &dimids[0]);CHKERR(err);
  err = nc_def_dim(ncid, "col", width,  &dimids[1]); CHKERR(err);
  err = nc_def_var(ncid, "red", NC_USHORT, 2, dimids, &varidr); CHKERR(err);
  err = nc_def_var(ncid, "green", NC_USHORT, 2, dimids, &varidg); CHKERR(err);
  err = nc_def_var(ncid, "blue", NC_USHORT, 2, dimids, &varidb); CHKERR(err);
  err = nc_def_var(ncid, "avg", NC_FLOAT, 2, dimids, &varid); CHKERR(err);
  err = nc_enddef(ncid); CHKERR(err);
  err = nc_put_var_ushort(ncid, varidr, dataD); CHKERR(err);
  #if 1
  for(i=0;i<height;i++){
    for(j=0;j<width;j++){
      //dataD[(height-1-i)*width+j] =  (uint16_t)data[i*step+j*channels+1];
      memcpy(&dataD[(height-1-i)*width+j], &data[i*step+j*channels+1], step/(img->width*channels));
      dataA[(height-1-i)*width+j] = dataD[(height-1-i)*width+j]/3.0;
    }
  }
  filter(height*width, dataD, max);
  err = nc_put_var_ushort(ncid, varidg, dataD); CHKERR(err);
  for(i=0;i<height;i++){
    for(j=0;j<width;j++){
      //dataD[(height-1-i)*width+j] =  (uint16_t)data[i*step+j*channels+2];
      memcpy(&dataD[(height-1-i)*width+j], &data[i*step+j*channels+2], step/(img->width*channels));
      dataA[(height-1-i)*width+j] = dataD[(height-1-i)*width+j]/3.0;
    }
  }
  filter(height*width, dataD, max);
  err = nc_put_var_ushort(ncid, varidb, dataD); CHKERR(err);
  err = nc_put_var_float(ncid, varid, dataA); CHKERR(err);
  #endif
  err = nc_close(ncid);
  free(dataD);
  /* // create a window */
  //cvNamedWindow("mainWin", CV_WINDOW_AUTOSIZE);
  //cvMoveWindow("mainWin", 100, 100);
     
  // invert the image
  //for(i=0;i<height;i++) for(j=0;j<width;j++) for(k=0;k<channels;k++)
  //                                            data[i*step+j*channels+k]=255-data[i*step+j*channels+k];
     
  // show the image
  //cvShowImage("mainWin", img );
     
  // wait for a key
  //cvWaitKey(0);
     
  // release the image
  cvReleaseImage(&img );
  return 0;
}

