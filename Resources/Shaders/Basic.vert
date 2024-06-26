#version 450 core  
layout (location = 0) in vec3 aPosition; 
layout (location = 1) in vec3 aNormal; 
layout (location = 2) in vec2 aTex; 

uniform mat4 cameraMatrix;
uniform mat4 model;

out vec2 TexCoord;
out vec3 Normal;
out vec3 CurrenPositon;

void main()  {
    TexCoord = aTex;
    Normal = vec3(model * vec4(aNormal, 1.0));
    CurrenPositon = vec3(model * vec4(aPosition, 1.0));


    gl_Position = cameraMatrix * vec4(CurrenPositon, 1.0);
}