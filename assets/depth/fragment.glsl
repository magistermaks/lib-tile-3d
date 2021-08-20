
#version 330 core

out vec4 color;

in vec2 uv;

uniform sampler2D sampler;

void main() {

	vec4 texel = texture(sampler, uv);

	color = vec4(texel.xyz, 1.0f);
	gl_FragDepth = texel.a;

}
