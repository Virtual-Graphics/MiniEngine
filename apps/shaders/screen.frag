#version 460 core

in vec2 v_uv;

out vec4 f_color;

uniform sampler2D u_texture;

void main() {
    f_color = vec4(texture(u_texture, v_uv).rgb, 1.0);
}
