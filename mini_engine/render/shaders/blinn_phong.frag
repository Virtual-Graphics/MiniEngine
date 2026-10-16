#version 460 core

in vec3 v_world_pos;
in vec3 v_normal;

out vec4 f_color;

layout (std140, binding = 0) uniform CameraBlock {
    mat4 u_view_proj;
    vec4 u_view_pos;
};

uniform vec3 u_ambient_color;
uniform vec3 u_light_direction;
uniform vec3 u_light_color;
uniform vec3 u_diffuse;
uniform vec3 u_specular;
uniform float u_shininess;

void main() {
    vec3 N = normalize(v_normal);
    vec3 L = normalize(-u_light_direction);
    vec3 V = normalize(u_view_pos.xyz - v_world_pos);
    vec3 H = normalize(L + V);

    float diff = max(dot(N, L), 0.0);
    float spec = diff > 0.0 ? pow(max(dot(N, H), 0.0), max(u_shininess, 1.0)) : 0.0;

    vec3 color = u_ambient_color * u_diffuse
               + u_light_color * u_diffuse * diff
               + u_light_color * u_specular * spec;

    f_color = vec4(color, 1.0);
}
