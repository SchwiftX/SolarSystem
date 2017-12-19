//To compile:
//g++ -framework SDL2 -framework OpenGL RenderToTexture.cpp glad/glad.c


#include "glad.h"  //Include order can matter here
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
float angleEarthRevolution = 0;
float angleMoonRevolution = 0;
float angleEarthRotation = 0;
float angleMoonRotation = 0;
float adjustEarthRev = 0;
float adjustMoonRev = 0;
float adjustEarthRot = 0;
float adjustMoonRot = 0;
float baseSpeed = .1;

const int numModels = 5;
int mStart[numModels], mEnd[numModels];


 GLuint tex0;
 GLuint tex1;
 GLuint tex2;
 GLuint tex3;
 GLuint tex4;
 GLuint tex5;
 GLuint tex6;
 GLuint tex7;
 GLuint tex8;

//SJG: Store the object coordinates
//You should have a representation for the state of each object
float objx=1.5, objy=0, objz=0;

int screenWidth = 800;  //500
int screenHeight = 600;  //375

glm::vec3 staColor[1] = {glm::vec3(0.1,0.1,0.1)};
glm::vec3 stdColor[1] = {glm::vec3(0.64,0.64,0.64)};
glm::vec3 stsColor[1] = {glm::vec3(0.0,0.0,0.0)};

const int numPlanets = 7;//0.mercury;1.venus;2.earth;3.moon;4.mars;5.jupiter;6.saturn;
float planetAngleRevolutions[numPlanets];
float planetAngleRotations[numPlanets];
float planetRevolutionSpeeds[numPlanets] = {0.6*.4f,0.6*.25f,0.6*.1f,0.6*1.2f,0.6*.08f,0.6*.05f,0.6*.03f};//–¥À¿
float planetRotationSpeeds[numPlanets] = {0.1*50.f,0.1*42.f,0.1*-25.5f,0.1*-1.2f,0.3*14.f,0.3*7.f,0.3*5.f};//–¥À¿
float planetRadius[numPlanets] = {2.f,3.5f,6.3f,1.2f,7.9f,12.f,15.f};//–¥À¿
float planetSizes[numPlanets] = { .3f,.5f,1.f,.4f,.8f,2.5f,1.f };//–¥À¿

glm::vec3 saturn_pos;

float radiusOfCamera = 20.f;
float rotationOfCamera = 0.f;
float camZ=6.f;

