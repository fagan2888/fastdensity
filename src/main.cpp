#include <iostream>
#include <chrono>
//Added by rémi
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <math.h>
#include <string>
#include <algorithm>
#include <ctime>
#include <omp.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#include <glew.h>
//GLFW
#include <glfw3.h>

#include "fastdensity/fastdensity.hpp"

using namespace std;

/* flobal vars */

unsigned int point_nb = 1000000;
unsigned int seed = 5;
unsigned int width = 1000;
unsigned int height = 600;
unsigned int mapSize = 32;
double stddev = 5.68;


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

/* This fonction is only here as debug purpose. Check if OpenMP work (call it on the main)*/
static void hello_World_OpenMp() {

	unsigned int n_thread = omp_get_max_threads();
	omp_set_num_threads(n_thread);

	#pragma omp parallel for
	for (int i = 0; i < 4; i++) {
		cout << "thread numéros : " << omp_get_thread_num() << endl;
	}
}

/*	return_current_time_and_date_as_string :
 *		Create a string representativ of the date and time
 *  @return :
 *		A string with shape "YYYY-MM-DD-HH:MM:SS"
 */

static string return_current_time_and_date_as_string() {
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	string s(30, '\0');
	strftime(&s[0], s.size(), "%Y-%m-%d_%Hh%Mm%Ss", localtime(&in_time_t));
	s.erase(remove(s.begin(), s.end(), '\0'), s.end());
	return s;
}

/*	Generation_1 :
 *		Generate a tab filled with random points placed in a closed box
 *	@params : 
 *		seed ; the seed wanted for the random number generation, will refresh it
 *  @return :
 *		A 1 dimensional array of int, with all the even indexes are the x value of the point and all the odd indexes are the y value of it
 */

static vector<int> generation_1(unsigned int seeds=seed, bool display=false) 
{
	vector<int> point_Array;
	srand(seeds); //seed of the rng && should reset the seed at it's starting point.

	/* array generation */
	for (unsigned int i = 0; i < point_nb; i++) {
		point_Array.push_back(rand()%(width+1));
		point_Array.push_back(rand()%(height+1));
	}

	/* array display */
	if (display) {
		for (unsigned int i = 0; i < point_nb * 2; i += 2) {
			cout << "le point : (" << point_Array[i] << " ; " << point_Array[i + 1] << ") est dans notre tableau" << endl;
		}
	}
	return point_Array;
}


/*	Generation_2 :
 *		Generate a tab filled with random points following a normal distribution (gaussian) with mean = 0
 *	@params :
 *		seed ; the seed wanted for the random number generation, will refresh it
 *  @return :
 *		A 1 dimensional array of int, with all the even indexes are the x value of the point and all the odd indexes are the y value of it
 */

static vector<int> generation_2(unsigned int seeds=seed, bool display=false)
{
	//normal distribution construction
	//when using n<4 as seed, it generate some patern of 0, don't know why. But work perfectly fine with other seed values
	default_random_engine generator(seeds); //seed of the rng && should reset the seed at it's starting point.
	normal_distribution<double> distribution (0.0,stddev);

	vector<int> point_Array;

	/* array generation */
	for (unsigned int i = 0; i < point_nb; i++) {
		point_Array.push_back(ceil(distribution(generator)));
		point_Array.push_back(ceil(distribution(generator)));
	}

	/* array display */
	if (display) {
		for (unsigned int i = 0; i < point_nb * 2; i += 2) {
			cout << "le point : (" << point_Array[i] << " ; " << point_Array[i + 1] << ") est dans notre tableau" << endl;
		}
	}
	return point_Array;
}

/*
DEBUG FUNCTION
display a square 2 dimensional array given as param as a 1 dimensionnal one on the standard output; work for density maps with mapSize size
*/
template< typename T >
static void display_2Darray(vector<T> tab) {

	cout << endl;

	for (unsigned int i = 0; i < mapSize; i++) {
		for (unsigned int j = 0; j < mapSize; j++) {
			cout << tab[i*mapSize+j] << " | ";
		}
		cout << endl;
	}
}

