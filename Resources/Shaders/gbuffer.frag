#version 450 core
layout (location = 0) out vec4 gAlbedo;
layout (location = 1) out vec4 gPosition;
layout (location = 2) out vec4 gNormal;

layout (binding = 0) uniform sampler2D diffuseTexture;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

void main()
{    
    // and the diffuse per-fragment color
    gAlbedo = texture(diffuseTexture, TexCoords);
    // store the fragment position vector in the first gbuffer texture
    gPosition = vec4(FragPos, 1.0);
    // also store the per-fragment normals into the gbuffer
    gNormal = vec4(normalize(Normal), 1.0);
}  