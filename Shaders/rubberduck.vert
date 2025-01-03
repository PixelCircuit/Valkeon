#version 450

layout(push_constant) uniform PerFrameData {
	mat4 MVP;
} pc;

layout (location=0) in vec3 pos;
layout (location=1) out vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
	gl_Position = pc.MVP * vec4(pos, 1.0);
	fragColor = inColor;
}