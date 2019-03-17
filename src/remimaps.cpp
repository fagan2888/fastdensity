// sous mac
// >g++ -std=c++11 -I/usr/local/include/ -lglfw -lGLEW -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs remimaps.cpp -framework OpenGL  -oremimaps
// >./vorocell
 
 
 
// Inclut les en-tetes standards
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <random>
using namespace std;
 
#include <stdlib.h>
#include <string.h>
 
#include <GL/glew.h>
#include <GLFW/glfw3.h>
 
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
 
#ifdef __APPLE__
  #include <OpenGL/gl.h>
#else
  #include <GL/gl.h>
#endif
 
// #define GLM_FORCE_RADIANS
// #include <glm/glm.hpp>
// #include <glm/gtc/constants.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>
 
 
// using namespace glm;
 
// default values
int window_width = 32;
int window_height = 32;

GLuint fbo;
GLuint color_renderbuffer;

GLuint programID;
cv::Mat img;

int N        = 1000000;
int NB_CAT   = 46;
int mapSize = 32;

GLuint *VertexArrayIDAs;
 
// stocke les variables uniformes qui seront communes a tous les vertex dessines
// dans une variable globale CAR
// C'est dans la fonction loadShaders que nous pouvos recupere les bonnes valeurs de pointeur (une fois le shader compile/linke)
// c'est dans le main que nous pouvons donne les bonnes valeurs "au bout du pointeur" pour que les shaders les recoivent
GLint uniform_projP, uniform_viewP, uniform_modelP;
 
double rnd() {return double(rand())/RAND_MAX;}
 
 
/*
██       ██████   █████  ██████    ███████ ██   ██  █████  ██████  ███████ ██████  ███████
██      ██    ██ ██   ██ ██   ██   ██      ██   ██ ██   ██ ██   ██ ██      ██   ██ ██
██      ██    ██ ███████ ██   ██   ███████ ███████ ███████ ██   ██ █████   ██████  ███████
██      ██    ██ ██   ██ ██   ██        ██ ██   ██ ██   ██ ██   ██ ██      ██   ██      ██
███████  ██████  ██   ██ ██████    ███████ ██   ██ ██   ██ ██████  ███████ ██   ██ ███████
*/
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){
  // Create the shaders
  GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
   
  // Read the Vertex Shader code from the file
  std::string VertexShaderCode;
  std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
  if (VertexShaderStream.is_open()){
    std::string Line = "";
    while(getline(VertexShaderStream, Line))
      VertexShaderCode += "\n" + Line;
    VertexShaderStream.close();
  } else {
    printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
    getchar();
    return 0;
  }
   
  // Read the Fragment Shader code from the file
  std::string FragmentShaderCode;
  std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
  if(FragmentShaderStream.is_open()){
    std::string Line = "";
    while(getline(FragmentShaderStream, Line))
      FragmentShaderCode += "\n" + Line;
    FragmentShaderStream.close();
  }
   
  GLint Result = GL_FALSE;
  int InfoLogLength;
   
   
  // Compile Vertex Shader
  printf("Compiling shader : %s\n", vertex_file_path);
  char const * VertexSourcePointer = VertexShaderCode.c_str();
  glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
  glCompileShader(VertexShaderID);
   
  // Check Vertex Shader
  glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if ( InfoLogLength > 0 ){
    std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    printf("%s\n", &VertexShaderErrorMessage[0]);
  }
   
   
  // Compile Fragment Shader
  printf("Compiling shader : %s\n", fragment_file_path);
  char const * FragmentSourcePointer = FragmentShaderCode.c_str();
  glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
  glCompileShader(FragmentShaderID);
   
  // Check Fragment Shader
  glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if ( InfoLogLength > 0 ){
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    printf("%s\n", &FragmentShaderErrorMessage[0]);
  }
   
  // Link the program
  printf("Linking program\n");
  GLuint ProgramID = glCreateProgram();
  glAttachShader(ProgramID, VertexShaderID);
  glAttachShader(ProgramID, FragmentShaderID);
  glLinkProgram(ProgramID);
   
  // Check the program
  glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
  glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if ( InfoLogLength > 0 ){
    std::vector<char> ProgramErrorMessage(InfoLogLength+1);
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    printf("%s\n", &ProgramErrorMessage[0]);
  }
   
  glDetachShader(ProgramID, VertexShaderID);
  glDetachShader(ProgramID, FragmentShaderID);
   
  glDeleteShader(VertexShaderID);
  glDeleteShader(FragmentShaderID);
   
  return ProgramID;
}
 
 
 
 
void key_callback(GLFWwindow *window, GLFWcharfun cbfun){
  //c/bfun
}

