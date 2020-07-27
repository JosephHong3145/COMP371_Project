#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 3) in vec2 aTexCoord;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform int currentAxis = 0;
uniform int lightSource = 0;
out vec3 vertexColor;
out vec2 TexCoord;



void main()
{
   if(currentAxis == 1) { vertexColor = vec3(1, 0, 0); }
   else if(currentAxis == 2) { vertexColor = vec3(0, 1, 0); }
   else if(currentAxis == 3) { vertexColor = vec3(0, 0, 1); }
   else { vertexColor = aColor; }
   if(lightSource == 1) { vertexColor = vec3(1, 1, 1); }

   mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;
   gl_Position = modelViewProjection * vec4(aPos, 1.0);
   TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}