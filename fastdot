#include <immintrin.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#define NORMAL 0

struct _Vec3 {
    float x;
    float y;
    float z;
    float w;
};

typedef struct _Vec3 Vec3;

Vec3 sub(Vec3 a, Vec3 b) {
    Vec3 ans;
    ans.x = a.x - b.x;
    ans.y = a.y - b.y;
    ans.z = a.z - b.z;
    return ans;
}

float hsum_sse1(__m128 v) {
    __m128 shuf = _mm_movehdup_ps(v);        // broadcast elements 3,1 to 2,0
    __m128 sums = _mm_add_ps(v, shuf);
    shuf        = _mm_movehl_ps(shuf, sums); // high half -> low half
    sums        = _mm_add_ss(sums, shuf);
    return        _mm_cvtss_f32(sums);
}

float normalDot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

__m128 singleDot(__m128 a, __m128 b) {
    return _mm_dp_ps(a, b, 0b00001111);
}

float dotProduct(__m128 a, __m128 b) {
    __m128 temp = _mm_mul_ps(a, b);
    return hsum_sse1(temp);
}

int main(int argc, char** argv) {
    // Conclusion of this experiment: Doing a single dot product,
    // there is no benefit to using SSE intrinsics vs. the naive
    // way.
    //
    float x = 0.0;
    // for (uint64_t i = 0; i < (1L << 29); i++) {
    //     Vec3 a = {i, i + 0.5, i + 1, 0.0};
    //     Vec3 b = {i, i - 0.5, i - 1, 0.0};
    //     x += a.x;
    //     #if NORMAL
    //     float ans = normalDot(a, b);
    //     #else
    //     // float _c[4] = {a.x, a.y, a.z, 0.0};
    //     // float _d[4] = {b.x, b.y, b.z, 0.0};
    //     __m128 c = _mm_load_ps((float*)&a);
    //     __m128 d = _mm_load_ps((float*)&b);
    //     __m128 ans = singleDot(c, d);
    //     #endif
    // }
    // printf("%f\n", x);

    // But what if we do four at a time?

    for (uint64_t i = 0; i < (1L << 29); i++) {
        // ray origin/direction
        // Vec3 center = {getRand(), getRand(), getRand(), 0.0};
        // Vec3 dir = {getRand(), getRand(), getRand(), 0.0};

        Vec3 center = {1.0 + i, 1.0 - i, 1.0, 0.0};
        Vec3 dir = {1.0, 1.0, 1.0, 0.0};

        Vec3 _a = {1.0, 2.0, 3.0, 4.0 - i / 2};
        Vec3 _b = {2.0, 3.0, 4.0, 5.0};
        Vec3 _c = {3.0, 4.0, 5.0 + i / 3, 6.0};
        Vec3 _d = {4.0, 5.0 - 2 * i, 6.0, 7.0};

        x += _d.y;

        // sphere centers
        // Vec3 _a = {getRand(), getRand(), getRand(), getRand()};
        // Vec3 _b = {getRand(), getRand(), getRand(), getRand()};
        // Vec3 _c = {getRand(), getRand(), getRand(), getRand()};
        // Vec3 _d = {getRand(), getRand(), getRand(), getRand()};
        #if NORMAL
        Vec3 arr[4] = {_a, _b, _c, _d};
        // Treat _a as {x, y, z, w} (w is dummy), Array of Structs
        for (int j = 0; j < 4; j++) {
            Vec3 diff = sub(center, arr[j]);
            float ans = normalDot(diff, dir);
        }
        #else
        // Treat _a as {x1, x2, x3, x4}, Struct of Arrays, ignore _d
        __m128 mx = _mm_load_ps((float*)&_a); // this shouldn't work...
        __m128 my = _mm_load_ps((float*)&_b); // treating Vec3 as float[4]
        __m128 mz = _mm_load_ps((float*)&_c);

        float _cx[4] = {center.x, center.x, center.x, center.x};
        float _cy[4] = {center.y, center.y, center.y, center.y};
        float _cz[4] = {center.z, center.z, center.z, center.z};

        __m128 cx = _mm_load_ps(_cx);
        __m128 cy = _mm_load_ps(_cy);
        __m128 cz = _mm_load_ps(_cz);

        float _dx[4] = {dir.x, dir.x, dir.x, dir.x};
        float _dy[4] = {dir.y, dir.y, dir.y, dir.y};
        float _dz[4] = {dir.z, dir.z, dir.z, dir.z};

        __m128 dx = _mm_load_ps(_dx);
        __m128 dy = _mm_load_ps(_dy);
        __m128 dz = _mm_load_ps(_dz);

        __m128 diffx = _mm_sub_ps(mx, cx);
        __m128 diffy = _mm_sub_ps(my, cy);
        __m128 diffz = _mm_sub_ps(mz, cz);

        __m128 dotx = _mm_mul_ps(diffx, dx);
        __m128 doty = _mm_mul_ps(diffy, dy);
        __m128 dotz = _mm_mul_ps(diffz, dz);

        __m128 temp1 = _mm_add_ps(dotx, doty);
        __m128 ans = _mm_add_ps(temp1, dotz);

        float f[4];

        _mm_store_ps(f, ans);
        // printf("%f, %f, %f, %f\n", f[0], f[1], f[2], f[3]);

        #endif
    }
    printf("%f\n", x);
}