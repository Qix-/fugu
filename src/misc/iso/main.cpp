#include <GL/glfw.h>
#include <stdlib.h>
#include "testheader.h"

int main(void){

	int running = GL_TRUE;

	if (!glfwInit()){
		exit(EXIT_FAILURE);
	}

	if (!glfwOpenWindow(0,0, 0,0,0,0,0,0, GLFW_WINDOW)){
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	while(running){
		glClear(GL_COLOR_BUFFER_BIT);
		glRotatef(1,0,0,1);
		glColor3f(1,0,0);
		glBegin(GL_TRIANGLES);
			glVertex2f(0,0);
			glVertex2f(.5,0);
			glVertex2f(.25,.6);
		glEnd();
		glfwSwapBuffers();

		running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
	}

	glfwTerminate();
	exit(EXIT_SUCCESS);
}