template< typename T >
static boolean isEqual_2DArray(vector<T> tab1, vector<T> tab2) {
	for (unsigned int i = 0; i < mapSize; i++) {
		for (unsigned int j = 0; j < mapSize; j++) {
			if (tab1[i*mapSize+j] != tab2[i*mapSize+j]) { return false; }
		}
	}
	return true;
}

/*
 * Compute_densityMap_CPU_simple_1Tab :
 * @params :
 *	tab : A 1 dimensional array of number, with all the even indexes are the x value of the point and all the odd indexes are the y value of it
 *  maxX, maxY, minX, minY : the  global extremums of the tab array
 * @return :
 *	the densityMap
 *
 * compute_densityMap_CPU_multThread_1Tab do the same with a bit of multi threading
 */
template< typename T >
vector<unsigned int> compute_densityMap_CPU_simple_1Tab(vector<T>tab, T maxX, T minX, T maxY, T minY) {
	vector<unsigned int> densityMap(mapSize*mapSize);
	T Xrange = maxX - minX;
	T Yrange = maxY - minY;
	unsigned int tempX, tempY;
	for (unsigned int i = 0; i < point_nb*2; i+=2) {
		tab[i] = tab[i] + minX * (-1);
		tab[i + 1] = tab[i + 1] + minY * (-1);
		tempX = round(((double)tab[i] / (double)Xrange) * (mapSize-1));
		tempY = round(((double)tab[i+1] /(double)Yrange) * (mapSize-1));
		densityMap[tempX*mapSize + tempY]++;
	}
	return densityMap;
}

template< typename T >
vector<unsigned int> compute_densityMap_CPU_multThread_1Tab(vector<T>tab, T maxX, T minX, T maxY, T minY) {
	vector<unsigned int> densityMap(mapSize*mapSize);
	T Xrange = maxX - minX;
	T Yrange = maxY - minY;
	#pragma omp parallel for
	for (int i = 0; i < point_nb*2; i += 2) {
		tab[i] = tab[i] + minX * (-1);
		tab[i + 1] = tab[i + 1] + minY * (-1);
		tab[i] = round(((double)tab[i] / (double)Xrange) * (mapSize - 1));
		tab[i + 1] = round(((double)tab[i + 1] / (double)Yrange) * (mapSize - 1));
	}
	#pragma omp critical
	for (unsigned int i = 0; i < point_nb*2; i += 2) {
		densityMap[tab[i] * mapSize + tab[i+1]]++;
	}
	return densityMap;
}

/*
 * Compute_densityMap_CPU_simple_2Tabs :
 * @params :
 *	tabX : A 1 dimensional array of number containing all the X axis values for points
 *  tabY : A 1 dimensional array of number containing all the Y axis values for points
 *  maxX, maxY, minX, minY : the  global extremums of the tab array
 *  mapSize : the size of density map wanted, (if 256 then the density map will generated will be 256^2
 * @return :
 *	the densityMap
 *
 * compute_densityMap_CPU_multThread_2Tabs do the same with a bit of multi threading
 */
template< typename T >
vector<unsigned int> compute_densityMap_CPU_simple_2Tabs(vector<T>tabX, vector<T>tabY, T maxX, T minX, T maxY, T minY) {
	vector<unsigned int> densityMap(mapSize*mapSize);
	T Xrange = maxX - minX;
	T Yrange = maxY - minY;
	unsigned int tempX, tempY;
	for (unsigned int i = 0; i < point_nb; i ++) {
		tabX[i] = tabX[i] + minX * (-1);
		tabY[i] = tabY[i] + minY * (-1);
		tempX = round(((double)tabX[i] / (double)Xrange) * (mapSize - 1));
		tempY = round(((double)tabY[i] / (double)Yrange) * (mapSize - 1));
		densityMap[tempX * mapSize + tempY]++;
	}
	return densityMap;
}

