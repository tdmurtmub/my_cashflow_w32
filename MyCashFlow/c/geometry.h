/***********************************************************************
Copyright (C) 2000-2006 by Wesley Steiner. All rights reserved.
***********************************************************************/

#ifndef _MODULE_GEOMETRY_
#define _MODULE_GEOMETRY_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sSize
	{
    int mWidth;
    int mHeight;
	} tSize;

typedef struct sPoint
	{
	int mX;
	int mY;
	} tPoint;

typedef struct sRect 
	{
    int mLeft; 
    int mTop; 
    int mRight; 
    int mBottom; 
	} tRect; 
 
#ifdef __cplusplus
}
#endif

#endif
