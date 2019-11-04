#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2DArray texSampler;

layout(location = 0) in vec3 fragTexCoord;

layout(location = 0) out vec4 outColor;

#define PI 3.14159265
// #define PI 180.0

void main() {
    vec3 texCoord = fragTexCoord;
    // float pi = 180.0;
    // texCoord.y /= 2.0 * sin(PI / 2.0 + 480.0 * PI);
    // float x = round(1.2);
    // texCoord.y = float(1);
    // texCoord.y /= 2.0 * sin(pi / 2.0);
    // texCoord.y /= 2.0;

    // int isrcH = int(round(fragTexCoord.y * 479.0));
    // int a = isrcH / 2;
    // float fsrcH = float(isrcH);
    // float w = float(PI * (fsrcH - 0.5));
    // float s = round(sin(w));
    // int is = int(s);
    // int idstH = (is + 1) * 120 + a;
    // float fdstH = float(idstH);
    // // float fdstH = (s + 1.0) * 120.0 +a;
    // texCoord.y = fdstH / 479.0;
    // // texCoord.y = s;

    float isodd = mod(texCoord.y, 2.0);
    vec4 result;
    if (bool(isodd)) {
	vec4 evenfield = texture(texSampler, vec3(texCoord.x, texCoord.y + 1.0, texCoord.z));
	vec4 oddfield = texture(texSampler, texCoord);
	result = mix(evenfield, oddfield, 0.5);
    } else {
	vec4 evenfield = texture(texSampler, texCoord);
	vec4 oddfield = texture(texSampler, vec3(texCoord.x, texCoord.y - 1.0, texCoord.z));
	result = mix(evenfield, oddfield, 0.5);
    }

    outColor = result;
    // outColor = texture(texSampler, texCoord);
}

