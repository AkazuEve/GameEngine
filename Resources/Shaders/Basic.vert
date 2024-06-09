#version 450 core  
layout (location = 0) in vec3 aPosition; 
layout (location = 1) in vec3 aNormal; 
layout (location = 2) in vec2 aTex; 

uniform mat4 cameraMatrix;
uniform mat4 model;

out vec2 TexCoord;
out vec3 Normal;

void main()  {
    gl_Position = cameraMatrix * model * vec4(aPosition, 1.0); 
    TexCoord = aTex;
    Normal = aNormal;
}