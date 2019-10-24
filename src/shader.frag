#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2DArray texSampler;

layout(location = 0) in vec3 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 yuv = texture(texSampler, fragTexCoord);

    float r = yuv.r * 255.0;
    float g = yuv.g * 255.0;
    float y = (r + g * 256.0) / 4.0 / 4095.0;
    yuv = vec4(y, y, y, 1.0);

    outColor = yuv;
}