template< typename T >
vector<unsigned int> compute_densityMap_CPU_multThread_2Tabs(vector<T>tabX, vector<T>tabY, T maxX, T minX, T maxY, T minY) {
	vector<unsigned int> densityMap(mapSize*mapSize);
	T Xrange = maxX - minX;
	T Yrange = maxY - minY;
	#pragma omp parallel for
	for (int i = 0; i < point_nb; i ++) {
		tabX[i] = tabX[i] + minX * (-1);
		tabY[i] = tabY[i] + minY * (-1);
		tabX[i] = round(((double)tabX[i] / (double)Xrange) * (mapSize - 1));
		tabY[i] = round(((double)tabY[i] / (double)Yrange) * (mapSize - 1));
	}
	#pragma omp critical
	for (int i = 0; i < point_nb; i++) {
		densityMap[tabX[i] * mapSize + tabY[i]]++;
	}
	return densityMap;
}

template< typename T >
static int GLFW_testing_zone(vector<T>tab) {

	//initiate the Shaders with opengl 3.3

	const char *vertexShaderSourcePoints = "#version 330 \n"
		"layout (location = 0) in vec2 aPos;\n"
		"uniform vec2 ranges;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x / ranges.x, aPos.y / ranges.y, 0.0, 1.0);\n"
		"}\0";
	const char *fragmentShaderSource = "#version 330 \n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
		"}\n\0";
	
	/* opengl and GLFW initialisation */

	if (!glfwInit()) { cout << "GLFW failed at initialisation" << endl; return -1; }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	GLenum errors;

	// set up a window

	GLFWwindow* window = glfwCreateWindow(600, 320, "Hello World", NULL, NULL);
	if (!window) {
		glfwTerminate();
		cout << "GLFW window creation failed" << endl;
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//Initialisation of GLEW
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		glfwTerminate();
		return -1;
	}
	fprintf(stdout, "Status: Using GLEW %s \n\n", glewGetString(GLEW_VERSION));

	// build and compile our shader program
	// ------------------------------------
	// vertex shader
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSourcePoints, NULL);
	glCompileShader(vertexShader);
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (success != GL_TRUE)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		return -1;
	}
	else {
		std::cout << "SHADER::VERTEX::COMPILATION_SUCCESS\n" << std::endl;
	}
	// fragment shader
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (success != GL_TRUE)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		return -1;
	}
	else {
		std::cout << "SHADER::FRAGMENT::COMPILATION_SUCCESS\n" << std::endl;
	}
	// link shaders
	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (success != GL_TRUE) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		return -1;
	}
	else {
		std::cout << "SHADER::PROGRAM::LINKING_SUCCESS\n" << std::endl;
	}
	// once the link is done, we don't need the shaders anymore, so we delete them.
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	//determining the min X & Y, the max X & Y of the given array and creating the corresponding vertices.
	/*vector<float> vertexPositions;
	int maxY = tab[0], maxX = tab[0], minY = tab[0], minX = tab[0];
	for (unsigned int i = 0; i < point_nb * 2; i += 2) {
		if (tab[i] < minX) { minX = tab[i]; }
		else if (tab[i] > maxX) { maxX = tab[i]; }
		if (tab[i + 1] < minY) { minY = tab[i + 1]; }
		else if (tab[i + 1] > maxY) { maxY = tab[i + 1]; }
	}
	float rangeX = maxX - minX;
	float rangeY = maxY - minY;

	for (unsigned int i = 0; i < point_nb * 2; i += 2) {
		vertexPositions.push_back(float(tab[i] + minX * (-1)));
		vertexPositions.push_back(float(tab[i+1] + minY * (-1)));
	}*/

	//testing READPIXELS
	vector<float> vertexPositions(32*32);
	for (unsigned int i = 0; i < 32; i++) {
		for (unsigned int j = 0; j < 32; j++) {
			vertexPositions.push_back(i);
			vertexPositions.push_back(j);
		}
	}
	float rangeX = 32.0;
	float rangeY = 32.0;
	// update shader uniform for normalizing points given in
	int vertexColorLocation = glGetUniformLocation(shaderProgram, "ranges");
	if (vertexColorLocation == -1) { std::cout << "couldn't link uniform vertex and programs\n" << std::endl; return -1; }
	else { std::cout << "link between uniform vertex and programs done\n" << std::endl; }
	glUseProgram(shaderProgram);
	glUniform2f(vertexColorLocation, rangeX, rangeY);

	// VBO = Vertex Buffer Object ___ VAO = Vertex Array Object
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexPositions.size() * sizeof(GLfloat), &vertexPositions[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	/* Creating and binding a new framebuffer for off screen rendering */
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	//creating a render buffer 
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, mapSize*2, mapSize*2);
	if ((errors = glGetError()) != GL_NO_ERROR) { std::cout << "erros at RenderBufferStorage for framebuffer : " << gluErrorString(errors) << std::endl; }
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT8, GL_RENDERBUFFER, rbo);
	if ((errors = glGetError()) != GL_NO_ERROR) { std::cout << "erros at glFramebufferRenderbuffer for framebuffer : " << gluErrorString(errors) << std::endl; }
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){ std::cout << "unsuccesfull completion in off screen rendering buffer creation\n" << std::endl; return -1; }
	else { std::cout << "succesfull completion in off screen rendering buffer creation\n" << std::endl; }

	/* rendering the data */
	// Render here
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// draw
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	//glDrawArrays(GL_POINTS, 0, point_nb*2);
	//glPointSize(10.0);
	glDrawArrays(GL_POINTS, 0, mapSize*mapSize * 2);
	glBindVertexArray(0); // unbind the vertex array

	/* Recovering the pixels from the buffer*/
	unsigned char data[32 * 32 * 1];
	glReadPixels(0, 0, mapSize, mapSize, GL_RED, GL_UNSIGNED_INT, data);

	// display the result of off rendering
	vector<unsigned int> temp(32 * 32 * 1);
	//unsigned int conversion = 4294967295; // 2^32-1
	for (int j = 0; j < mapSize; j++) {
		for (int i = 0; i < mapSize; i++) {
			temp[(mapSize - 1 - j)*mapSize + i] = data[j*mapSize + i];
		}
	}
	display_2Darray(temp);
	temp.clear();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fbo);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window)) {

		// Render here
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// draw
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
		//glPointSize(10.0);
		//glDrawArrays(GL_POINTS, 0, point_nb*2);
		glDrawArrays(GL_POINTS, 0, mapSize*mapSize * 2); //mapSize*mapSize*2
		glBindVertexArray(0); // unbind the vertex array

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}
	// de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;
}