/*
DEBUG FUNCTION
display a 2 dimensional array given as param on the standard output
*/
template< typename T >
static void display_2Darray(vector<vector<T>> tab) {

	cout << endl;

	for (unsigned int i = 0; i < tab.size(); i++) {
		for (unsigned int j = 0; j < tab[i].size(); j++) {
			cout << tab[i][j] << " | ";
		}
		cout << endl;
	}
}

/*
 ██████ ██   ██  █████  ██████   █████   ██████ ████████ ███████ ██████           ██████  █████  ██      ██      ██████   █████   ██████ ██   ██
██      ██   ██ ██   ██ ██   ██ ██   ██ ██         ██    ██      ██   ██         ██      ██   ██ ██      ██      ██   ██ ██   ██ ██      ██  ██
██      ███████ ███████ ██████  ███████ ██         ██    █████   ██████          ██      ███████ ██      ██      ██████  ███████ ██      █████
██      ██   ██ ██   ██ ██   ██ ██   ██ ██         ██    ██      ██   ██         ██      ██   ██ ██      ██      ██   ██ ██   ██ ██      ██  ██
 ██████ ██   ██ ██   ██ ██   ██ ██   ██  ██████    ██    ███████ ██   ██ ███████  ██████ ██   ██ ███████ ███████ ██████  ██   ██  ██████ ██   ██
*/
void character_callback(GLFWwindow* window, unsigned int codepoint){
  //cout <<"character_callback "<<codepoint<<" "<<endl;
  const uint32_t DEF_UINT = 77;//td::numeric_limits< uint32_t >::max();
  const float DEF_FLOAT = 7.654321;//td::numeric_limits< uint32_t >::max();

  switch (codepoint) {
    case 's':
      cout <<"window width: "<<window_width<<endl;
      glBindFramebuffer(GL_FRAMEBUFFER, fbo);
      // clear before every draw 1
      glClearColor(0.0, 0.0, 0.0, 1.0);

      glClearBufferfv(GL_RENDERBUFFER, color_renderbuffer, &DEF_FLOAT);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
              
      if (VertexArrayIDAs!=0){
        
        glUseProgram(programID);
        
        glBindVertexArray(VertexArrayIDAs[0]);
        glDrawArrays(GL_POINTS, 0, N); //all the vertices
        glBindVertexArray (0);       
        glUseProgram(0);

        glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3)?1:4);
        glPixelStorei(GL_PACK_ROW_LENGTH, img.step/img.elemSize()); 
        glReadPixels(0, 0, img.cols, img.rows, GL_RED, GL_FLOAT, img.data);

        cv::Mat flipped(img);
        cv::flip(img, flipped, 0);
        /*for (int i=0; i<700; i++){
          cout <<i<<": "<<flipped.at<float>(i,907, 0)<<endl;
          flipped.at<float>(i, i, 0) += 128.5;
        }*/
		vector<vector<unsigned int>> densityMap(mapSize, vector<unsigned int>(mapSize));

		for (int i = 0; i < window_height; i++) {
			for (int j = 0; j < window_width; j++) {
				//cout << "( " << i << " ; " << j << " ) ==> " << flipped.at<float>(i, j) << endl;
				densityMap[j][i] = flipped.at<float>(i, j);
			}
		}
		display_2Darray(densityMap);
        cv::imwrite("snapshot.png", img);
      }    
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    break;
     
    case 'e':
    break;
    
    case 'o':
      break;
    
    case 'i':
      break;
     
    case 'l':
    break;
    
    case 'f':
    break;
     
    case 'q':
     break;
      
    case 'w':

     break;
     
    case 'v':
    break;
     
    case 'p':
    break;
     
    case '+':
    break;
     
    case '-':
    break;
  }
   
}


