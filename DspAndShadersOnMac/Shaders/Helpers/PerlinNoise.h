// mx_noise.metal
// Perlin noise that matches Three.js TSL's mx_noise_float (MaterialX noise2d / noise3d).

#include <metal_stdlib>
using namespace metal;

inline uint mx_hash(uint3 k)
{
    uint a = k.x, b = k.y, c = k.z;
    c ^= b; c -= rotate(b, 14u);
    a ^= c; a -= rotate(c, 11u);
    b ^= a; b -= rotate(a, 25u);
    c ^= b; c -= rotate(b, 16u);
    a ^= c; a -= rotate(c,  4u);
    b ^= a; b -= rotate(a, 14u);
    c ^= b; c -= rotate(b, 24u);
    return c;
}

inline uint mx_hash(int2 cell) { return mx_hash(uint3(uint2(cell), 0u) + (0xdeadbeefu + 21u)); }
inline uint mx_hash(int3 cell) { return mx_hash(uint3(cell)            + (0xdeadbeefu + 25u)); }

inline float mx_gradient(uint h, float2 d)
{
    float2 g = (h & 4u) ? d.yx : d;
    g.y *= 2.0f;
    return ((h & 1u) ? -g.x : g.x)
         + ((h & 2u) ? -g.y : g.y);   
}

inline float mx_gradient(uint h, float3 d)
{
    h &= 15u;
    float u = (h < 8u) ? d.x : d.y;
    float v = (h < 4u) ? d.y : ((h == 12u || h == 14u) ? d.x : d.z);
    return ((h & 1u) ? -u : u)
         + ((h & 2u) ? -v : v);
}

template <typename T>
inline T mx_fade(T t) { return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f); }

inline float mx_corner(int2 cell, float2 f, int2 o) { return mx_gradient(mx_hash(cell + o), f - float2(o)); }
inline float mx_corner(int3 cell, float3 f, int3 o) { return mx_gradient(mx_hash(cell + o), f - float3(o)); }

inline float mx_perlin_noise(float2 p)
{
    int2   cell = int2(floor(p));
    float2 f    = fract(p);
    float2 t    = mx_fade(f);

    float n00 = mx_corner(cell, f, int2(0, 0));
    float n10 = mx_corner(cell, f, int2(1, 0));
    float n01 = mx_corner(cell, f, int2(0, 1));
    float n11 = mx_corner(cell, f, int2(1, 1));

    return 0.6616f * mix(mix(n00, n10, t.x),
                         mix(n01, n11, t.x), t.y);
}

inline float mx_perlin_noise(float3 p)
{
    int3   cell = int3(floor(p));
    float3 f    = fract(p);
    float3 t    = mx_fade(f);

    float n000 = mx_corner(cell, f, int3(0, 0, 0));
    float n100 = mx_corner(cell, f, int3(1, 0, 0));
    float n010 = mx_corner(cell, f, int3(0, 1, 0));
    float n110 = mx_corner(cell, f, int3(1, 1, 0));
    float n001 = mx_corner(cell, f, int3(0, 0, 1));
    float n101 = mx_corner(cell, f, int3(1, 0, 1));
    float n011 = mx_corner(cell, f, int3(0, 1, 1));
    float n111 = mx_corner(cell, f, int3(1, 1, 1));

    float nz0 = mix(mix(n000, n100, t.x), mix(n010, n110, t.x), t.y);
    float nz1 = mix(mix(n001, n101, t.x), mix(n011, n111, t.x), t.y);
    return 0.9820f * mix(nz0, nz1, t.z);
}

//API

inline float mx_noise_float(float2 p, float amplitude = 1.0f, float pivot = 0.0f)
{
    return mx_perlin_noise(p) * amplitude + pivot;
}

inline float mx_noise_float(float3 p, float amplitude = 1.0f, float pivot = 0.0f)
{
    return mx_perlin_noise(p) * amplitude + pivot;
}


//Worley

// How "distance" is measured. Changes the shape of the cells.
enum MxWorleyMetric {
    MX_EUCLIDEAN = 0,   // straight-line distance: round cells
    MX_DIST2     = 1,   // squared straight-line distance (TSL's default; cheaper, darker falloff)
    MX_MANHATTAN = 2,   // |dx| + |dy|: diamond-shaped cells
    MX_CHEBYSHEV = 3,   // max(|dx|, |dy|): square cells
};