/*	
 *  one_entry_benchmark :
 *		Create an entry in a CSV file that contain the whole information about the execution of one type execution
 *		For the Moment, just getting informations for creating an array of point following one of both génération. Then making some calulation with the array,
 *		finally store the result in a CSV file.
 *		In future should only take the pre calculated array as paramaters.
 *	@params :
 *		filename ; the file name of CSV
 *		point_nb ; number of point inside the array
 *		seed ; the seed used for generating the array
 *		width & height ; referes to generation 1 params, they got -1 as default value in case generation2 is used instead of generation1
 *		stddev ; the standard deviation that'll follow the distribution (for generation2)
 *		entry_type ; an integer used as identifier for the type of algorithm used for generating the density map (1 => normal / 2 => Multi threaded / 3 => GPU )
 */

static void one_entry_benchmark(string filename, unsigned int entry_type, unsigned int number, bool two_tabs = false) {
	ofstream file_pointer;
	/* opens an existing csv file or creates a new file. Every output operation will be push at the end of file.
		flag in : Open for input operations (reading essentially);
		flag out : Open for output operations.
		flag app : 	All output operations are performed at the end of the file, appending the content to the current content of the file.
	*/
	file_pointer.open(filename, ios::app);

	long totalTime = 0;
	vector<int> point_Array;
	vector<int> point_ArrayX;
	vector<int> point_ArrayY;
	vector<unsigned int> densityMap;
	unsigned int seeds = seed;
	string generation = "";
	string entry_name = "";

	for (unsigned int nb = 0; nb < number; nb++) {
		point_ArrayX.clear();
		point_ArrayY.clear();

		/* Changing the seed for every loop, used for forcing new data on cache.
			I choose this way of changing the seed for keeping the same seed for future calcul process.
			Will probably need a preload of every points on the cache for equalise the calcul process.
		*/
		seeds = seeds + 1;
		if (!two_tabs) {
			point_Array = generation_2(seeds);
			generation = "2";
		}
		else {
			point_Array = generation_1(seeds);
			generation = "1";
		}

		//determining the min X & Y, the max X & Y
		int maxY = point_Array[0], maxX = point_Array[0], minY = point_Array[0], minX = point_Array[0];
		for (unsigned int i = 0; i < point_nb * 2; i += 2) {
			if (point_Array[i] < minX) { minX = point_Array[i]; }
			else if (point_Array[i] > maxX) { maxX = point_Array[i]; }
			if (point_Array[i + 1] < minY) { minY = point_Array[i + 1]; }
			else if (point_Array[i + 1] > maxY) { maxY = point_Array[i + 1]; }
		}

		if (two_tabs) {
			for (unsigned int i = 0; i < point_nb * 2; i += 2) {
				point_ArrayX.push_back(point_Array[i]);
				point_ArrayY.push_back(point_Array[i + 1]);
			}
		}

		switch (entry_type){

			case 1 :
				entry_name = "CPU_densityMap_simple";
				if (two_tabs) {
					auto start = chrono::steady_clock::now();
					densityMap = compute_densityMap_CPU_simple_2Tabs(point_ArrayX, point_ArrayY, maxX, minX, maxY, minY);
					auto end = chrono::steady_clock::now();
					auto diff = end - start;
					auto elapsed_time = chrono::duration <long, nano>(diff).count();
					totalTime += elapsed_time;
					//Insert data to file filename
					std::cout << entry_name << to_string(nb) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << elapsed_time << endl;
					file_pointer << entry_name << to_string(nb) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << elapsed_time << "\n";
				}
				else {
					auto start = chrono::steady_clock::now();
					densityMap = compute_densityMap_CPU_simple_1Tab(point_Array, maxX, minX, maxY, minY);
					auto end = chrono::steady_clock::now();
					auto diff = end - start;
					auto elapsed_time = chrono::duration <long, nano>(diff).count();
					totalTime += elapsed_time;
					//Insert data to file filename
					std::cout << entry_name << to_string(nb) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << elapsed_time << endl;
					file_pointer << entry_name << to_string(nb) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << elapsed_time << "\n";
				}
				break;

			case 2 :
				entry_name = "CPU_densityMap_multThread";
				if (two_tabs) {
					auto start = chrono::steady_clock::now();
					densityMap = compute_densityMap_CPU_multThread_2Tabs(point_ArrayX, point_ArrayY, maxX, minX, maxY, minY);
					auto end = chrono::steady_clock::now();
					auto diff = end - start;
					auto elapsed_time = chrono::duration <long, nano>(diff).count();
					totalTime += elapsed_time;
					//Insert data to file filename
					std::cout << entry_name << to_string(nb) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << elapsed_time << endl;
					file_pointer << entry_name << to_string(nb) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << elapsed_time << "\n";
				}
				else {
					auto start = chrono::steady_clock::now();
					densityMap = compute_densityMap_CPU_multThread_1Tab(point_Array, maxX, minX, maxY, minY);
					auto end = chrono::steady_clock::now();
					auto diff = end - start;
					auto elapsed_time = chrono::duration <long, nano>(diff).count();
					totalTime += elapsed_time;
					//Insert data to file filename
					std::cout << entry_name << to_string(nb) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << elapsed_time << endl;
					file_pointer << entry_name << to_string(nb) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << elapsed_time << "\n";
				}
				break;

			case 3 :
				entry_name = "GPU_densityMap";
				if (two_tabs) {
					auto start = chrono::steady_clock::now();
					//densityMap = compute_densityMap_GPU_2Tabs(point_ArrayX, point_ArrayY, maxX, minX, maxY, minY);
					auto end = chrono::steady_clock::now();
					auto diff = end - start;
					auto elapsed_time = chrono::duration <long, nano>(diff).count();
					totalTime += elapsed_time;
					//Insert data to file filename
					std::cout << entry_name << to_string(nb) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << elapsed_time << endl;
					file_pointer << entry_name << to_string(nb) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << elapsed_time << "\n";
				}
				else {
					auto start = chrono::steady_clock::now();
					//densityMap = compute_densityMap_GPU_1Tab(point_Array, maxX, minX, maxY, minY);
					auto end = chrono::steady_clock::now();
					auto diff = end - start;
					auto elapsed_time = chrono::duration <long, nano>(diff).count();
					totalTime += elapsed_time;
					//Insert data to file filename
					std::cout << entry_name << to_string(nb) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << elapsed_time << endl;
					file_pointer << entry_name << to_string(nb) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << elapsed_time << "\n";
				}
				break;
		}
		//uncomment this if you want to display the generated density map on the shell
		//display_2Darray(densityMap);
	}

	totalTime = totalTime / number;

	std::cout << "\n" << "Mean_On_" << to_string(number) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << totalTime << endl;
	file_pointer << "\n" << "Mean_On_" << to_string(number) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << totalTime << "\n" << "\n";

	file_pointer.close();
}

