/**
 * \file
 * \brief A simple class for encapsulating PPM images
 * \author ben
  *
 * \cond showlicense
 * \verbatim
 * --------------------------------------------------------------
 *    ___
 *   |  _|___
 *   |  _| . | fg: real-time procedural
 *   |_| |_  | animation and generation
 *       |___| of 3D forms
 *
 *   Copyright (c) 2005 Ben Porter
 *   Use of this software is governed by the terms outlined in
 *   the LICENSE file.
 *
 * --------------------------------------------------------------
 * \endverbatim
 * \endcond
 */


#include "ppm.h"
#include <cstdlib>

Ppm::Ppm(std::string filename)
:mIsValid(false)
,mName(filename)
{
	FILE* fp;
	if ((fp=fopen(filename.c_str(),"r"))==NULL)
	{
		return;
	};
	
	/* VERIFY MAGIC NUMBER */

	char magic_number[2];	
	_DiscardComments(fp);
	fscanf(fp,"%c%c",magic_number,magic_number+1);		

	if (_CheckMN(magic_number)==0)
	{
		fclose(fp);
		return;
	}
	
	/* VERIFIED TO BE A PPM SO MAKE A NEW STRUCT */
	_DiscardComments(fp);

	/* GET DIMENSIONS */

	fscanf(fp,"%d%d",&mWidth,&mHeight);
	_DiscardComments(fp);

	/* GET MAX COLOR VAL */
	
	fscanf(fp,"%d%*c",&mMaxVal);

	mBPC = 2; //byte per component
	if (mMaxVal<256)
	{
		mBPC = 1;
	}

	/* READ IN RASTER INFO */

	mData = malloc(mBPC*3*mWidth*mHeight);		
	fread(mData,mBPC,3*mWidth*mHeight,fp);
	fclose(fp);
	
	/* IS VALID NOW */
	mIsValid = true;	
}

Ppm::~Ppm()
{
	free(mData);
}

int Ppm::_CheckMN(char* mn)
{
	if (!(mn[0]=='P'&&mn[1]=='6'))
	{
		return 0;
	}
	else return 1;
}

void Ppm::_DiscardComments(std::FILE* fp)
{
	while(1)
	{
		char c;
		fscanf(fp,"%c",&c);

		if (c=='#'||c=='\n') //we have a comment
		{
			while(c!='\n'&&c!=0) fscanf(fp,"%c",&c);
		}
		else //not a comment so put the char back in
		{
			ungetc(c,fp);
			break;
		}		
	}
}


GLuint Ppm::GetGLTex()
{
	GLuint texID;

	glGenTextures(1,&texID);
	if (texID==0) return 0;
	glBindTexture(GL_TEXTURE_2D,texID);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,mWidth,mHeight,0,GL_RGB,GL_UNSIGNED_BYTE,mData);
	// glDisable(GL_TEXTURE_2D);

	return texID;
}
	
void Ppm::Save(unsigned char *data, int width, int height, std::string filename)
{
	FILE* fp;
	if ((fp=fopen(filename.c_str(),"w"))==NULL)
	{
		return;
	};
	
	/* INSERT MAGIC NUMBER */
	fprintf(fp,"P6 ");		
	
	/* INSERT DIMENSIONS */
	fprintf(fp,"%d %d ",width,height);

	/* INSERT MAX COLOR VAL */	
	fprintf(fp,"%d\n",255);

	/* PRINT OUT RASTER INFO */
	fwrite(data,1,3*width*height,fp);
	fclose(fp);
}
