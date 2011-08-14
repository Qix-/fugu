/* A simple class for encapsulating PPM images 
 * BP 12.05.05
 */

#ifndef PPM_H
#define PPM_H

#include <string>
#include <cstdio>

#include "GL/glew.h"

class Ppm
{
	public:

	Ppm(std::string filename);
	~Ppm();

	bool IsValid(){return mIsValid;}
	int GetWidth(){return mWidth;}
	int GetHeight(){return mHeight;}
	
	GLuint GetGLTex(); //binds the image into a GL texture object

	// Save: Outputs a PPM specified by the parameters (assumes 1 byte per colour component, 3 colours per pixel)
	static void Save(unsigned char *data, int width, int height, std::string filename);
	
	protected:		

	bool mIsValid; //is this image valid? (i.e. loaded)

	std::string mName;	
	int mWidth, mHeight;
	int mMaxVal;
	int mBPC; //bytes per color component
	void* mData;


	/* Helper functions */
	protected:
	int _CheckMN(char* mn); //check the magic number
	void _DiscardComments(std::FILE* fp); //ignore comments
};

#endif