/* Used for launching every entry of the benchmark */

static void create_Benchmark(string filename, unsigned int number, bool two_tabs = false) {

	string CPU = "";
	string CPU_nb = "";
	string Thread_nb = to_string(omp_get_max_threads());
	string RAM = "";
	string GPU = "";

	/* Get computeur components informations */
	#ifdef _WIN32
	SYSTEM_INFO siSysInfo;
	ULONGLONG ram_buffer;
	// Copy the hardware information to the SYSTEM_INFO structure. 
	GetSystemInfo(&siSysInfo);
	GetPhysicallyInstalledSystemMemory(&ram_buffer);

	switch (siSysInfo.wProcessorArchitecture)
	{
	case 9:
		CPU = "x64 AMD or Intel"; break;
	case 5:
		CPU = "ARM"; break;
	case 12:
		CPU = "ARM64"; break;
	case 6:
		CPU = "Intel titanium-based"; break;
	case 0:
		CPU = "x86"; break;
	case 0xffff:
		CPU = "Unknown architecture"; break;
	}

	CPU_nb = to_string(siSysInfo.dwNumberOfProcessors);

	RAM = to_string(ram_buffer) + "bytes";

	#endif
	#ifdef _unix_
		//TODO
	#endif
	#ifdef _MACH_
		//TODO
	#endif


	/* File initialisation*/

	ifstream file_pointer_read;
	file_pointer_read.open(filename, ios::in);
	string reading;
	getline(file_pointer_read, reading);
	file_pointer_read.close();
	ofstream file_pointer;
	/* opens an existing csv file or creates a new file. Every output operation will be push at the end of file.
		flag in : Open for input operations (reading essentially);
		flag out : Open for output operations.
		flag app : 	All output operations are performed at the end of the file, appending the content to the current content of the file.
	*/
	file_pointer.open(filename, ios::out | ios::app);
	if (!file_pointer)
	{
		cout << "Error in creating file!!! " << filename << endl;
	}
	if (reading == "") {
		file_pointer << "the computeur informations are : \n";
		file_pointer << "CPU_Architecture : " << CPU << "\n";
		file_pointer << "CPU_number : " << CPU_nb << "\n";
		file_pointer << "Thread_number : " << Thread_nb << "\n";
		file_pointer << "RAM capacity : " << RAM << "\n";
		file_pointer << "GPU_information : " << GPU << "\n";
		file_pointer << "\n";
		file_pointer << "Benchmark Name, number of points, seed used, stddev, width, height, generation used, densityMap size, elapsed_time (ns), \n";
	}
	else {
		cout << "Error, the file " << filename << "wasn't empty" << endl;
	}

	file_pointer.close();

	/* launch every entry */
	one_entry_benchmark(filename, 1, number, two_tabs);
	one_entry_benchmark(filename, 2, number, two_tabs);
	one_entry_benchmark(filename, 3, number, two_tabs);
	one_entry_benchmark(filename, 1, number, !two_tabs);
	one_entry_benchmark(filename, 2, number, !two_tabs);
	one_entry_benchmark(filename, 3, number, !two_tabs);
}