/*
███    ███  █████  ██   ██ ███████ ███    ███  ██████  ██████  ███████ ██       █████
████  ████ ██   ██ ██  ██  ██      ████  ████ ██    ██ ██   ██ ██      ██      ██   ██
██ ████ ██ ███████ █████   █████   ██ ████ ██ ██    ██ ██   ██ █████   ██      ███████
██  ██  ██ ██   ██ ██  ██  ██      ██  ██  ██ ██    ██ ██   ██ ██      ██      ██   ██
██      ██ ██   ██ ██   ██ ███████ ██      ██  ██████  ██████  ███████ ███████ ██   ██
*/
GLuint *makeModelA(int width, int height, int seed) {
  cout << "  makeModelA "<<N<<" "<<N<<endl;
   
  srand(seed); //seed of the rng && should reset the seed at it's starting point.

  GLfloat* g_vertex_pos_data1    = new GLfloat[N*2];

  for(int i=0; i<N; i++) {
    //g_vertex_pos_data1[i*2+0] = (rand() % (width + 1));
    //g_vertex_pos_data1[i*2+1] = (rand() % (height + 1));
	  g_vertex_pos_data1[i * 2 + 0] = rnd();
	  g_vertex_pos_data1[i * 2 + 1] = rnd();
  }


    // Bon maintenant on cree le VAO et cette fois on va s'en servir !
      GLuint *VertexArrayIDs = new GLuint[1];
      glGenVertexArrays(1, &VertexArrayIDs[0]);
      glBindVertexArray(VertexArrayIDs[0]);
       
      // Spheres 
      // Generate 1 buffer, put the resulting identifier in vertexbuffer
      GLuint vertexbuffer1;
      glGenBuffers(1, &vertexbuffer1);
       
      // The following commands will talk about our 'vertexbuffer' buffer
      glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer1);
        // Only allocate memory. Do not send yet our vertices to OpenGL.
        glBufferData(GL_ARRAY_BUFFER, N*2*sizeof(GLfloat), 0, GL_STATIC_DRAW);
                                       
        glBufferSubData(GL_ARRAY_BUFFER, 0,                   N*2*sizeof(GLfloat), g_vertex_pos_data1);
                                         
        glVertexAttribPointer(
           0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
           2,                  // size
           GL_FLOAT,           // type
           GL_FALSE,           // normalized?
           0,                  // stride
           (void*)0            // array buffer offset
        );
        glEnableVertexAttribArray(0);
         
         
      glBindBuffer(GL_ARRAY_BUFFER, 0);
       
      // on desactive le VAO a la fin de l'initialisation
      glBindVertexArray (0);

      delete[] g_vertex_pos_data1;

      //cout << "  makeModelA done "<<N<<endl;
       
      return VertexArrayIDs;    
}


  
/*
███    ███  █████  ██ ███    ██
████  ████ ██   ██ ██ ████   ██
██ ████ ██ ███████ ██ ██ ██  ██
██  ██  ██ ██   ██ ██ ██  ██ ██
██      ██ ██   ██ ██ ██   ████
*/
int main(int argc, char * argv[]){
  cout << "\033[1;31mremimaps is located here:\033[0m "<<argv[0]<<endl;
  cout << "";
  cout << "   \033[1;41mpress\033[0m\033[32m s \033[0mto save a screenshot png"<<endl;
  cout << "   \033[1;41mpress\033[0m\033[32m e \033[0mto  "<<endl;
  cout << "   \033[1;41mpress\033[0m\033[32m + or - \033[0mto "<<endl;
  cout << "   \033[1;41mpress\033[0m\033[32m i \033[0mto "<<endl;
  cout << "   \033[1;41mpress\033[0m\033[32m l \033[0mto "<<endl;
  cout << "   \033[1;41mpress\033[0m\033[32m v \033[0mto "<<endl;
  cout << "   \033[1;41mpress\033[0m\033[32m p \033[0mto \033[0m"<<endl;
  //system("pwd");
   
  // Initialise GLFW
  if( !glfwInit() ) {
      fprintf( stderr, "Failed to initialize GLFW\n" );
      return -1;
  }
   
  glfwWindowHint(GLFW_SAMPLES, 1); // 2x antialiasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // On veut OpenGL 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Pour rendre MacOS heureux ; ne devrait pas tre necessaire
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // On ne veut pas l'ancien OpenGL
  glfwWindowHint(GLFW_DEPTH_BITS, 24);
   
  GLFWwindow* window = NULL; 
  // cherche le bon moniteur et la meilleure resolution   
  int count;
  GLFWmonitor** monitors = glfwGetMonitors(&count);
  for (int i=0; i<count; i++){
    GLFWmonitor* monitor = monitors[i];
    int xpos, ypos;
    glfwGetMonitorPos(monitor, &xpos, &ypos);
    int count2, widthMM, heightMM;
    glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);
    cout <<widthMM<<" x "<<heightMM<<endl;
    cout << "    at  "<<xpos <<", "<< ypos <<endl;
    const GLFWvidmode* modes = glfwGetVideoModes(monitor, &count2);
    for (int j=0; j<count2; j++){
      cout << "  "<<modes[j].width <<"x"<< modes[j].height <<endl;
      if (modes[j].width==3360 && modes[j].height==2100){
        window_width = modes[j].width/2;
        window_height = modes[j].height/2-20;
        
        window = glfwCreateWindow( window_width, window_height, "remimaps", nullptr, nullptr);
        glfwGetWindowPos(window, &xpos, &ypos);		
        cout << "    created at  "<<xpos <<", "<< ypos <<endl;
        glfwWindowHint(GLFW_SAMPLES, 2); // 4x antialiasing

      }
    }
  }
   
  // Ouvre une fenetre et cree son contexte OpenGl
  if (window==NULL) window = glfwCreateWindow(window_width, window_height, "remimaps safe mode1024x768", nullptr, nullptr);
  if( window == NULL ){
      fprintf( stderr, "Failed to open GLFW window. If you have an old Intel GPU, they are not 4.1 compatible.\n" );
      glfwTerminate();
      return -1;
  }
   
  glfwMakeContextCurrent(window); // Initialise GLEW
  cout << "version: "<<endl; 
  cout << "default shader lang: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
  cout << "opengl : " << glGetString(GL_VERSION) << endl;
  cout <<" GLFW opengl "<<glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR)<<"."<<glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR)<<endl;

  glfwSwapInterval(0);
  glewExperimental=true; // Nécessaire dans le profil de base
  if (glewInit() != GLEW_OK) {
      fprintf(stderr, "Failed to initialize GLEW\n");
      return -1;
  }
   
  //  depth test.. mais je pense pas que ce soit utile pour faire un heatmap. on ne veut rien omettre a priori
  glDisable(GL_DEPTH_TEST);
  //glEnable(GL_DEPTH_TEST);

  //glDepthFunc(GL_LESS);
  //glDepthRange(0, 50);

  /*GLuint texture_map;
  glGenTextures(1, &texture_map);
  glBindTexture(GL_TEXTURE_2D, texture_map);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, window_width, window_height, 0, GL_R32I, GL_UNSIGNED_INT, NULL);

  glBindTexture(GL_TEXTURE_2D, 0);*/

  glGenRenderbuffers(1, &color_renderbuffer);
  glBindRenderbuffer( GL_RENDERBUFFER, (GLuint)color_renderbuffer );
  glRenderbufferStorage( GL_RENDERBUFFER, GL_R32F, window_width, window_height );
  glBindRenderbuffer( GL_RENDERBUFFER, 0 );
  
  // en fait je vais utiliser une image opencv pour recuperer le resultat d'openGL
  //cv::Mat img();
  
  
  img.create(window_height, window_width, CV_32FC1);
  
  // un fbo c'est pour ne pas dessiner a l'ecran mais dans un buffer qu'on pourra recuperer ensuite
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  
  // il y a 2 types de fbo. Utiliser une texture permet de l'utiliser facilement par la suite pour re-coloriser 
  // et visualiser le resulata mais est-ce aussi rapide ???
  //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_map, 0);
  //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_renderbuffer);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_renderbuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  // si on utilise le depth il faudra aussi un  buffer pour ca.  
  //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_renderbuffer);

  // allowing point size
  glEnable(GL_PROGRAM_POINT_SIZE);
   
  glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
   
  // Accept fragment if it closer to the camera than the former one
   
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  glfwSetCharCallback(window, character_callback);
  //glfwSetKeyCallback(window, key_callback);
   
   
  programID = LoadShaders( "remimaps.vs", "remimaps.fs" );
  GLint rx =  glGetUniformLocation(programID, "rangex");
  GLint ry = glGetUniformLocation(programID, "rangey");

  // load data
  
  // make opengl model
  
  VertexArrayIDAs = makeModelA(window_width, window_height, 5);
   
  cout << "starting loop REMI "<<N<<endl;

  int frameCount = 0;
  double previousTime = glfwGetTime();

  do {
    // clear before every draw 1
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
    if (VertexArrayIDAs!=0){
      
      glUseProgram(programID);
	  glUniform1f(rx, 2.0);
	  glUniform1f(ry, 2.0);
      
      glBindVertexArray(VertexArrayIDAs[0]);
      glDrawArrays(GL_POINTS, 0, N); //all the vertices
      glBindVertexArray (0);       
      glUseProgram(0);

    }
          
    // Swap buffers
    glfwSwapBuffers(window);
    
    frameCount++;
    //double currentTime = glfwGetTime();
    if (frameCount%60==0){
      double currentTime = glfwGetTime();
      cout << "time to display 60 frames: "<<(currentTime-previousTime) << endl;
      previousTime = glfwGetTime();
    }
    glfwPollEvents();
     
  } // check the esc key
  while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 );
}
 
