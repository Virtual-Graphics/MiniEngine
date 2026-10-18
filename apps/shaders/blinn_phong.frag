#version 460 core

in vec3 v_world_pos;
in vec3 v_normal;

out vec4 f_color;

layout (std140, binding = 0) uniform CameraBlock {
    mat4 u_view_proj;
    vec3 u_view_pos;
};

const vec3  light_pos      = vec3(15.0, 15.0, 15.0);
const vec3  light_color    = vec3(1.0);
const vec3  ambient_color  = vec3(0.15);
const vec3  diffuse_color  = vec3(0.90, 0.90, 0.90);
const vec3  specular_color = vec3(0.08);
const float shininess      = 8.0;

void main() {
    vec3 N = normalize(v_normal);
    vec3 L = normalize(light_pos - v_world_pos);
    vec3 V = normalize(u_view_pos - v_world_pos);
    vec3 H = normalize(L + V);

    float diff = max(dot(N, L), 0.0);
    float spec = diff > 0.0 ? pow(max(dot(N, H), 0.0), shininess) : 0.0;

    vec3 color = ambient_color * diffuse_color
               + light_color * diffuse_color * diff
               + light_color * specular_color * spec;

    f_color = vec4(color, 1.0);
}
