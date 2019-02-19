#version 330 core

smooth in float depth;
smooth in vec4 colort;

//out vec4 color;
layout(location = 0) out vec4 frag_color;
//layout(location = 0) out uint frag_color;

void main(){
  float rho =0.2;
  float tworhotwo = 2*rho*rho;
  float d0 = dot(gl_PointCoord-0.5,gl_PointCoord-0.5);
  float dx = gl_PointCoord.x-0.5;
  float dy = gl_PointCoord.y-0.5;
  float dsq = dx*dx+dy*dy;
  //float d1 = sqrt(dsq);
  float d2 = exp(-dsq/tworhotwo);
  
  if(d0>0.25)
    discard;
  else
    frag_color = vec4(1.0, 1.0, 1.0, d2);

  //color.g = 1.0;
  //color.b = 1.0;
  //color.a = 1.0-d1*2;
  //color.a = (0.1/(rho*sqrt(3.14)))*d2;
  //gl_FragColor=vec4(luminance,luminance,luminance,1.);

  //gl_FragDepth = depth/2+d0/12.0;
  //gl_FragDepth = min(depth/2+d0/12.0, gl_FragDepth+d0/12.0);
}
