#version 450 core  
layout (location = 0) in vec3 aPosition; 
layout (location = 1) in vec3 aNormal; 
layout (location = 2) in vec2 aTexCoords; 

uniform mat4 cameraMatrix;
uniform mat4 model;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

void main()  {
    TexCoords = aTexCoords;
    Normal = vec3(model * vec4(aNormal, 1.0));
    FragPos = vec3(model * vec4(aPosition, 1.0));


    gl_Position = cameraMatrix * vec4(FragPos, 1.0);
}