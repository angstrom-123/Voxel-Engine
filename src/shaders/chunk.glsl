#version 300 es
precision highp uint;

@ctype mat4 em_mat4
@ctype vec4 em_vec4
@ctype vec3 em_vec3
@ctype vec2 em_vec2

@block unpack
vec3 unpack_pos(uvec4 vert) {
    float x = vert.x >> 3;
    float y = vert.y;
    float z = vert.z >> 3;
    return vec3(x, y, z);
}
vec2 unpack_uv(uvec4 vert) {
    uint uo = (vert.x >> 2) & 0x01;
    uint vo = (vert.x >> 1) & 0x01;

    uint up = vert.w >> 4;
    uint vp = vert.w & 0x0F;
    float u = float(up + uo) * 0.0625;
    float v = float(vp + vo) * 0.0625;
    return vec2(u, v);
}
vec3 unpack_normal(uvec4 vert) {
    uint packed = vert.z & 0x07;
    if      (packed == 1) return vec3( 1.0,  0.0,  0.0);
    else if (packed == 2) return vec3(-1.0,  0.0,  0.0);
    else if (packed == 3) return vec3( 0.0,  1.0,  0.0);
    else if (packed == 4) return vec3( 0.0, -1.0,  0.0);
    else if (packed == 5) return vec3( 0.0,  0.0,  1.0);
    else if (packed == 6) return vec3( 0.0,  0.0, -1.0);
    else return vec3(0.0, 0.0, 0.0);
}
@end

// Shadowmap
@vs vs_shadowmap
@glsl_options flip_vert_y

@include_block unpack

layout(binding=0) uniform vs_params_shadowmap {
    mat4 u_vp;
    vec3 u_chunk;
};

in uvec4 a_vert;

void main() {
    vec3 pos = unpack_pos(a_vert);
    gl_Position = u_vp * vec4(pos + u_chunk, 1.0);
}

@end

@fs fs_shadowmap
layout(location=0) out float out_shadowmap;

void main() {
    out_shadowmap = 1.0 - gl_FragCoord.z;
}

@end 

@program shadowmap vs_shadowmap fs_shadowmap

// Offscreen
@vs vs_offscreen

@include_block unpack

layout(binding=0) uniform vs_params_offscreen {
    mat4 u_vp;
    vec3 u_chunk;
    mat4 u_sun_vp;
};

in uvec4 a_vert;

out vec2 v_uv;
out vec3 v_nrm;
out vec4 v_pos;
out vec4 v_sunspace_pos;

void main() {
    vec4 m = vec4(unpack_pos(a_vert) + u_chunk, 1.0);

    v_uv = unpack_uv(a_vert);
    v_nrm = unpack_normal(a_vert);
    v_pos = u_vp * m;
    v_sunspace_pos = u_sun_vp * m;

    gl_Position = v_pos;
}

@end

@fs fs_offscreen
layout(binding=0) uniform sampler u_atlas_smp;
layout(binding=0) uniform texture2D u_atlas;
layout(binding=1) uniform sampler u_shadow_smp;
layout(binding=1) uniform texture2D u_shadow_map;

layout(binding=1) uniform fs_params_offscreen {
    vec3 u_sun_dir;
};

in vec2 v_uv;
in vec3 v_nrm;
in vec4 v_pos;
in vec4 v_sunspace_pos;

layout(location=0) out vec4 out_albedo;
layout(location=1) out vec4 out_normal;
layout(location=2) out float out_depth;
layout(location=3) out float out_shadow;

float visibility() {
    vec3 up = vec3(0.0, 1.0, 0.0);
    if (dot(u_sun_dir, up) < 0.0)
        return 0.0;

    vec3 sm_pos = -v_sunspace_pos.xyz / v_sunspace_pos.w;
    sm_pos = sm_pos * 0.5 + 0.5;
    sm_pos.x = 1.0 - sm_pos.x;

    if (sm_pos.x > 1.0 || sm_pos.x < 0.0 || sm_pos.y > 1.0 || sm_pos.y < 0.0)
        return 1.0;

    float bias = max(0.010 * (1.0 - dot(v_nrm, u_sun_dir)), 0.010);

    float curr_dep = sm_pos.z;

    float texel_size = 1.0 / 1024.0;
    float visibility = 0.0;
    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            vec2 pcf_uv = sm_pos.xy + vec2(x, y) * texel_size;
            float pcf_dep = texture(sampler2D(u_shadow_map, u_shadow_smp), pcf_uv).x;
            visibility += (curr_dep + bias < pcf_dep) ? 0.0 : 1.0;
        }
    }

    return visibility / 25.0;
}

void main() {
    out_normal = vec4(v_nrm * 0.5 + 0.5, 1.0);
    out_depth = gl_FragCoord.z;
    out_albedo = texture(sampler2D(u_atlas, u_atlas_smp), v_uv);
    out_shadow = visibility();
}

@end 

@program offscreen vs_offscreen fs_offscreen

// Composite
@vs vs_composite

in vec2 a_pos;

out vec2 v_uv;

void main() {
    gl_Position = vec4(a_pos * 2.0 - 1.0, 0.5, 1.0);
    v_uv = a_pos;
}

