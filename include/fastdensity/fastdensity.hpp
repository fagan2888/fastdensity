// -*- mode: c++; -*-
#ifndef fastdensity_hpp
#define fastdensity_hpp

#include <cstring>


template <typename T>
struct FastDensity {
  int _width;
  int _height;
  T _xmin;
  T _xscale;
  T _ymin;
  T _yscale;
  unsigned int * _density;
  FastDensity(unsigned int width, unsigned int height, 
              T xmin = 0, T  xscale = 1, T  ymin = 0, T yscale = 1) 
    : _width(width),
      _height(height),
      _xmin(xmin),
      _xscale(xscale), 
      _ymin(ymin),
      _yscale(yscale),
      _density(new unsigned int[width*height])
  {}

  void clear() {
    std::memset(_density, 0, _width*_height*sizeof(unsigned int));
  }
  
  void add(int n, const T * x, const T * y) {
    while (n-- >= 0) {
      int X = int((*x - _xmin) * _xscale);
      int Y = int((*y - _ymin) * _yscale);
      if (X >= 0 && X < _width && Y >= 0 && Y < _height)
        _density[Y*_width+X]++;
    }
  }

  void subtract(int n, const T * x, const T * y) {
    while (n-- >= 0) {
      int X = int((*x - _xmin) * _xscale);
      int Y = int((*y - _ymin) * _yscale);
      if (X >= 0 && X < _width && Y >= 0 && Y < _height)
        _density[Y*_width+X]--;
    }
  }
};

typedef FastDensity<float>  FFastDensity;
typedef FastDensity<double> DFastDensity;


#endif

