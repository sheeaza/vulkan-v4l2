#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2DArray texSampler;

layout(location = 0) in vec3 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 yuv = texture(texSampler, fragTexCoord);
    // yuv = vec4(yuv.x * 4.0, yuv.y, yuv.z, 1.0);
    yuv = vec4(yuv.x, yuv.y, yuv.z, 1.0);

    // outColor.r = yuv.x * 1.0 + yuv.y * 0.0 + yuv.z * 1.4;
    // outColor.g = yuv.x * 1.0 + yuv.y * -0.343 + yuv.z * -0.711;
    // outColor.b = yuv.x * 1.0 + yuv.y * 1.765 + yuv.z * 0.0;
    // outColor.a = 1.0;
    outColor = yuv;
}

