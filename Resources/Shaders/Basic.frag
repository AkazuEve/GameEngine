#version 450 core  
layout (location = 0) out vec4 FragColor;  

layout (binding = 0) uniform sampler2D diffuseTexture;

uniform mat4 model;

in vec2 TexCoord;
in vec3 Normal;

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float posterize(float value, int levels) {
    return round(value * levels) / levels;
}

void main()  {
    vec3 outColor;
    vec4 textureSample = texture(diffuseTexture, TexCoord);

    vec3 hsvSample = rgb2hsv(textureSample.rgb);
    vec3 posterizedHsv = vec3(posterize(hsvSample.x, 15), posterize(hsvSample.y, 6), posterize(hsvSample.z, 5));

    vec3 lightAngle = vec3(0.0, 1.0, -1.0);

	FragColor = vec4(hsv2rgb(posterizedHsv), 1.0) * dot(Normal, normalize(lightAngle));
}