//To compile:
//g++ -framework SDL2 -framework OpenGL RenderToTexture.cpp glad/glad.c


#include "glad/glad.h"  //Include order can matter here
#ifdef __APPLE__
 #include <SDL2/SDL.h>
 #include <SDL2/SDL_opengl.h>
#else
 #include <SDL.h>
 #include <SDL_opengl.h>
#endif
#include <cstdio>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

bool saveOutput = false;
float timePast = 0;

//SJG: Store the object coordinates
//You should have a representation for the state of each object
float objx=1.5, objy=0, objz=0;

int screenWidth = 800;  //500
int screenHeight = 600;  //375


bool DEBUG_ON = false;
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName);
bool fullscreen = false;
void Win2PPM(int width, int height);

int main(int argc, char *argv[]){
  SDL_Init(SDL_INIT_VIDEO);  //Initialize Graphics (for OpenGL)
    
  //Ask SDL to get a recent version of OpenGL (3.2 or greater)
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	
	//Create a window (offsetx, offsety, width, height, flags)
	SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 100, 100, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
	

	//Create a context to draw in
	SDL_GLContext context = SDL_GL_CreateContext(window);
	
	//Load OpenGL extentions with GLAD
	if (gladLoadGLLoader(SDL_GL_GetProcAddress)){
		printf("\nOpenGL loaded\n");
		printf("Vendor:   %s\n", glGetString(GL_VENDOR));
		printf("Renderer: %s\n", glGetString(GL_RENDERER));
		printf("Version:  %s\n\n", glGetString(GL_VERSION));
	}
	else {
		printf("ERROR: Failed to initialize OpenGL context.\n");
		return -1;
	}
	

	//Full screen quad (for render to texture)
	float quad[24] = {1,1,0,1, -1,1,1,1, -1,-1,1,0,  1,-1,0,0, 1,1,0,1, -1,-1,1,0,};

	const int numModels = 4;
	int mStart[numModels], mEnd[numModels];
	ifstream modelFile;
	int numLines;
	float* model0, *model1, *model2, *model3;
		
	//Load Model 0
	modelFile.open("models/teapot.txt");
	numLines = 0;
	modelFile >> numLines;
	model0 = new float[numLines];
	for (int i = 0; i < numLines; i++){
		modelFile >> model0[i];
	}
	printf("Mode line count: %d\n",numLines);
	mStart[0] = 0;
	mEnd[0] = numLines/8;
	modelFile.close();
	
	//Load Model 1
	modelFile.open("models/knot.txt");
	numLines = 0;
	modelFile >> numLines;
	model1 = new float[numLines];
	for (int i = 0; i < numLines; i++){
		modelFile >> model1[i];
	}
	printf("Mode line count: %d\n",numLines);
	mStart[1] = mEnd[0];
	mEnd[1] = mStart[1] + numLines/8;
	modelFile.close();
	
	//Load Model 2
	modelFile.open("models/cube.txt");
	numLines = 0;
	modelFile >> numLines;
	model2 = new float[numLines];
	for (int i = 0; i < numLines; i++){
		modelFile >> model2[i];
	}
	printf("Mode line count: %d\n",numLines);
	mStart[2] = mEnd[1];
	mEnd[2] = mStart[2] + numLines/8;
	modelFile.close();
	
	//Load Model 3
	modelFile.open("models/sphere.txt");
	numLines = 0;
	modelFile >> numLines;
	model3 = new float[numLines];
	for (int i = 0; i < numLines; i++){
		modelFile >> model3[i];
	}
	printf("Mode line count: %d\n",numLines);
	mStart[3] = mEnd[2];
	mEnd[3] = mStart[3] + numLines/8;
	modelFile.close();
	
	
	//Concatenate model arrays
	float* modelData = new float[mEnd[numModels-1]*8];
	copy(model0, model0+mEnd[0]*8, modelData);
	copy(model1, model1+(mEnd[1]-mStart[1])*8, modelData+mEnd[0]*8);
	copy(model2, model2+(mEnd[2]-mStart[2])*8, modelData+mEnd[1]*8);
	copy(model3, model3+(mEnd[3]-mStart[3])*8, modelData+mEnd[2]*8);

	
	
	SDL_Surface* surface = SDL_LoadBMP("wood.bmp");
	if (surface==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    
    //Load the texture into memory
    glActiveTexture(GL_TEXTURE0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w,surface->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface->pixels);
    
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    SDL_FreeSurface(surface);
    
	
		glEnable(GL_TEXTURE_2D);
	//Store the dim and bright objects in different textures

  GLuint dimText;  //Dimly lit objects
	glGenTextures(1, &dimText);
	glBindTexture(GL_TEXTURE_2D, dimText);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	
	GLuint brightText;  //Brightly lit objects
	glGenTextures(1, &brightText);
	glBindTexture(GL_TEXTURE_2D, brightText);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	
	//Create an FBO for each render pass

	//FBO and Depth buffer for Dim objects
  GLuint fboDim;
	glGenFramebuffers(1, &fboDim);
	glBindFramebuffer(GL_FRAMEBUFFER, fboDim);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dimText, 0);
	
	GLuint depth_dim;
	glGenRenderbuffers(1, &depth_dim);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_dim);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, screenWidth, screenHeight);
	//Attach depth buffer to fbo
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_dim);
	
	//FBO and Depth buffer for Bright objects
	GLuint fboBright;
	glGenFramebuffers(1, &fboBright);
	glBindFramebuffer(GL_FRAMEBUFFER, fboBright);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brightText, 0);
	
	GLuint depth_rb2;
	glGenRenderbuffers(1, &depth_rb2);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_rb2);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, screenWidth, screenHeight);
	//Attach depth buffer to fbo
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rb2);
     
	
	//Unbind our FBOs
	glBindFramebuffer(GL_FRAMEBUFFER,0); //Return to normal rendering


  //*************
	//Create Shaders
	//*************
	int quadShader = InitShader("v1.glsl", "f1.glsl"); 
	glUseProgram(quadShader);
	GLint uniMethod = glGetUniformLocation(quadShader, "method");
	
	int modelShader = InitShader("vertex.glsl", "fragment.glsl");
	glUseProgram(modelShader);
	GLint uniModel = glGetUniformLocation(modelShader, "model");
	GLint uniView = glGetUniformLocation(modelShader, "view");
	GLint uniProj = glGetUniformLocation(modelShader, "proj");
	GLint uniColor = glGetUniformLocation(modelShader, "inColor");
	GLint uniMinLum = glGetUniformLocation(modelShader, "minLum");
	GLint uniMaxLum = glGetUniformLocation(modelShader, "maxLum");

	//*************
	//Create VAO
	//*************

	//Build a Vertex Array Object. This stores the attribute mappings from the model shader to the model VBOs
	GLuint modelVAO;
	glGenVertexArrays(1, &modelVAO); //Create a VAO
	glBindVertexArray(modelVAO); //Bind the above created VAO to the current context

		//Allocate memory on the graphics card to store geometry (vertex buffer object)
	GLuint vbo[1];
	glGenBuffers(1, vbo);  //Create 1 buffer called vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //Set the vbo as the active array buffer (Only one buffer can be active at a time)

	glBufferData(GL_ARRAY_BUFFER, mEnd[numModels-1]*8*sizeof(float), modelData, GL_DYNAMIC_DRAW); //upload vertices to vbo

	//Tell OpenGL how to set model rendering vertex shader input 
	GLint posAttrib = glGetAttribLocation(modelShader, "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0); 
	glEnableVertexAttribArray(posAttrib);
	
	GLint normAttrib = glGetAttribLocation(modelShader, "inNormal");
	glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(5*sizeof(float)));
	glEnableVertexAttribArray(normAttrib);
	
	GLint texAttrib = glGetAttribLocation(modelShader, "inTexcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));

	

	//Build a Vertex Array Object. This stores the attribute mappings from shaders to VBOs
	GLuint quadVAO;
	glGenVertexArrays(1, &quadVAO); //Create a VAO
	glBindVertexArray(quadVAO); //Bind the above created VAO to the current context

	GLuint quadVbo[1];
	glGenBuffers(1, quadVbo);  //Create 1 buffer called vbo
	glBindBuffer(GL_ARRAY_BUFFER, quadVbo[0]); //Set the vbo as the active array buffer (Only one buffer can be active at a time)
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW); //upload vertices to vbo

	//Tell OpenGL how to set vertex shader input 
	GLint quadPosAttrib = glGetAttribLocation(quadShader, "position");
	glVertexAttribPointer(quadPosAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
	glEnableVertexAttribArray(quadPosAttrib);
	
	GLint quadTexAttrib = glGetAttribLocation(quadShader, "texcoord");
	glVertexAttribPointer(quadTexAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(quadTexAttrib);
	
	
	glEnable(GL_DEPTH_TEST); 
	
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK); 
	
	//Event Loop (Loop forever processing each event as fast as possible)
	SDL_Event windowEvent;
	
	float eyeSep = .025;
	int method = 0;
	bool offAxis = false;
	
	float lastTime = SDL_GetTicks()/1000.f;
	float newTime = SDL_GetTicks()/1000.f;
	bool quit = false;
	while (!quit){
		while (SDL_PollEvent(&windowEvent)){
			if (windowEvent.type == SDL_QUIT) quit = true; //Exit event loop
			//List of keycodes: https://wiki.libsdl.org/SDL_Keycode - You can catch many special keys
			//Scancode referes to a keyboard position, keycode referes to the letter (e.g., EU keyboards)
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE) 
			quit = true; ; //Exit event loop
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_f){ //If "f" is pressed
				fullscreen = !fullscreen;
				SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0); //Toggle fullscreen 
			}

			//SJG: Use key input to change the state of the object
			//     We can use the ".mod" flag to see if modifiers such as shift are pressed
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_UP){ //If "up key" is pressed
				if (windowEvent.key.keysym.mod & KMOD_SHIFT) objz += .1; //Is shift pressed?
				else objx -= .1;
			}
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_DOWN){ //If "down key" is pressed
				if (windowEvent.key.keysym.mod & KMOD_SHIFT) objz -= .1; //Is shift pressed?
				else objx += .1;
			}
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_LEFT){ //If "up key" is pressed
				objy += .1;
			}
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_RIGHT){ //If "down key" is pressed
				objy -= .1;
			}
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_q){ //If "up key" is pressed
				eyeSep += .005;
				printf("%f\n",eyeSep);
			}
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_w){ //If "down key" is pressed
				eyeSep -= .005;
				printf("%f\n",eyeSep);
			}
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_m){ 
				method += 1;
				method %= 3;
				printf("Method: %d\n",method);
			}
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_o){ 
				offAxis = !offAxis;
				printf("Projection: %s\n",offAxis?"Off Axis":"Toe In");
			}
      }
      
      if (!saveOutput) timePast = SDL_GetTicks()/1000.f; 
      if (saveOutput) timePast += .07; //Fix framerate at 14 FPS
      
      newTime = SDL_GetTicks()/1000.f;
      //printf("%f FPS\n",1/(newTime-lastTime));
      lastTime = SDL_GetTicks()/1000.f;
      
			
			glUseProgram(modelShader); //Use the normal model shader (phong) for each eye
      for (int pass = 0; pass < 2; pass++){
				if (pass == 0){
					 glUniform1f(uniMinLum, 0);
					 glUniform1f(uniMaxLum, 10);
					 glBindFramebuffer(GL_FRAMEBUFFER, fboDim);
					 glClearColor(.4f, .5f, 0.8f, 1.0f);
				}
        else{
					 glUniform1f(uniMinLum, 1);
					 glUniform1f(uniMaxLum, 100);
					 glBindFramebuffer(GL_FRAMEBUFFER, fboBright);
					 glClearColor(.0f, 0.0f, 0.0f, 1.0f);;
				}
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);  //Render to normal buffer instead
      
        // Clear the screen to default color
        //glClearColor(.2f, 0.4f, 0.8f, 1.0f);
				//glClearColor(1.0f, 1.0f, 1.0f, 1.0f)
				
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
		
		float near_ = .5;
		float far_ = 10;
		float ratio  = screenWidth / (float)screenHeight;
		float fov = 3.14f/4;
		
		glm::vec3 camPos = glm::vec3(3.0f, 0.0f, 0.0f);
		glm::vec3 lookAt = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 right = glm::vec3(0.0f, 1.0f, 0.0f); //TODO: Recompute this!
		
			
			glm::mat4 view = glm::lookAt(
			camPos,  //Cam Position
			lookAt,  //Look at point
			glm::vec3(0.0f, 0.0f, 1.0f)); //Up
		
			glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
		
		
			glm::mat4 proj = glm::perspective(fov, ratio, near_, far_); //FOV, aspect, near, far
			glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
		
		glm::mat4 model;
		model = glm::translate(model,glm::vec3(objx,objy,objz));
		
      
		glBindTexture(GL_TEXTURE_2D, tex);

		glBindVertexArray(modelVAO);
		
		
		//timePast = .5;
		model = glm::rotate(model,.4f*timePast * 3.14f/2,glm::vec3(0.0f, 1.0f, 1.0f));
		model = glm::rotate(model,.4f*timePast * 3.14f/4,glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model,1.f*glm::vec3(1.f,1.f,1.f)); //An example of scale
		glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
		
		glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(.0f, 0.5f, 0.0f)));
		
		glDrawArrays(GL_TRIANGLES, mStart[0], mEnd[0]-mStart[0]); //(Primitives, Start, Number of vertices)
		
		model = glm::mat4();
		model = glm::translate(model,glm::vec3(-1.3,1.85,-.6));
		model = glm::rotate(model,-.2f*timePast * 3.14f/2,glm::vec3(0.0f, 1.0f, 1.0f));
		model = glm::rotate(model,-.2f*timePast * 3.14f/4,glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model,1.f*glm::vec3(1.f,1.f,1.f)); //An example of scale
		glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
		
		glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(.8f, .3f, .0f)));
		
		glDrawArrays(GL_TRIANGLES, mStart[1], mEnd[1]-mStart[1]); 
		
		model = glm::mat4();
		model = glm::translate(model,glm::vec3(-1,-1.6,0));
		model = glm::rotate(model,-.2f*timePast * 3.14f/2,glm::vec3(0.0f, 1.0f, 1.0f));
		model = glm::rotate(model,-.2f*timePast * 3.14f/4,glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model,.8f*glm::vec3(1.f,1.f,1.f)); //An example of scale
		glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
		
		glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(0.1f, 0.1f, .9f)));
		glDrawArrays(GL_TRIANGLES, mStart[2], mEnd[2]-mStart[2]);
		
		
		model = glm::mat4();
		model = glm::translate(model,glm::vec3(-7.2,2.5,2.3));
		model = glm::rotate(model,-.2f*timePast * 3.14f/2,glm::vec3(0.0f, 1.0f, 1.0f));
		model = glm::rotate(model,-.2f*timePast * 3.14f/4,glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model,1.9f*glm::vec3(1.f,1.f,1.f)); //An example of scale
		glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
		
		glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(.4f, 0.2f, .7f)));
		glDrawArrays(GL_TRIANGLES, mStart[3], mEnd[3]-mStart[3]);
		
		
		model = glm::mat4();
		model = glm::translate(model,glm::vec3(0,0,-1.2));
		model = glm::scale(model,.8f*glm::vec3(40.f,10.f,.1f)); //An example of scale
		glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
		
		glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(0.6f, 0.6f, .6f)));
		glDrawArrays(GL_TRIANGLES, mStart[2], mEnd[2]-mStart[2]);
		
    }
      
	// ==============
	// Draw Quad
	glBindFramebuffer(GL_FRAMEBUFFER, 0); //Normal render
	

	glClearColor(.2f, 0.4f, 0.8f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	GLuint textures[2];
	glGenTextures(2, textures);
	
	int width, height;
	unsigned char* image;

	
	
	glUseProgram(quadShader);
	glUniform1i(uniMethod, method);

      
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, dimText); 
	glUniform1i(glGetUniformLocation(quadShader, "texDim"), 0);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, brightText); 
	glUniform1i(glGetUniformLocation(quadShader, "texBright"), 1);
	
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);       
	
	if (saveOutput) Win2PPM(screenWidth,screenHeight);
	

	SDL_GL_SwapWindow(window); //Double buffering
	}
	
	//Clean Up
	glDeleteProgram(modelShader);
    glDeleteBuffers(1, vbo);
    glDeleteVertexArrays(1, &modelVAO);
	glDeleteVertexArrays(1, &quadVAO);

	SDL_GL_DeleteContext(context);
	SDL_Quit();
	return 0;
}

