#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2DArray texSampler;

layout(location = 0) in vec3 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 yuv = texture(texSampler, fragTexCoord);
    //
    float yl = yuv.r;
    float yh = yuv.g;
    float b = yuv.b;
    float y = (yh * 255.0 * 255.0 + yl * 255.0) / 16.0 / 1023.0;
    // yuv = vec4(yl, yl, yl, 1.0);
    yuv = vec4(y, y, y, 1.0);
    // yh *= 8.0;
    // yuv = vec4(yh, yh, yh, 1.0);
    // yuv = vec4(b, b, b, 1.0);

    // float yh = yuv.x * 255.0;
    // float yl = yuv.y * 255.0;
    // float y = (yl * 4.0 + yh) / 1023.0;
    // yuv = vec4(y, y, y, 1.0);

    outColor = yuv;
}

