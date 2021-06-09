
#version 330

out vec4 color;
  
in vec2 uv;

uniform sampler2D canvas;

void main() {
    color = texture(canvas, uv);
}
