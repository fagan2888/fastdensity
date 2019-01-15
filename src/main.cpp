#include <iostream>
#include <chrono>

#include "fastdensity/fastdensity.hpp"

using namespace std;

int main() {
  FFastDensity ff(256,256);
  float x[] = {1,2,3};
  float y[] = {1,2,3};

  auto start = chrono::steady_clock::now();
  ff.add(3, x, y);
  auto end = chrono::steady_clock::now();
  auto diff = end - start;
  cout << chrono::duration <double, nano> (diff).count() << " ns" << endl;
}