bool DEBUG_ON = false;
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName);
void DrawCelShandingGeometry(int edgeShader, int celShader, glm::mat4 proj, glm::mat4 view, glm::vec3 pos, int texID,int index);
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

	ifstream modelFile;
	int numLines;
	float* model0, *model1, *model2, *model3, *model4;
		
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
	modelFile.open("models/donut.txt");
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
	
    //Load Model 4
    modelFile.open("models/donut.txt");
    numLines = 0;
    modelFile >> numLines;
    model4 = new float[numLines];
    for (int i = 0; i < numLines; i++) {
        modelFile >> model4[i];
    }
    printf("Mode line count: %d\n", numLines);
    mStart[4] = mEnd[3];
    mEnd[4] = mStart[4] + numLines / 8;
    modelFile.close();
	
	//Concatenate model arrays
	float* modelData = new float[mEnd[numModels-1]*8];
	copy(model0, model0+mEnd[0]*8, modelData);
	copy(model1, model1+(mEnd[1]-mStart[1])*8, modelData+mEnd[0]*8);
	copy(model2, model2+(mEnd[2]-mStart[2])*8, modelData+mEnd[1]*8);
	copy(model3, model3+(mEnd[3]-mStart[3])*8, modelData+mEnd[2]*8);
    copy(model4, model4+(mEnd[4]-mStart[4])*8, modelData+mEnd[3]*8);
	
    SDL_Surface* surface = SDL_LoadBMP("sun.bmp");
    if (surface==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    glGenTextures(1, &tex0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0);
    
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    //Load the texture into memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w,surface->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(surface);
    //// End Allocate Texture ///////
    
    
    //// Allocate Texture 1 (Brick) ///////
    SDL_Surface* surface1 = SDL_LoadBMP("mercury.bmp");
    if (surface1==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    glGenTextures(1, &tex1);
    
    //Load the texture into memory
    glActiveTexture(GL_TEXTURE1);
    
    glBindTexture(GL_TEXTURE_2D, tex1);
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    //How to filter
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface1->w,surface1->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface1->pixels);
    
    glGenerateMipmap(GL_TEXTURE_2D);
    
    SDL_FreeSurface(surface1);
    
    
    SDL_Surface* surface2 = SDL_LoadBMP("venus.bmp");
    if (surface2==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    glGenTextures(1, &tex2);
    
    //Load the texture into memory
    glActiveTexture(GL_TEXTURE2);
    
    glBindTexture(GL_TEXTURE_2D, tex2);
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface2->w,surface2->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface2->pixels);
    
    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(surface2);
    
    SDL_Surface* surface3 = SDL_LoadBMP("earth.bmp");
    if (surface3==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    glGenTextures(1, &tex3);
    
    //Load the texture into memory
    glActiveTexture(GL_TEXTURE3);
    
    glBindTexture(GL_TEXTURE_2D, tex3);
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface3->w,surface3->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface3->pixels);
    
    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(surface3);
    
    SDL_Surface* surface4 = SDL_LoadBMP("moon.bmp");
    if (surface4==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    glGenTextures(1, &tex4);
    
    //Load the texture into memory
    glActiveTexture(GL_TEXTURE4);
    
    glBindTexture(GL_TEXTURE_2D, tex4);
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface4->w,surface4->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface4->pixels);
    
    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(surface4);
    
    SDL_Surface* surface5 = SDL_LoadBMP("mars.bmp");
    if (surface5==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    glGenTextures(1, &tex5);
    
    //Load the texture into memory
    glActiveTexture(GL_TEXTURE5);
    
    glBindTexture(GL_TEXTURE_2D, tex5);
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface5->w,surface5->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface5->pixels);
    
    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(surface5);
    
    SDL_Surface* surface6 = SDL_LoadBMP("jupiter.bmp");
    if (surface6==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    glGenTextures(1, &tex6);
    
    //Load the texture into memory
    glActiveTexture(GL_TEXTURE6);
    
    glBindTexture(GL_TEXTURE_2D, tex6);
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface6->w,surface6->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface6->pixels);
    
    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(surface6);
    
    SDL_Surface* surface7 = SDL_LoadBMP("saturn.bmp");
    if (surface7==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    glGenTextures(1, &tex7);
    
    //Load the texture into memory
    glActiveTexture(GL_TEXTURE7);
    
    glBindTexture(GL_TEXTURE_2D, tex7);
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface7->w,surface7->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface7->pixels);
    
    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(surface7);
    
    SDL_Surface* surface8 = SDL_LoadBMP("ring.bmp");
    if (surface8==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    glGenTextures(1, &tex8);
    
    //Load the texture into memory
    glActiveTexture(GL_TEXTURE8);
    
    glBindTexture(GL_TEXTURE_2D, tex8);
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface8->w,surface8->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface8->pixels);
    
    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(surface8);
    
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
	
	int textureShader = InitShader("vertexTex.glsl", "fragmentTex.glsl");
    int phongShader = InitShader("vertex.glsl", "fragment.glsl"); //edge detection
    int celShader = InitShader("vertexS.glsl","fragmentS.glsl"); //cel shading
	glUseProgram(textureShader);
	GLint uniModel = glGetUniformLocation(textureShader, "model");
	GLint uniView = glGetUniformLocation(textureShader, "view");
	GLint uniProj = glGetUniformLocation(textureShader, "proj");
	GLint uniColor = glGetUniformLocation(textureShader, "inColor");

	//*************
	//Create VAO
	//*************
    
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
                if (windowEvent.key.keysym.mod & KMOD_SHIFT)
                    camZ += 1; //Is shift pressed?
                else {
                    if(radiusOfCamera>3)
                        radiusOfCamera -= 1;
                }
            }
            if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_DOWN){ //If "down key" is pressed
                if (windowEvent.key.keysym.mod & KMOD_SHIFT) camZ -= 1; //Is shift pressed?
                else {
                    radiusOfCamera += 1;
                }
            }
            if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_LEFT){ //If "up key" is pressed
                rotationOfCamera += .01*3.14f;
            }
            if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_RIGHT){ //If "down key" is pressed
                rotationOfCamera -= .01*3.14f;
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
				method %= 2;
				printf("Method: %d\n",method);
			}
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_o){ 
				offAxis = !offAxis;
				printf("Projection: %s\n",offAxis?"Off Axis":"Toe In");
			}
      }
      
      if (!saveOutput) timePast = SDL_GetTicks()/1000.f; 
      if (saveOutput) timePast += .07; //Fix framerate at 14 FPS
        
      //correspond texture to index

      
      newTime = SDL_GetTicks()/1000.f;
      //printf("%f FPS\n",1/(newTime-lastTime));
      lastTime = SDL_GetTicks()/1000.f;
        for (int i = 0; i < numPlanets; i++) {
            planetAngleRevolutions[i] = planetRevolutionSpeeds[i] * timePast*3.14f;
            planetAngleRotations[i] = planetRotationSpeeds[i] * timePast*3.14f;
        }
        glm::vec3 sunPos = glm::vec3(0, 0, 0);
        glm::vec3 earthPos = sunPos + glm::vec3(planetRadius[2]*sin(planetAngleRevolutions[2]), planetRadius[2] *cos(planetAngleRevolutions[2]), 0);
        glm::vec3 saturnPos = sunPos + glm::vec3(planetRadius[6]*sin(planetAngleRevolutions[6]), planetRadius[6] *cos(planetAngleRevolutions[6]), 0);
        saturn_pos = saturnPos;
        glm::vec3 moonPos = earthPos + glm::vec3(planetRadius[3] * sin(planetAngleRevolutions[3]), planetRadius[3] * cos(planetAngleRevolutions[3]), 0);
        
        glm::vec3 Planet_pos[numPlanets];
        for (int i = 0; i < numPlanets; i++) {
            glm::vec3 planetPos;
            if (i==3)//Moon
                Planet_pos[i] = earthPos + glm::vec3(planetRadius[i] * sin(planetAngleRevolutions[i]), planetRadius[i] * cos(planetAngleRevolutions[i]), 0);
            else
                Planet_pos[i] = sunPos + glm::vec3(planetRadius[i] * sin(planetAngleRevolutions[i]), planetRadius[i] * cos(planetAngleRevolutions[i]), 0);
        }

      glUseProgram(textureShader); //Use the normal model shader (phong) for each eye
      for (int pass = 0; pass < 2; pass++){
            GLuint modelVAO;
        if (pass == 1){
            //Build a Vertex Array Object. This stores the attribute mappings from the model shader to the model VBOs
            glGenVertexArrays(1, &modelVAO); //Create a VAO
            glBindVertexArray(modelVAO); //Bind the above created VAO to the current context
            
            //Allocate memory on the graphics card to store geometry (vertex buffer object)
            GLuint vbo[1];
            glGenBuffers(1, vbo);  //Create 1 buffer called vbo
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //Set the vbo as the active array buffer (Only one buffer can be active at a time)
            
            glBufferData(GL_ARRAY_BUFFER, mEnd[numModels-1]*8*sizeof(float), modelData, GL_DYNAMIC_DRAW); //upload vertices to vbo
            
            //Tell OpenGL how to set model rendering vertex shader input
            GLint posAttrib = glGetAttribLocation(textureShader, "position");
            glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
            glEnableVertexAttribArray(posAttrib);
            
            GLint normAttrib = glGetAttribLocation(textureShader, "inNormal");
            glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(5*sizeof(float)));
            glEnableVertexAttribArray(normAttrib);
            
            GLint texAttrib = glGetAttribLocation(textureShader, "inTexcoord");
            glEnableVertexAttribArray(texAttrib);
            glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
            
            glUseProgram(textureShader); //Use the normal model shader (phong) for each eye
			glBindFramebuffer(GL_FRAMEBUFFER, fboDim);
            glClearColor(.1f, .1f, 0.1f, 1.0f);
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex0);
            glUniform1i(glGetUniformLocation(textureShader, "tex0"), 0); //sun

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, tex1);
            glUniform1i(glGetUniformLocation(textureShader, "tex1"), 1);  //mercury

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, tex2);
            glUniform1i(glGetUniformLocation(textureShader, "tex2"), 2);  //venus
            
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, tex3);
            glUniform1i(glGetUniformLocation(textureShader, "tex3"), 3);  //earth
            
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, tex4);
            glUniform1i(glGetUniformLocation(textureShader, "tex4"), 4);  //moon
            
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, tex5);
            glUniform1i(glGetUniformLocation(textureShader, "tex5"), 5);  //mars
            
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_2D, tex6);
            glUniform1i(glGetUniformLocation(textureShader, "tex6"), 6);  //jupiter
            
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_2D, tex7);
            glUniform1i(glGetUniformLocation(textureShader, "tex7"), 7);  //saturn
            
            glActiveTexture(GL_TEXTURE8);
            glBindTexture(GL_TEXTURE_2D, tex8);
            glUniform1i(glGetUniformLocation(textureShader, "tex8"), 8);  //saturn ring
            }
        else{
            //Build a Vertex Array Object. This stores the attribute mappings from the model shader to the model VBOs
            glGenVertexArrays(1, &modelVAO); //Create a VAO
            glBindVertexArray(modelVAO); //Bind the above created VAO to the current context
            
            //Allocate memory on the graphics card to store geometry (vertex buffer object)
            GLuint vbo[1];
            glGenBuffers(1, vbo);  //Create 1 buffer called vbo
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //Set the vbo as the active array buffer (Only one buffer can be active at a time)
            
            glBufferData(GL_ARRAY_BUFFER, mEnd[numModels-1]*8*sizeof(float), modelData, GL_DYNAMIC_DRAW); //upload vertices to vbo
            //Tell OpenGL how to set model rendering vertex shader input
            //Tell OpenGL how to set model rendering vertex shader input
            GLint posAttrib = glGetAttribLocation(textureShader, "position");
            glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
            glEnableVertexAttribArray(posAttrib);
            
            GLint normAttrib = glGetAttribLocation(textureShader, "inNormal");
            glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(5*sizeof(float)));
            glEnableVertexAttribArray(normAttrib);
            
            GLint texAttrib = glGetAttribLocation(textureShader, "inTexcoord");
            glEnableVertexAttribArray(texAttrib);
            glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
            glBindFramebuffer(GL_FRAMEBUFFER, fboBright);
            glClearColor(.3f, 0.3f, 0.3f, 1.0f);
            
            GLint sposAttrib = glGetAttribLocation(phongShader,"i_position");
            glVertexAttribPointer(sposAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
            glEnableVertexAttribArray(sposAttrib);
            
            GLint snormalAttrib = glGetAttribLocation(phongShader,"i_normal");
            glVertexAttribPointer(snormalAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(5*sizeof(float)));
            glEnableVertexAttribArray(snormalAttrib);
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex0);
            glUniform1i(glGetUniformLocation(textureShader, "tex0"), 0); //sun
            
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, tex1);
            glUniform1i(glGetUniformLocation(textureShader, "tex1"), 1);  //mercury
            
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, tex2);
            glUniform1i(glGetUniformLocation(textureShader, "tex2"), 2);  //venus
            
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, tex3);
            glUniform1i(glGetUniformLocation(textureShader, "tex3"), 3);  //earth
            
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, tex4);
            glUniform1i(glGetUniformLocation(textureShader, "tex4"), 4);  //moon
            
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, tex5);
            glUniform1i(glGetUniformLocation(textureShader, "tex5"), 5);  //mars
            
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_2D, tex6);
            glUniform1i(glGetUniformLocation(textureShader, "tex6"), 6);  //jupiter
            
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_2D, tex7);
            glUniform1i(glGetUniformLocation(textureShader, "tex7"), 7);  //saturn
            
            glActiveTexture(GL_TEXTURE8);
            glBindTexture(GL_TEXTURE_2D, tex8);
            glUniform1i(glGetUniformLocation(textureShader, "tex8"), 8);  //saturn ring
            }
				
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
          glm::vec3 camPos = glm::vec3(radiusOfCamera*cos(rotationOfCamera), radiusOfCamera*sin(rotationOfCamera), camZ);
          glm::vec3 lookAt = glm::vec3(0.0f, 0.0f, 0.0f);
          glm::vec3 right = glm::vec3(0.0f, 1.0f, 0.0f); //TODO: Recompute this!
          glm::mat4 view = glm::lookAt(
                                       camPos,  //Cam Position
                                       lookAt,  //Look at point
                                       glm::vec3(-4.0f, 0.0f, 16.0f)); //Up
          glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
		
		float near_ = .5;
		float far_ = 80;
		float ratio  = screenWidth / (float)screenHeight;
		float fov = 3.14f/3.5;
        glm::mat4 proj = glm::perspective(fov, ratio, near_, far_); //FOV, aspect, near, far
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
		glm::mat4 model;
		//model = glm::translate(model,glm::vec3(objx,objy,objz));

		glBindVertexArray(modelVAO);
		//timePast = .5;
        if(pass == 1)
        {
            
        GLint pLight1_pos = glGetUniformLocation(textureShader, "pLight1.position");
        glm::vec4 pL1_pos = view*glm::vec4(0.8*earthPos.x,0.8*earthPos.y,0.8*earthPos.z,1);
        glUniform3fv(pLight1_pos, 1, glm::value_ptr(pL1_pos));
            
        GLint pLight1_intensity = glGetUniformLocation(textureShader,"pLight1.intensity");
        glm::vec3 pL1_intensity(2,2,2);
        glUniform3fv(pLight1_intensity, 1, glm::value_ptr(pL1_intensity));
            
        GLint pLight2_pos = glGetUniformLocation(textureShader, "pLight2.position");
        glm::vec4 pL2_pos = view*glm::vec4(0.8*Planet_pos[0].x,0.8*Planet_pos[0].y,0.8*Planet_pos[0].z,1);
        glUniform3fv(pLight2_pos, 1, glm::value_ptr(pL2_pos));
            
        GLint pLight2_intensity = glGetUniformLocation(textureShader,"pLight2.intensity");
        glm::vec3 pL2_intensity(2,2,2);
        glUniform3fv(pLight2_intensity, 1, glm::value_ptr(pL2_intensity));
        
        GLint pLight3_pos = glGetUniformLocation(textureShader, "pLight3.position");
        glm::vec4 pL3_pos = view*glm::vec4(0.8*Planet_pos[1].x,0.8*Planet_pos[1].y,0.8*Planet_pos[1].z,1);
        glUniform3fv(pLight3_pos, 1, glm::value_ptr(pL3_pos));
            
        GLint pLight3_intensity = glGetUniformLocation(textureShader,"pLight3.intensity");
        glm::vec3 pL3_intensity(2,2,2);
        glUniform3fv(pLight3_intensity, 1, glm::value_ptr(pL3_intensity));
            
        GLint pLight4_pos = glGetUniformLocation(textureShader, "pLight4.position");
        glm::vec4 pL4_pos = view*glm::vec4(0.8*Planet_pos[4].x,0.8*Planet_pos[4].y,0.8*Planet_pos[4].z,1);
        glUniform3fv(pLight4_pos, 1, glm::value_ptr(pL4_pos));
            
        GLint pLight4_intensity = glGetUniformLocation(textureShader,"pLight4.intensity");
        glm::vec3 pL4_intensity(2,2,2);
        glUniform3fv(pLight4_intensity, 1, glm::value_ptr(pL4_intensity));
            
        GLint pLight5_pos = glGetUniformLocation(textureShader, "pLight5.position");
        glm::vec4 pL5_pos = view*glm::vec4(0.8*Planet_pos[5].x,0.8*Planet_pos[5].y,0.8*Planet_pos[5].z,1);
        glUniform3fv(pLight5_pos, 1, glm::value_ptr(pL5_pos));
            
        GLint pLight5_intensity = glGetUniformLocation(textureShader,"pLight5.intensity");
        glm::vec3 pL5_intensity(2,2,2);
        glUniform3fv(pLight5_intensity, 1, glm::value_ptr(pL5_intensity));
            
        GLint pLight6_pos = glGetUniformLocation(textureShader, "pLight6.position");
        glm::vec4 pL6_pos = view*glm::vec4(0.8*Planet_pos[6].x,0.8*Planet_pos[6].y,0.8*Planet_pos[6].z,1);
        glUniform3fv(pLight6_pos, 1, glm::value_ptr(pL6_pos));
            
        GLint pLight6_intensity = glGetUniformLocation(textureShader,"pLight6.intensity");
        glm::vec3 pL6_intensity(2,2,2);
        glUniform3fv(pLight6_intensity, 1, glm::value_ptr(pL6_intensity));
            
        GLint uniaColor = glGetUniformLocation(textureShader, "aColor");
        glUniform3fv(uniaColor,1,glm::value_ptr(staColor[0]));
            
        GLint unidColor = glGetUniformLocation(textureShader, "dColor");
        glUniform3fv(unidColor,1,glm::value_ptr(stdColor[0]));
            
        GLint unisColor = glGetUniformLocation(textureShader, "sColor");
        glUniform3fv(unisColor,1,glm::value_ptr(stsColor[0]));
            
        GLint uniNs = glGetUniformLocation(textureShader, "Ns");
        glUniform1i(uniNs,96.078431);
            
        GLint uniCel = glGetUniformLocation(textureShader, "isCelShading");
        glUniform1i(uniCel, 0);
            
//        model = glm::translate(model, sunPos);
//        model = glm::scale(model, 3.f*glm::vec3(1.f, 1.f, 1.f)); //An example of scale
//        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
//        glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(.4f, 0.2f, .7f)));
//        GLint uniTexSunID = glGetUniformLocation(textureShader, "texID");
//        glUniform1i(uniTexSunID, 0);
//        glDrawArrays(GL_TRIANGLES, mStart[3], mEnd[3] - mStart[3]);//Sphere as Sun
//
//        model = glm::mat4();
//        model = glm::translate(model, earthPos);
//        model = glm::rotate(model, 23.f/180.f*3.14f, glm::vec3(1.0f, 0.f, 0.0f));//obliquity of the ecliptic
//        model = glm::rotate(model, angleEarthRotation, glm::vec3(0.0f, 0.f, 1.f));
//        model = glm::scale(model, 1.f*glm::vec3(1.f, 1.f, 1.f)); //An example of scale
//        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
//        GLint uniTexEarthID = glGetUniformLocation(textureShader, "texID");
//        glUniform1i(uniTexEarthID, 2);
//        glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(.8f, .3f, .0f)));
//
//        glDrawArrays(GL_TRIANGLES, mStart[3], mEnd[3] - mStart[3]); //Earth
//
//        model = glm::mat4();
//        model = glm::translate(model, moonPos);
//        model = glm::rotate(model,angleMoonRotation,glm::vec3(0.0f, 0.0f, 1.0f));
//        model = glm::scale(model,.5f*glm::vec3(1.f,1.f,1.f)); //An example of scale
//        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
//        glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(.0f, 0.5f, 0.0f)));
//        GLint uniTexMoonID = glGetUniformLocation(textureShader, "texID");
//        glUniform1i(uniTexMoonID, 1);
//        glDrawArrays(GL_TRIANGLES, mStart[3], mEnd[3]-mStart[3]); //(Primitives, Start, Number of vertices)//Moon
            model = glm::translate(model, sunPos);
            model = glm::scale(model, 3.f*glm::vec3(1.f, 1.f, 1.f)); //An example of scale
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(.4f, 0.2f, .7f)));
            GLint uniID = glGetUniformLocation(textureShader, "texID");
            glUniform1i(uniID, 0);
            glDrawArrays(GL_TRIANGLES, mStart[3], mEnd[3] - mStart[3]);//Sphere as Sun
            
            for (int i = 0; i < numPlanets; i++) {
                model = glm::mat4();
                glm::vec3 planetPos;
                if (i==3)//Moon
                    planetPos = earthPos + glm::vec3(planetRadius[i] * sin(planetAngleRevolutions[i]), planetRadius[i] * cos(planetAngleRevolutions[i]), 0);
                else
                    planetPos = sunPos + glm::vec3(planetRadius[i] * sin(planetAngleRevolutions[i]), planetRadius[i] * cos(planetAngleRevolutions[i]), 0);
                model = glm::translate(model, planetPos);
                if(i==2)//Earth
                    model = glm::rotate(model, 23.f / 180.f*3.14f, glm::vec3(1.0f, 0.f, 0.0f));//obliquity of the ecliptic
                model = glm::rotate(model, planetAngleRotations[i], glm::vec3(0.0f, 0.f, 1.f));
                model = glm::scale(model, planetSizes[i]*glm::vec3(1.f, 1.f, 1.f)); //An example of scale
                glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
                glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(0.1f, 0.1f, .9f)));
                GLint uniID = glGetUniformLocation(textureShader, "texID");
                glUniform1i(uniID, i+1);
                glDrawArrays(GL_TRIANGLES, mStart[3], mEnd[3] - mStart[3]); //Earth
            }
            
            model = glm::mat4();
            model = glm::translate(model, saturnPos);
            model = glm::scale(model, 1.f*glm::vec3(1.f, 1.f, .1f)); //An example of scale
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(.8f, .3f, .0f)));
            uniID = glGetUniformLocation(textureShader, "texID");
            glUniform1i(uniID, 8);
            glDrawArrays(GL_TRIANGLES, mStart[1], mEnd[1] - mStart[1]); //Saturn ring
        }
        else
        {
            DrawCelShandingGeometry(phongShader, textureShader, proj, view, sunPos, 0, 0);
            for(int i=0; i<numPlanets; i++)
            {
                DrawCelShandingGeometry(phongShader, textureShader, proj, view, Planet_pos[i], i+1, i);
            }
            DrawCelShandingGeometry(phongShader, textureShader, proj, view, saturn_pos, 8, 8);
        }
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
	glDeleteProgram(textureShader);
    //glDeleteBuffers(1, vbo);
    //glDeleteVertexArrays(1, &modelVAO);
	glDeleteVertexArrays(1, &quadVAO);

	SDL_GL_DeleteContext(context);
	SDL_Quit();
	return 0;
}