// --- Random numbers per cell ------------------------------------------------------

// Turn a hash into a float in 0..1.
inline float mx_bits_to_01(uint bits) { return float(bits) / float(0xffffffffu); }

// Bob Jenkins' mix step, needed for hashing 4 numbers (3D cell + channel).
inline void mx_bjmix(thread uint3& k)
{
    k.x -= k.z; k.x ^= rotate(k.z,  4u); k.z += k.y;
    k.y -= k.x; k.y ^= rotate(k.x,  6u); k.x += k.z;
    k.z -= k.y; k.z ^= rotate(k.y,  8u); k.y += k.x;
    k.x -= k.z; k.x ^= rotate(k.z, 16u); k.z += k.y;
    k.y -= k.x; k.y ^= rotate(k.x, 19u); k.x += k.z;
    k.z -= k.y; k.z ^= rotate(k.y,  4u); k.y += k.x;
}

inline uint mx_hash(int4 k)
{
    uint3 abc = uint3(k.xyz) + (0xdeadbeefu + 29u);
    mx_bjmix(abc);
    abc.x += uint(k.w);
    return mx_hash(abc);   // final scramble, from mx_noise.h
}

// The random feature point inside a cell, as an offset from the cell's corner (0..1).
// The last number in each hash picks a channel, so x, y and z get different randoms.
inline float2 mx_cell_point(int2 cell)
{
    return float2(mx_bits_to_01(mx_hash(int3(cell, 0))),
                  mx_bits_to_01(mx_hash(int3(cell, 1))));
}

inline float3 mx_cell_point(int3 cell)
{
    return float3(mx_bits_to_01(mx_hash(int4(cell, 0))),
                  mx_bits_to_01(mx_hash(int4(cell, 1))),
                  mx_bits_to_01(mx_hash(int4(cell, 2))));
}

// --- Distance -----------------------------------------------------------------------

inline float mx_worley_distance(float2 d, int metric)
{
    d = abs(d);
    if (metric == MX_MANHATTAN) return d.x + d.y;
    if (metric == MX_CHEBYSHEV) return max(d.x, d.y);
    return dot(d, d);   // squared; the sqrt (if wanted) happens once at the end
}

inline float mx_worley_distance(float3 d, int metric)
{
    d = abs(d);
    if (metric == MX_MANHATTAN) return d.x + d.y + d.z;
    if (metric == MX_CHEBYSHEV) return max3(d.x, d.y, d.z);
    return dot(d, d);
}

// --- Worley noise -------------------------------------------------------------------

inline float mx_worley_noise_float(float2 p, float jitter = 1.0f, int metric = MX_DIST2)
{
    int2   cell = int2(floor(p));
    float2 f    = fract(p);           // position inside the cell, 0..1
    float  best = 1e6f;

    for (int y = -1; y <= 1; ++y)
    for (int x = -1; x <= 1; ++x)
    {
        int2   o     = int2(x, y);
        // jitter = 0 puts the point at the cell center (0.5); 1 uses the full random offset.
        float2 point = float2(o) + mix(float2(0.5f), mx_cell_point(cell + o), jitter);
        best = min(best, mx_worley_distance(point - f, metric));
    }

    return (metric == MX_EUCLIDEAN) ? sqrt(best) : best;
}

inline float mx_worley_noise_float(float3 p, float jitter = 1.0f, int metric = MX_DIST2)
{
    int3   cell = int3(floor(p));
    float3 f    = fract(p);
    float  best = 1e6f;

    for (int z = -1; z <= 1; ++z)
    for (int y = -1; y <= 1; ++y)
    for (int x = -1; x <= 1; ++x)
    {
        int3   o     = int3(x, y, z);
        float3 point = float3(o) + mix(float3(0.5f), mx_cell_point(cell + o), jitter);
        best = min(best, mx_worley_distance(point - f, metric));
    }

    return (metric == MX_EUCLIDEAN) ? sqrt(best) : best;
}
    
    //==================================

    // By Inigo Quilez (https://iquilezles.org/articles/palettes/)
    inline float3 palette(float t, float3 a, float3 b, float3 c, float3 d)
    {
        return a + b * cos(6.283185f * (c * t + d));
    }
