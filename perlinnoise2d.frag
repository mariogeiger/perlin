#version 130
in vec3 pos;
in float fog;
out vec4 color;

void main(void)
{
    vec4 a;
    vec4 b;

    float g = 0.2;
    bool bx = mod(pos.x, 2.0*g) < g;
    bool by = mod(pos.y, 2.0*g) < g;
    if (bx ^^ by) {
        a = vec4(0xdb, 0xa8, 0x73, 0xff) / 0xff;
        b = vec4(0x8c, 0x57, 0x2e, 0xff) / 0xff;
    } else {
        a = vec4(0xde, 0xf1, 0xf5, 0xff) / 0xff;
        b = vec4(0x8c, 0xd5, 0xdc, 0xff) / 0xff;
    }

    color = pos.z * a + (1.0 - pos.z) * b;

    float f = clamp(fog, 0.0, 1.0);
    color = (1.0 - f) * color + f * vec4(0x20, 0x20, 0x20, 0xff) / 0xff;
}
