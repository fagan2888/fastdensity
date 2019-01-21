#version 410 core

//Entrees
layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec4 vertexColor;

// Sortie
smooth out vec4  colort;
smooth out float depth;


void main(){
  gl_Position  = vec4(vertexPosition,0.0, 1.0);
  depth        = 1.0;
  colort       = vertexColor;
  gl_PointSize = 10.0f;
}
