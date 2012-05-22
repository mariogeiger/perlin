#version 130
in vec3 vertex;
uniform mat4 matrixp;
uniform mat4 matrixv;
uniform mat4 matrixm;
out vec3 pos;
out float fog;

void main(void)
{
    gl_Position = matrixp * matrixv * matrixm * vec4(vertex, 1.0);
    pos = vertex;
    fog = (gl_Position.z + 1.0) / 2.0;
}