@end

@fs fs_composite
layout(binding=0) uniform sampler u_composite_smp;
layout(binding=0) uniform texture2D u_galbedo;
layout(binding=1) uniform texture2D u_gnormal_composite;
layout(binding=2) uniform texture2D u_gdepth_composite;
layout(binding=3) uniform texture2D u_gshadow;

layout(binding=1) uniform fs_params_composite {
    mat4 u_inv_vp;
    vec3 u_sun_dir;
    vec3 u_eye_pos;
    float u_view_distance;
};

in vec2 v_uv;

layout(location=0) out vec4 out_colour;

vec3 remap(vec3 vec) {
    return vec * 2.0 - 1.0;
}

vec3 world_pos_from_depth(float d) {
    vec4 clip = vec4(remap(vec3(v_uv, d)), 1.0);
    vec4 world = u_inv_vp * clip;
    return world.xyz / world.w;
}

vec3 gamma(vec3 c) {
    float gamma = 1.5;
    return pow(c, vec3(1.0 / gamma));
}

vec3 light(vec3 nrm, float sha) {
    vec3 l = normalize(u_sun_dir);
    vec3 n = normalize(nrm);
    float n_dot_l = dot(n, l);

    float diffuse = 0.0;
    if (n_dot_l > 0.0) {
        float s = sha;
        diffuse = clamp(n_dot_l * s, 0.0, 0.5);
    }

    float sun_dot_up = dot(u_sun_dir, vec3(0.0, 1.0, 0.0));

    vec3 sun_tint = vec3(1.0);
    float sun_strength = max(sun_dot_up, 0.3);
    float day_ambient = 0.5;

    return vec3((diffuse + day_ambient) * sun_tint * sun_strength);
}

void main() {
    float depth = texture(sampler2D(u_gdepth_composite, u_composite_smp), v_uv).x;
    vec3 position = world_pos_from_depth(depth);
    vec4 albedo = texture(sampler2D(u_galbedo, u_composite_smp), v_uv);
    vec3 normal = remap(texture(sampler2D(u_gnormal_composite, u_composite_smp), v_uv).xyz);
    float shadow = texture(sampler2D(u_gshadow, u_composite_smp), v_uv).x;

    vec3 lighting = light(normal, shadow);
    vec3 corrected = vec3(gamma(lighting * albedo.xyz));

    float dist = length(position - u_eye_pos);
    float rem = (u_view_distance - dist) / 10.0;
    out_colour = vec4(corrected, 1.0);
    if (albedo.a == 1.0 && rem <= 1.0) {
        out_colour.a *= rem;
    }
}

@end

@program composite vs_composite fs_composite

// Skybox
@vs vs_skybox

layout(binding=0) uniform vs_params_skybox {
    mat4 u_vp;
    vec3 u_pos;
};

in vec3 a_pos;
in vec3 a_nrm;

out vec4 v_pos;
out vec3 v_nrm;
out vec3 v_view_pos;

void main() {
    vec4 m = vec4(a_pos + u_pos, 1.0);

    v_pos = m;
    v_nrm = a_nrm;
    v_view_pos = u_pos;

    gl_Position = u_vp * m;
}
@end 

@fs fs_skybox

layout(binding=1) uniform fs_params_skybox {
    vec3 u_sun_dir;
};

in vec4 v_pos;
in vec3 v_nrm;
in vec3 v_view_pos;

layout(location=0) out vec4 out_skybox;

const float pi = 3.14159265359;
const float invPi = 1.0 / pi;
const float zenithOffset = 0.1;
const float multiScatterPhase = 0.1;
// const float density = 0.7;
const float density = 0.5;
// const float anisotropicIntensity = 0.0;
const float anisotropicIntensity = -0.5;
const vec3 skyColor = vec3(0.39, 0.57, 1.0) * (1.0 + anisotropicIntensity);

#define smooth(x) x*x*(3.0-2.0*x)
#define zenithDensity(x) density / pow(max(x - zenithOffset, 0.35e-2), 0.75)

vec3 getSkyAbsorption(vec3 x, float y){
	
	vec3 absorption = x * -y;
	     absorption = exp2(absorption) * 2.0;
	
	return absorption;
}

float getSunPoint(vec3 p, vec3 lp){
	return smoothstep(0.03, 0.026, distance(p, lp)) * 50.0;
}

float getRayleigMultiplier(vec3 p, vec3 lp){
	return 1.0 + pow(1.0 - clamp(distance(p, lp), 0.0, 1.0), 2.0) * pi * 0.5;
}

float getMie(vec3 p, vec3 lp){
	float disk = clamp(1.0 - pow(distance(p, lp), 0.1), 0.0, 1.0);
	
	return disk*disk*(3.0 - 2.0 * disk) * 2.0 * pi;
}

