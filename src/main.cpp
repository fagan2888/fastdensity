#include <iostream>
#include <chrono>
//Added by rémi
#include <cstdlib>
#include <vector>
#include <random>
#include <math.h>

#include "fastdensity/fastdensity.hpp"

#include <GLFW/glfw3.h>

using namespace std;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
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

static vector<int> generation_1(int width, int height, unsigned int n, int seed, bool display) 
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

static vector<int> generation_2(double stddev, int n, int seed, bool display)
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
  int window_width = 0;
  cout << "window width :" << endl;
  cin >> window_width;
  int window_height = 0;
  cout << "window height :" << endl;
  cin >> window_height;
  int answer = -1;
  bool display;
  cout << "display Y/N (resp 1/0) :" << endl;
  cin >> answer;
  answer == 0 ? display = false : display = true;
  int stddev = 0;
  cout << "standard deviation for the normal distribution" << endl;
  cin >> stddev;

  //vector<int> temp1 = generation_1(window_width, window_height, points_nb, seed, display);
  vector<int> temp2 = generation_2(stddev, points_nb, seed, display);

  auto start = chrono::steady_clock::now();
  ff.add(3, x, y);
  auto end = chrono::steady_clock::now();
  auto diff = end - start;
  cout << chrono::duration <double, nano> (diff).count() << " ns" << endl;

  if (!glfwInit())
    return -1;

  GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Hello World", NULL, NULL);
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
