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

#include "fastdensity/fastdensity.hpp"

#include <GLFW/glfw3.h>

using namespace std;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

/*	Generation_1 :
 *		Create a string representativ of the date and time
 *  @return :
 *		A string with shape "YYYY-MM-DD-HH:MM:SS"
 */

static string return_current_time_and_date_as_string() {
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	string s(30, '\0');
	strftime(&s[0], s.size(), "%Y-%m-%d-%H:%M:%S", localtime(&in_time_t));
	s.erase(remove(s.begin(), s.end(), '\0'), s.end());
	cout << s << endl;
	return s;
}

/*	Generation_1 :
 *		Generate a tab filled with random points placed in a closed box
 *	@params : 
 *		width & heigth ; the limit of the box where points will be generate
 *		n ; number of points that should be generated
 *		seed ; the seed wanted for the random number generation, will refresh it
 *  @return :
 *		A 1 dimensional array of int, with all the even indexes are the x value of the point and all the odd indexes are the y value of it
 */

static vector<int> generation_1(unsigned int width, unsigned int height, unsigned int n, int seed, bool display=false) 
{
	vector<int> point_Array;
	srand(seed); //seed of the rng && should reset the seed at it's starting point.

	/* array generation */
	for (int i = 0; i < n*2; i += 2) {
		point_Array.push_back(rand()%(width+1));
		point_Array.push_back(rand()%(height+1));
	}

	/* array display */
	if (display) {
		for (int i = 0; i < n * 2; i += 2) {
			cout << "le point : (" << point_Array[i] << " ; " << point_Array[i + 1] << ") est dans notre tableau" << endl;
		}
	}
	return point_Array;
}


/*	Generation_2 :
 *		Generate a tab filled with random points following a normal distribution (gaussian) with mean = 0
 *	@params :
 *		stddev ; the standard deviation that'll follow the distribution
 *		n ; number of points that should be generated
 *		seed ; the seed wanted for the random number generation, will refresh it
 *  @return :
 *		A 1 dimensional array of int, with all the even indexes are the x value of the point and all the odd indexes are the y value of it
 */

static vector<int> generation_2(double stddev, unsigned int n, int seed, bool display=false)
{
	//normal distribution construction
	default_random_engine generator(seed); //seed of the rng && should reset the seed at it's starting point.
	normal_distribution<double> distribution (0.0,stddev);

	vector<int> point_Array;

	/* array generation */
	for (int i = 0; i < n * 2; i += 2) {
		point_Array.push_back(ceil(distribution(generator)));
		point_Array.push_back(ceil(distribution(generator)));
	}

	/* array display */
	if (display) {
		for (int i = 0; i < n * 2; i += 2) {
			cout << "le point : (" << point_Array[i] << " ; " << point_Array[i + 1] << ") est dans notre tableau" << endl;
		}
	}
	return point_Array;
}

/*
 * For the moment this is an empty function, will change in future
 */
template< typename T >
static void TODO_fun(vector<T>tab) {

}

/*	@temporary
 *  add_Benchmark :
 *		Create an entry in a CSV file that contain the whole information about the execution of a Benchmark
 *		For the Moment, just getting informations for creating an array of point following one of both génération. Then making some calulation with the array, 
 *		finally store the result in a CSV file.
 *		In future should only take the pre calculated array as paramaters.
 *	@params :
 *		filename ; the file name of CSV
 *		name ; name that should be given to the CSV's entry corresponding to the benchmark
 *		n ; number of point inside the array
 *		seed ; the seed used for generating the array
 *		width & height ; referes to generation 1 params, they got -1 as default value in case generation2 is used instead of generation1
 *		stddev ; the standard deviation that'll follow the distribution (for generation2)
 */

static void add_Benchmark(string name, int n, int seed, double stddev, int width = -1, int height = -1)
{
	vector<int> point_Array;

	if (width == -1 && height == -1) {
		point_Array = generation_2(stddev, n, seed);
	}
	else {
		point_Array = generation_1(width, height, n, seed);
	}

	auto start = chrono::steady_clock::now();
	TODO_fun(point_Array);
	auto end = chrono::steady_clock::now();
	auto diff = end - start;
	auto elapsed_time = chrono::duration <double, nano> (diff).count();

	/* getting the date for naming the file */
	string filename = return_current_time_and_date_as_string();
	filename = "bench_"+filename + ".csv";
	cout << filename << endl;

	ofstream file_pointer;
	/* opens an existing csv file or creates a new file. Every output operation will be push at the end of file.
		flag out : Open for output operations.
		flag app : 	All output operations are performed at the end of the file, appending the content to the current content of the file.
	*/
	file_pointer.open(filename, ios::out | ios::app);
	if (!file_pointer)
	{
		cout << "Error in creating file!!! " << filename <<endl;
	}
	//Insert data to file filename
	if (width == -1 && height == -1) {
		file_pointer << (string)name << ", " << n << ", " << seed << ", " << stddev << ", " << "not used" << ", " << "not used" << ", " << "generation2" << ", " << elapsed_time << "\n";
	}
	else {
		file_pointer << name << ", " << n << ", " << seed << ", " << stddev << ", " << "not used" << ", " << "not used" << ", " << "generation2" << ", " << elapsed_time << "\n";
	}

}

int main() {
  FFastDensity ff(256,256);
  float x[] = {1,2,3};
  float y[] = {1,2,3};

  /* ALL_PARAMS of the speed test. */
  cout << "the test will have the following params :" << endl;
  int points_nb=0;
  cout << " number of point to generate" << endl;
  cin >>  points_nb;
  int seed = 1;
  cout << "seed for the tests" << endl;
  cin >>  seed;
  /*int window_width = 0;
  cout << "window width :" << endl;
  cin >> window_width;
  int window_height = 0;
  cout << "window height :" << endl;
  cin >> window_height;
  int answer = -1;
  bool display;
  cout << "display Y/N (resp 1/0) :" << endl;
  cin >> answer;
  answer == 0 ? display = false : display = true;*/
  int stddev = 0;
  cout << "standard deviation for the normal distribution :" << endl;
  cin >> stddev;

  /*vector<int> temp1 = generation_1(window_width, window_height, points_nb, seed, display);
  vector<int> temp2 = generation_2(stddev, points_nb, seed, display);*/

  add_Benchmark("test", points_nb, seed, stddev);

  /*auto start = chrono::steady_clock::now();
  ff.add(3, x, y);
  auto end = chrono::steady_clock::now();
  auto diff = end - start;
  cout << chrono::duration <double, nano> (diff).count() << " ns" << endl;
  */
  if (!glfwInit())
    return -1;

  GLFWwindow* window = glfwCreateWindow(640, 360, "Hello World", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwSetKeyCallback(window, key_callback);
  glfwMakeContextCurrent(window);
  /* Get opengl resources from now */

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