void DrawCelShandingGeometry(int edgeShader, int textureShader, glm::mat4 proj, glm::mat4 view, glm::vec3 pos, int texID, int index)
{
    //edge detection
    glUseProgram(edgeShader);
    glm::mat4 model = glm::mat4();
    if(index == 8) //Saturn Ring
    {
        model = glm::mat4();
        model = glm::translate(model, saturn_pos);
        model = glm::scale(model, 1.f*glm::vec3(1.f, 1.f, .1f)); //An example of scale
    }
    else if(texID == 0) //Sun
    {
        model = glm::translate(model, pos);
        model = glm::scale(model, 3.f*glm::vec3(1.f, 1.f, 1.f)); //An example of scale
    }
    else if(index == 2) //Earth
    {
        model = glm::translate(model, pos);
        model = glm::rotate(model, 23.f/180.f*3.14f, glm::vec3(1.0f, 0.f, 0.0f));//obliquity of the ecliptic
        model = glm::rotate(model, planetAngleRotations[index], glm::vec3(0.0f, 0.f, 1.f));
        model = glm::rotate(model, 3.14f/2, glm::vec3(1.0f, 0.f, 0.0f));
        model = glm::scale(model, 1.f*glm::vec3(1.f, 1.f, 1.f)); //An example of scale
    }else
    {
        model = glm::translate(model, pos);
        model = glm::rotate(model, planetAngleRotations[index], glm::vec3(0.0f, 0.f, 1.f));
        model = glm::rotate(model, 3.14f/2, glm::vec3(1.0f, 0.f, 0.0f));
        model = glm::scale(model, planetSizes[index]*glm::vec3(1.f, 1.f, 1.f)); //An example of scale
    }
    GLint uniMVP = glGetUniformLocation(edgeShader,"u_mvp_mat");
    glm::mat4 MVP = proj*view*model;
    glUniformMatrix4fv(uniMVP, 1, GL_FALSE, glm::value_ptr(MVP));
    
    GLint offset = glGetUniformLocation(edgeShader,"u_offset1");
    glUniform1f(offset,0.03f);
    GLint incolor1 = glGetUniformLocation(edgeShader,"u_color1");
    glm::vec3 back_color(0.0f,0.0f,0.0f);
    glUniform3fv(incolor1,1,glm::value_ptr(back_color));
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glDepthMask(GL_FALSE);
    glDrawArrays(GL_TRIANGLES, mStart[3], mEnd[3]-mStart[3]); //(Primitive Type, Start Vertex, End Vertex)
    glDisable(GL_CULL_FACE);
    
    //add cel shading effect
    glUseProgram(textureShader);
    GLint uniProj = glGetUniformLocation(textureShader,"proj");
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
    GLint uniView = glGetUniformLocation(textureShader,"view");
    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
    GLint uniModel = glGetUniformLocation(textureShader,"model");
    glUniformMatrix4fv(uniModel,1,GL_FALSE,glm::value_ptr(model));
    GLint uniColor = glGetUniformLocation(textureShader,"inColor");
    glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(.4f, 0.2f, .7f)));
    if(index ==0 && texID == 0)
    {
        GLint uniID = glGetUniformLocation(textureShader, "texID");
        glUniform1i(uniID, 0);
    }
    else
    {
        GLint uniID = glGetUniformLocation(textureShader, "texID");
        glUniform1i(uniID, texID);
    }
    GLint uniNumOfShading = glGetUniformLocation(textureShader,"u_numShades");
    GLint uniCel = glGetUniformLocation(textureShader, "isCelShading");
    glUniform1i(uniCel, 1);
    glUniform1f(uniNumOfShading,2);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glDepthMask(GL_TRUE);
    if(texID == 8)
    {
        glDrawArrays(GL_TRIANGLES,mStart[1], mEnd[1]-mStart[1]);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES,mStart[3], mEnd[3]-mStart[3]);
    }
    glDisable(GL_CULL_FACE);
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