boolean full_test_tab_equality() {
	vector<int> point_Array;
	vector<int> point_ArrayX;
	vector<int> point_ArrayY;
	vector<unsigned int> densityMap_reference;
	vector<unsigned int> densityMap_tested;

	point_Array = generation_2();

	//determining the min X & Y, the max X & Y
	int maxY = point_Array[0], maxX = point_Array[0], minY = point_Array[0], minX = point_Array[0];
	for (unsigned int i = 0; i < point_nb * 2; i += 2) {
		if (point_Array[i] < minX) { minX = point_Array[i]; }
		else if (point_Array[i] > maxX) { maxX = point_Array[i]; }
		if (point_Array[i + 1] < minY) { minY = point_Array[i + 1]; }
		else if (point_Array[i + 1] > maxY) { maxY = point_Array[i + 1]; }
	}

	densityMap_reference = compute_densityMap_CPU_simple_1Tab(point_Array, maxX, minX, maxY, minY);
	densityMap_tested = compute_densityMap_CPU_multThread_1Tab(point_Array, maxX, minX, maxY, minY);
	if (!isEqual_2DArray(densityMap_reference, densityMap_tested)) { cout << "multi threaded density with 1 tab generation isn't equal to basic generation" << endl; display_2Darray(densityMap_reference); display_2Darray(densityMap_tested); return false; }
	/*densityMap_tested = compute_densityMap_GPU_1Tab(point_Array, maxX, minX, maxY, minY);
	if (!isEqual_2DArray(densityMap_reference, densityMap_tested)) { cout << "GPU density with 1 tab generation isn't equal to basic generation" << endl; return false; }*/

	point_Array = generation_1();

	//determining the min X & Y, the max X & Y
	maxY = point_Array[0];
	maxX = point_Array[0];
	minY = point_Array[0];
	minX = point_Array[0];
	for (unsigned int i = 0; i < point_nb * 2; i += 2) {
		if (point_Array[i] < minX) { minX = point_Array[i]; }
		else if (point_Array[i] > maxX) { maxX = point_Array[i]; }
		if (point_Array[i + 1] < minY) { minY = point_Array[i + 1]; }
		else if (point_Array[i + 1] > maxY) { maxY = point_Array[i + 1]; }
	}

	for (unsigned int i = 0; i < point_nb * 2; i += 2) {
		point_ArrayX.push_back(point_Array[i]);
		point_ArrayY.push_back(point_Array[i + 1]);
	}
	densityMap_reference = compute_densityMap_CPU_simple_2Tabs(point_ArrayX, point_ArrayY, maxX, minX, maxY, minY);
	densityMap_tested = compute_densityMap_CPU_multThread_2Tabs(point_ArrayX, point_ArrayY, maxX, minX, maxY, minY);
	if (!isEqual_2DArray(densityMap_reference, densityMap_tested)) { cout << "multi threaded density with 2 tabs generation isn't equal to basic generation" << endl; display_2Darray(densityMap_reference); display_2Darray(densityMap_tested);  return false; }
	/*densityMap_tested = compute_densityMap_GPU_2Tabs(point_ArrayX, point_ArrayY, maxX, minX, maxY, minY);
	if (!isEqual_2DArray(densityMap_reference, densityMap_tested)) { cout << "GPU density with 2 tabs generation isn't equal to basic generation" << endl; return false; }*/

	return true;
}


