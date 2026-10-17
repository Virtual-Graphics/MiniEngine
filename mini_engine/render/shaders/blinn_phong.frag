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

uniform sampler2DShadow u_shadow_map;
uniform bool u_shadow_enabled;
uniform mat4 u_light_view_proj;
uniform float u_shadow_depth_bias;
uniform float u_shadow_normal_bias;

float compute_shadow(vec3 world_pos, vec3 N, vec3 L) {
    if (!u_shadow_enabled) {
        return 1.0;
    }

    vec4 light_clip = u_light_view_proj * vec4(world_pos + N * u_shadow_normal_bias, 1.0);
    vec3 proj = light_clip.xyz / light_clip.w;
    proj = proj * 0.5 + 0.5;

    if (proj.z > 1.0 || proj.z < 0.0 ||
            proj.x < 0.0 || proj.x > 1.0 || proj.y < 0.0 || proj.y > 1.0) {
        return 1.0;
    }

    const float bias = max(u_shadow_depth_bias * (1.0 - dot(N, L)), u_shadow_depth_bias * 0.1);
    const vec2 texel = 1.0 / vec2(textureSize(u_shadow_map, 0));

    float sum = 0.0;
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            sum += texture(u_shadow_map, vec3(proj.xy + vec2(x, y) * texel, proj.z - bias));
        }
    }
    return sum / 9.0;
}

void main() {
    vec3 N = normalize(v_normal);
    vec3 L = normalize(-u_light_direction);
    vec3 V = normalize(u_view_pos.xyz - v_world_pos);
    vec3 H = normalize(L + V);

    float diff = max(dot(N, L), 0.0);
    float spec = diff > 0.0 ? pow(max(dot(N, H), 0.0), max(u_shininess, 1.0)) : 0.0;

    float shadow = compute_shadow(v_world_pos, N, L);

    vec3 color = u_ambient_color * u_diffuse
               + shadow * (u_light_color * u_diffuse * diff
                         + u_light_color * u_specular * spec);

    f_color = vec4(color, 1.0);
}
