#version 330 core

//Entrees
layout(location = 0) in vec2 vertexPosition;

//params
uniform float rangex; 
uniform float rangey; 

// Sortie

void main(){
  gl_Position  = vec4(vertexPosition[0], vertexPosition[1] ,0.0, 1.0);
  gl_PointSize = 1.0f;
}