int main() {
  FFastDensity ff(256,256);
  float x[] = {1,2,3};
  float y[] = {1,2,3};

  //hello_World_OpenMp();

  if (!full_test_tab_equality()) {
	  cout << "-----------------------------------------" << endl
		   << " /!\\ Equality tests aren't validated /!\\ " << endl
		   << "-----------------------------------------" << endl;
  }

  /* ALL_PARAMS of the speed test. */
  cout << "the test will have the following params :" << endl;
  cout << " number of point to generate : " << point_nb << endl;
  cout << "seed for the tests : " << seed << endl;
  cout << "window width : " << width << endl;
  cout << "window height : " << height << endl;
  cout << "standard deviation for the normal distribution : " << stddev << endl;
  /*int answer = -1;
  bool display;
  cout << "display Y/N (resp 1/0) :" << endl;
  cin >> answer;
  answer == 0 ? display = false : display = true;*/
  string bench_name;


  /* getting the date for naming the file */
  stringstream ss;
  ss << "bench_" << return_current_time_and_date_as_string() << ".csv";
  string filename = ss.str();
  //create_Benchmark(filename, 100);
  
  /* opengl and GLFW tests */
  if (GLFW_testing_zone(generation_2()) != 0) { cout << "problems with GLFW stuff" << endl; return -1; }

  return 0;
}