// Create a NULL-terminated string by reading the provided file
static char* readShaderSource(const char* shaderFile)
{
	FILE *fp;
	long length;
	char *buffer;

	// open the file containing the text of the shader code
	fp = fopen(shaderFile, "r");

	// check for errors in opening the file
	if (fp == NULL) {
		printf("can't open shader source file %s\n", shaderFile);
		return NULL;
	}

	// determine the file size
	fseek(fp, 0, SEEK_END); // move position indicator to the end of the file;
	length = ftell(fp);  // return the value of the current position

	// allocate a buffer with the indicated number of bytes, plus one
	buffer = new char[length + 1];

	// read the appropriate number of bytes from the file
	fseek(fp, 0, SEEK_SET);  // move position indicator to the start of the file
	fread(buffer, 1, length, fp); // read all of the bytes

	// append a NULL character to indicate the end of the string
	buffer[length] = '\0';

	// close the file
	fclose(fp);

	// return the string
	return buffer;
}

// Create a GLSL program object from vertex and fragment shader files
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName)
{
	GLuint vertex_shader, fragment_shader;
	GLchar *vs_text, *fs_text;
	GLuint program;

	// check GLSL version
	printf("GLSL version: %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	// Create shader handlers
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	// Read source code from shader files
	vs_text = readShaderSource(vShaderFileName);
	fs_text = readShaderSource(fShaderFileName);

	// error check
	if (vs_text == NULL) {
		printf("Failed to read from vertex shader file %s\n", vShaderFileName);
		exit(1);
	} else if (DEBUG_ON) {
		printf("Vertex Shader:\n=====================\n");
		printf("%s\n", vs_text);
		printf("=====================\n\n");
	}
	if (fs_text == NULL) {
		printf("Failed to read from fragent shader file %s\n", fShaderFileName);
		exit(1);
	} else if (DEBUG_ON) {
		printf("\nFragment Shader:\n=====================\n");
		printf("%s\n", fs_text);
		printf("=====================\n\n");
	}

	// Load Vertex Shader
	const char *vv = vs_text;
	glShaderSource(vertex_shader, 1, &vv, NULL);  //Read source
	glCompileShader(vertex_shader); // Compile shaders
	
	// Check for errors
	GLint  compiled;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		printf("Vertex shader failed to compile:\n");
		if (1){//DEBUG_ON) {
			GLint logMaxSize, logLength;
			glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
			printf("printing error message of %d bytes\n", logMaxSize);
			char* logMsg = new char[logMaxSize];
			glGetShaderInfoLog(vertex_shader, logMaxSize, &logLength, logMsg);
			printf("%d bytes retrieved\n", logLength);
			printf("error message: %s\n", logMsg);
			delete[] logMsg;
		}
		exit(1);
	}
	
	// Load Fragment Shader
	const char *ff = fs_text;
	glShaderSource(fragment_shader, 1, &ff, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled);
	
	//Check for Errors
	if (!compiled) {
		printf("Fragment shader failed to compile\n");
		if (1){//DEBUG_ON) {
			GLint logMaxSize, logLength;
			glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
			printf("printing error message of %d bytes\n", logMaxSize);
			char* logMsg = new char[logMaxSize];
			glGetShaderInfoLog(fragment_shader, logMaxSize, &logLength, logMsg);
			printf("%d bytes retrieved\n", logLength);
			printf("error message: %s\n", logMsg);
			delete[] logMsg;
		}
		exit(1);
	}

	// Create the program
	program = glCreateProgram();

	// Attach shaders to program
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);

	// Link and set program to use
	glLinkProgram(program);
	glUseProgram(program);

	return program;
}

void Win2PPM(int width, int height){
	char outdir[10] = "out/"; //Must be defined!
	int i,j;
	FILE* fptr;
    static int counter = 0;
    char fname[32];
    unsigned char *image;
    
    /* Allocate our buffer for the image */
    image = (unsigned char *)malloc(3*width*height*sizeof(char));
    if (image == NULL) {
      fprintf(stderr,"ERROR: Failed to allocate memory for image\n");
    }
    
    /* Open the file */
    sprintf(fname,"%simage_%04d.ppm",outdir,counter);
    if ((fptr = fopen(fname,"w")) == NULL) {
      fprintf(stderr,"ERROR: Failed to open file for window capture\n");
    }
    
    /* Copy the image into our buffer */
    glReadBuffer(GL_BACK);
    glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,image);
    
    /* Write the PPM file */
    fprintf(fptr,"P6\n%d %d\n255\n",width,height);
    for (j=height-1;j>=0;j--) {
      for (i=0;i<width;i++) {
         fputc(image[3*j*width+3*i+0],fptr);
         fputc(image[3*j*width+3*i+1],fptr);
         fputc(image[3*j*width+3*i+2],fptr);
      }
    }
    
    free(image);
    fclose(fptr);
    counter++;
}