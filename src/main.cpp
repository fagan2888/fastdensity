#include <iostream>
#include <chrono>

#include "fastdensity/fastdensity.hpp"

#include <GLFW/glfw3.h>

using namespace std;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main() {
  FFastDensity ff(256,256);
  float x[] = {1,2,3};
  float y[] = {1,2,3};

  auto start = chrono::steady_clock::now();
  ff.add(3, x, y);
  auto end = chrono::steady_clock::now();
  auto diff = end - start;
  cout << chrono::duration <double, nano> (diff).count() << " ns" << endl;

  if (!glfwInit())
    return -1;

  GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
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