vec3 getAtmosphericScattering(vec3 p, vec3 lp){
	float zenith = zenithDensity(p.y);
	float sunPointDistMult =  clamp(length(max(lp.y + multiScatterPhase - zenithOffset, 0.0)), 0.0, 1.0);
	
	float rayleighMult = getRayleigMultiplier(p, lp);
	
	vec3 absorption = getSkyAbsorption(skyColor, zenith);
    vec3 sunAbsorption = getSkyAbsorption(skyColor, zenithDensity(lp.y + multiScatterPhase));
	vec3 sky = skyColor * zenith * rayleighMult;
	vec3 sun = getSunPoint(p, lp) * absorption;
	vec3 mie = getMie(p, lp) * sunAbsorption;
	
	vec3 totalSky = mix(sky * absorption, sky / (sky + 0.5), sunPointDistMult);
         totalSky += sun + mie;
	     totalSky *= sunAbsorption * 0.5 + 0.5 * length(sunAbsorption);
	
	return totalSky;
}

vec3 jodieReinhardTonemap(vec3 c){
    float l = dot(c, vec3(0.2126, 0.7152, 0.0722));
    vec3 tc = c / (c + 1.0);

    return mix(c / (l + 1.0), tc, tc);
}

void main() {
    vec3 moved_down = -v_view_pos;
    moved_down.y += 20.0; // Offset the horizon line down a little
    vec3 dir = normalize(v_pos.xyz + moved_down);
    vec3 sky = getAtmosphericScattering(dir, u_sun_dir) * pi;
    sky = jodieReinhardTonemap(sky);
    sky = pow(sky, vec3(2.2));
    out_skybox = vec4(sky, 1.0);
}

@end 

@program skybox vs_skybox fs_skybox

// Effects
@vs vs_effects

in vec2 a_pos;

out vec2 v_uv;

void main() {
    gl_Position = vec4(a_pos * 2.0 - 1.0, 0.5, 1.0);
    v_uv = a_pos;
}
@end

@fs fs_effects

// #define DEBUG_SHADOWS

layout(binding=0) uniform sampler u_effects_smp;
#ifdef DEBUG_SHADOWS

layout(binding=5) uniform texture2D u_effects_shadow_map;

#else

layout(binding=0) uniform texture2D u_colour;
layout(binding=1) uniform texture2D u_gnormal_effects;
layout(binding=2) uniform texture2D u_gdepth_effects;
layout(binding=3) uniform texture2D u_skybox;
layout(binding=4) uniform texture2D u_ssao_noise;

layout(binding=0) uniform fs_params_effects {
    vec4 u_ssao_samples[64];
    mat4 u_inv_vp;
    mat4 u_proj;
    mat4 u_view;
};

#endif

in vec2 v_uv;

out vec4 frag_col;

#ifdef DEBUG_SHADOWS

void main() {
    frag_col = texture(sampler2D(u_effects_shadow_map, u_effects_smp), v_uv);
}

#else

const vec2 noise_scale = vec2(1920.0 / 4.0, 1080.0 / 4.0);
const float kernel_size = 64.0;
const float radius = 0.5;
const float bias = 0.025;

vec3 remap(vec3 vec) {
    return vec * 2.0 - 1.0;
}

vec3 world_pos_from_depth(float d) {
    vec4 clip = vec4(remap(vec3(v_uv, d)), 1.0);
    vec4 world = u_inv_vp * clip;
    return world.xyz / world.w;
}

void main() {
    vec4 colour = texture(sampler2D(u_colour, u_effects_smp), v_uv);
    vec4 skybox = texture(sampler2D(u_skybox, u_effects_smp), v_uv);
    vec3 normal = remap(texture(sampler2D(u_gnormal_effects, u_effects_smp), v_uv).xyz);
    float depth = texture(sampler2D(u_gdepth_effects, u_effects_smp), v_uv).x;
    vec3 random = vec3(texture(sampler2D(u_ssao_noise, u_effects_smp), v_uv * noise_scale).xy, 0.0);
    vec3 position = world_pos_from_depth(depth);

    vec3 frag_position = normalize((u_view * vec4(position, 1.0)).xyz);
    vec3 frag_normal = normalize((u_view * vec4(normal, 0.0)).xyz);

    vec3 tangent = normalize(random - frag_normal * dot(random, frag_normal));
    vec3 bitangent = cross(frag_normal, tangent);
    mat3 tbn = mat3(tangent, bitangent, frag_normal);

    float occlusion = 0.0;
    for (int i = 0; i < 1; i++) {
        vec3 sample_pos = tbn * u_ssao_samples[i].xyz;
        sample_pos = frag_position + sample_pos * radius;
        // sample_pos *= 0.00000001;
        // sample_pos += frag_position;
        vec4 offset = vec4(sample_pos, 1.0);
        offset = u_proj * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;
        float sample_depth = texture(sampler2D(u_gdepth_effects, u_effects_smp), offset.xy).x;
        occlusion += sample_depth >= sample_pos.z + bias ? 1.0 : 0.0;
        occlusion = sample_depth;
    }
    // occlusion = 1.0 - (occlusion / kernel_size);

    if (all(lessThan(normal, vec3(0.001))))
        frag_col = vec4(skybox.xyz, 1.0);
    else {
        colour.xyz = colour.xyz + vec3(occlusion * 0.0000000000001);
        frag_col = mix(skybox, colour, colour.a);
    }
}
#endif
@end 

@program effects vs_effects fs_effects
