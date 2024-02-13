#pragma once
#include "Math.hpp"

#if defined(__GNUC__) || defined(__clang__)
    
#endif

AX_NAMESPACE 

enum CPUIDBits : int
{
    CPUIDBits_SSE    = (1 << 25),
    CPUIDBits_SSE2   = (1 << 26),
    CPUIDBits_SSE3   = (1 << 9),
    CPUIDBits_SSE4_1 = (1 << 19),
    CPUIDBits_SSE4_2 = (1 << 20),
    CPUIDBits_AVX2   = (1 << 5),
    CPUIDBits_AVX512 = (1 << 16),
};

// for runtime SIMD extension detection.
// sometimes you might need runtime extension detection.
// recommended using global variable like this in a cpp file:
// int g_ax_simd_bits = 0; or inline int g_ax_simd_bits = 0; in here.
// then call AX_InitSIMD_CPUID only once in program lifetime
// then use !!(g_ax_simd_bits & CPUIDBits_SSE2) to get the support value
inline int AX_InitSIMD_CPUID()
{
    int info[4];
    AX_CPUID(1, info);
    int mask = 1;
    mask |= info[3] & (CPUIDBits_SSE | CPUIDBits_SSE2);
    mask |= info[2] & (CPUIDBits_SSE3 | CPUIDBits_SSE4_1 | CPUIDBits_SSE4_2);
    AX_CPUID(7, info);
    mask |= info[1] & (CPUIDBits_AVX2 | CPUIDBits_AVX512);
    return mask;
}

#if defined(AX_SUPPORT_SSE) && !defined(AX_ARM)
/*//////////////////////////////////////////////////////////////////////////*/
/*                                 SSE                                      */
/*//////////////////////////////////////////////////////////////////////////*/

typedef __m128 vec_t;
typedef __m128 veci_t;

#define VecZero()           _mm_setzero_ps()
#define VecOne()            _mm_set1_ps(1.0f)
#define VecNegativeOne()    _mm_setr_ps( -1.0f, -1.0f, -1.0f, -1.0f)
#define VecSet1(x)          _mm_set1_ps(x)
#define VecSet(x, y, z, w)  _mm_set_ps(x, y, z, w)  /* -> {w, z, y, x} */
#define VecSetR(x, y, z, w) _mm_setr_ps(x, y, z, w) /* -> {x, y, z, w} */
#define VecLoad(x)          _mm_loadu_ps(x)
#define VecLoadA(x)         _mm_load_ps(x)
#define Vec3Load(x)         _mm_loadu_ps(x)

#define VecStore(ptr, x)       _mm_store_ps(ptr, x)
#define VecFromInt(x, y, z, w) _mm_castsi128_ps(_mm_setr_epi32(x, y, z, w))
#define VecFromInt1(x)         _mm_castsi128_ps(_mm_set1_epi32(x))
#define VecToInt(x) x
#define VecFromVeci(x) x

// Get Set
#define VecSplatX(V1) _mm_permute_ps(V1, MakeShuffleMask(0, 0, 0, 0))
#define VecSplatY(V1) _mm_permute_ps(V1, MakeShuffleMask(1, 1, 1, 1))
#define VecSplatZ(V1) _mm_permute_ps(V1, MakeShuffleMask(2, 2, 2, 2))
#define VecSplatW(V1) _mm_permute_ps(V1, MakeShuffleMask(3, 3, 3, 3))

#define VecGetX(v) _mm_cvtss_f32(v)
#define VecGetY(v) _mm_cvtss_f32(VecSplatY(v))
#define VecGetZ(v) _mm_cvtss_f32(VecSplatZ(v))
#define VecGetW(v) _mm_cvtss_f32(VecSplatW(v))

#define VecSetX(v, x) _mm_move_ss(v, _mm_set_ss(x))
#define VecSetY(v, y) _mm_insert_ps(v, _mm_set_ss(y), 0x10)
#define VecSetZ(v, z) _mm_insert_ps(v, _mm_set_ss(z), 0x20)
#define VecSetW(v, w) _mm_insert_ps(v, _mm_set_ss(w), 0x30)

// Arithmetic
#define VecAdd(a, b) _mm_add_ps(a, b)
#define VecSub(a, b) _mm_sub_ps(a, b)
#define VecMul(a, b) _mm_mul_ps(a, b)
#define VecDiv(a, b) _mm_div_ps(a, b)

#define VecAddf(a, b) _mm_add_ps(a, VecSet1(b))
#define VecSubf(a, b) _mm_sub_ps(a, VecSet1(b))
#define VecMulf(a, b) _mm_mul_ps(a, VecSet1(b))
#define VecDivf(a, b) _mm_div_ps(a, VecSet1(b))

#define VecFmad(a, b, c) _mm_fmadd_ps(a, b, c)
#define VecHadd(a, b) _mm_hadd_ps(a, b)

#define VecRcp(a) _mm_rcp_ps(a)
#define VecSqrt(a) _mm_sqrt_ps(a)

// Vector Math
#define VecDot(a, b)  _mm_dp_ps(a, b, 0xff)
#define VecDotf(a, b) _mm_cvtss_f32(_mm_dp_ps(a, b, 0xff))
#define VecNorm(v)    _mm_mul_ps(_mm_rsqrt_ps(_mm_dp_ps(v, v, 0xff)), v);
#define VecLenf(v)    _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(v, v, 0xff)))
#define VecLen(v)     VecSqrt(_mm_dp_ps(v, v, 0xff))

#define Vec3Dot(a, b)  _mm_dp_ps(a, b, 0x7f)
#define Vec3Dotf(a, b) _mm_cvtss_f32(_mm_dp_ps(a, b, 0x7f))
#define Vec3Norm(v)    _mm_mul_ps(_mm_rsqrt_ps(_mm_dp_ps(v, v, 0x7f)), v)
#define Vec3Lenf(v)    _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(v, v, 0x7f)))
#define Vec3Len(v)     _mm_sqrt_ps(_mm_dp_ps(v, v, 0x7f))

// Swizzling Masking
#define VecSelect1000 _mm_castsi128_ps(_mm_setr_epi32(0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000))
#define VecSelect1100 _mm_castsi128_ps(_mm_setr_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000))
#define VecSelect1110 _mm_castsi128_ps(_mm_setr_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000))
#define VecSelect1011 _mm_castsi128_ps(_mm_setr_epi32(0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF))

#define VecIdentityR0 _mm_setr_ps(1.0f, 0.0f, 0.0f, 0.0f)
#define VecIdentityR1 _mm_setr_ps(0.0f, 1.0f, 0.0f, 0.0f)
#define VecIdentityR2 _mm_setr_ps(0.0f, 0.0f, 1.0f, 0.0f)
#define VecIdentityR3 _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f)

#define VecMaskXY _mm_castsi128_ps(_mm_setr_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000))
#define VecMask3  _mm_castsi128_ps(_mm_setr_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000))
#define VecMaskX  _mm_castsi128_ps(_mm_setr_epi32(0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000))
#define VecMaskY  _mm_castsi128_ps(_mm_setr_epi32(0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000))
#define VecMaskZ  _mm_castsi128_ps(_mm_setr_epi32(0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000))
#define VecMaskW  _mm_castsi128_ps(_mm_setr_epi32(0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF))

#define MakeShuffleMask(x,y,z,w)     (x | (y<<2) | (z<<4) | (w<<6)) /* internal use only */
// vec(0, 1, 2, 3) -> (vec[x], vec[y], vec[z], vec[w])
#define VecSwizzleMask(vec, mask)    _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec), mask))
#define VecSwizzle(vec, x, y, z, w)  VecSwizzleMask(vec, MakeShuffleMask(x,y,z,w))

// return (vec1[x], vec1[y], vec2[z], vec2[w])
#define VecShuffle(vec1, vec2, x, y, z, w)   _mm_shuffle_ps(vec1, vec2, MakeShuffleMask(x,y,z,w))
#define VecShuffleR(vec1, vec2, x, y, z, w)  _mm_shuffle_ps(vec1, vec2, MakeShuffleMask(w,z,y,x))
// Special shuffle
#define VecShuffle_0101(vec1, vec2) _mm_movelh_ps(vec1, vec2)
#define VecShuffle_2323(vec1, vec2) _mm_movehl_ps(vec2, vec1)

// Logical
#define VecAnd(a, b)    _mm_and_ps(a, b)
#define VecOr(a, b)     _mm_or_ps(a, b)
#define VecXor(a, b)    _mm_xor_ps(a, b)
#define VecMask(a, msk) _mm_and_ps(a, msk)

#define VecMax(a, b) _mm_max_ps(a, b)
#define VecMin(a, b) _mm_min_ps(a, b)

#define VecCmpGe(a, b) _mm_cmpge_ps(a, b) /* greater or equal */
#define VecCmpLt(a, b) _mm_cmplt_ps(a, b) /* less than */
#define VecCmpLe(a, b) _mm_cmple_ps(a, b) /* less or equal */
#define VecMovemask(a) _mm_movemask_ps(a)

#define VecSelect(V1, V2, Control)  _mm_blendv_ps(V1, V2, Control);
#define VecBlend(a, b, c) _mm_blendv_ps(a, b, c)

#elif defined(AX_ARM)
/*//////////////////////////////////////////////////////////////////////////*/
/*                                 NEON                                     */
/*//////////////////////////////////////////////////////////////////////////*/

typedef float32x4_t vec_t;
typedef uint32x4_t veci_t;

#define VecZero()           vdupq_n_f32(0.0f)
#define VecOne()            vdupq_n_f32(1.0f)
#define VecNegativeOne()    vdupq_n_f32(-1.0f)
#define VecSet1(x)          vdupq_n_f32(x)
#define VecSet(x, y, z, w)  ARMCreateVec(w, z, y, x) /* -> {w, z, y, x} */
#define VecSetR(x, y, z, w) ARMCreateVec(x, y, z, w) /* -> {x, y, z, w} */
#define VecLoad(x)          vld1q_f32(x)
#define VecLoadA(x)         vld1q_f32(x)
#define Vec3Load(x)         ARMVector3Load(x)

#define VecStore(ptr, x)        vst1q_f32(ptr, x)
#define VecFromInt1(x)          vdupq_n_s32(x)
#define VecFromInt(x, y, z, w)  ARMCreateVecI(x, y, z, w)
#define VecToInt(x) x

#define VecFromVeci(x) x
// Get Set
#define VecSplatX(v)  vdupq_lane_f32(vget_low_f32(v), 0)
#define VecSplatY(v)  vdupq_lane_f32(vget_low_f32(v), 1)
#define VecSplatZ(v)  vdupq_lane_f32(vget_high_f32(v), 0)
#define VecSplatW(v)  vdupq_lane_f32(vget_high_f32(v), 1)

#define VecGetX(v) vgetq_lane_f32(v, 0)
#define VecGetY(v) vgetq_lane_f32(v, 1)
#define VecGetZ(v) vgetq_lane_f32(v, 2)
#define VecGetW(v) vgetq_lane_f32(v, 3)

#define VecSetX(v, x) ((v) = vsetq_lane_f32(x, v, 0))
#define VecSetY(v, y) ((v) = vsetq_lane_f32(y, v, 1))
#define VecSetZ(v, z) ((v) = vsetq_lane_f32(z, v, 2))
#define VecSetW(v, w) ((v) = vsetq_lane_f32(w, v, 3))

// Arithmetic
#define VecAdd(a, b) vaddq_f32(a, b)
#define VecSub(a, b) vsubq_f32(a, b)
#define VecMul(a, b) vmulq_f32(a, b)
#define VecDiv(a, b) ARMVectorDevide(a, b)

#define VecAddf(a, b) vaddq_f32(a, VecSet1(b))
#define VecSubf(a, b) vsubq_f32(a, VecSet1(b))
#define VecMulf(a, b) vmulq_f32(a, VecSet1(b))
#define VecDivf(a, b) ARMVectorDevide(a, VecSet1(b))

#define VecFmad(a, b, c) vfmaq_f32(a, b, c)
#define VecHadd(a, b)    vpaddq_f32(a, b)
#define VecSqrt(a)       vsqrtq_f32(a)
#define VecRcp(a)        vrecpeq_f32(a)

// Vector Math
#define VecDot(a, b)  ARMVectorDot(a, b)
#define VecDotf(a, b) VecGetX(ARMVectorDot(a, b))
#define VecNorm(v)    ARMVectorNorm(v)
#define VecLenf(v)    VecGetX(ARMVectorLength(v))
#define VecLen(v)     ARMVectorLength(v)

#define Vec3Dot(a, b)  ARMVector3Dot(a, b)
#define Vec3Dotf(a, b) VecGetX(ARMVector3Dot(a, b))
#define Vec3Norm(v)    ARMVector3Norm(v)
#define Vec3Lenf(v)    VecGetX(ARMVector3Length(v))
#define Vec3Len(v)     ARMVector3Length(v)

// Swizzling Masking
#define VecSelect1000 ARMCreateVecI(0xFFFFFFFFu, 0x00000000u, 0x00000000u, 0x00000000u)
#define VecSelect1100 ARMCreateVecI(0xFFFFFFFFu, 0xFFFFFFFFu, 0x00000000u, 0x00000000u)
#define VecSelect1110 ARMCreateVecI(0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0x00000000u)
#define VecSelect1011 ARMCreateVecI(0xFFFFFFFFu, 0x00000000u, 0xFFFFFFFFu, 0xFFFFFFFFu)

#define VecIdentityR0 ARMCreateVec(1.0f, 0.0f, 0.0f, 0.0f)
#define VecIdentityR1 ARMCreateVec(0.0f, 1.0f, 0.0f, 0.0f)
#define VecIdentityR2 ARMCreateVec(0.0f, 0.0f, 1.0f, 0.0f)
#define VecIdentityR3 ARMCreateVec(0.0f, 0.0f, 0.0f, 1.0f)

#define VecMaskXY ARMCreateVecI(0xFFFFFFFFu, 0xFFFFFFFFu, 0x00000000u, 0x00000000u)
#define VecMask3  ARMCreateVecI(0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0x00000000u)
#define VecMaskX  ARMCreateVecI(0xFFFFFFFFu, 0x00000000u, 0x00000000u, 0x00000000u)
#define VecMaskY  ARMCreateVecI(0x00000000u, 0xFFFFFFFFu, 0x00000000u, 0x00000000u)
#define VecMaskZ  ARMCreateVecI(0x00000000u, 0x00000000u, 0xFFFFFFFFu, 0x00000000u)
#define VecMaskW  ARMCreateVecI(0x00000000u, 0x00000000u, 0x00000000u, 0xFFFFFFFFu)

// vec(0, 1, 2, 3) -> (vec[x], vec[y], vec[z], vec[w])
#define VecSwizzle(vec, x, y, z, w) ARMVectorSwizzle<x, y, z, w>(vec)

#define VecShuffle(vec1, vec2, x, y, z, w)  ARMVectorShuffle<x, y, z, w>(vec1, vec2)
#define VecShuffleR(vec1, vec2, x, y, z, w) ARMVectorShuffle<w, z, y, x>(vec1, vec2)

// special shuffle
#define VecShuffle_0101(vec1, vec2) vcombine_f32(vget_low_f32(vec1), vget_low_f32(vec2))
#define VecShuffle_2323(vec1, vec2) vcombine_f32(vget_high_f32(vec1), vget_high_f32(vec2))

#define VecAnd(a, b) vandq_u32(vreinterpretq_u32_f32(a), b)
#define VecOr(a, b)  vorrq_u32(vreinterpretq_u32_f32(a), b)
#define VecXor(a, b) veorq_u32(vreinterpretq_u32_f32(a), b)

#define VecMask(a, msk) VecSelect(vdupq_n_f32(0.0f), a, msk)

#define VecMax(a, b) vmaxq_f32(a, b)
#define VecMin(a, b) vminq_f32(a, b)

#define VecCmpGe(a, b) vcgeq_f32(a, b) // greater or equal
#define VecCmpLt(a, b) vcltq_f32(a, b) // less than
#define VecCmpLe(a, b) vcleq_f32(a, b) // less or equal
#define VecMovemask(a) ARMVecMovemask(a) /* not done */

#define VecSelect(V1, V2, Control) vbslq_f32(Control, V2, V1)
#define VecBlend(a, b, Control)    vbslq_f32(Control, b, a)

__forceinline vec_t ARMVector3Load(float* src)
{
    return vcombine_f32(vld1_f32(src), vld1_lane_f32(src + 2, vdup_n_f32(0), 0));
}

__forceinline vec_t ARMCreateVec(float x, float y, float z, float w) {
    float32x2_t V0 = vcreate_f32((uint64_t)__builtin_bit_cast(uint, x) | ((uint64)__builtin_bit_cast(uint, y) << 32));
    float32x2_t V1 = vcreate_f32((uint64_t)__builtin_bit_cast(uint, z) | ((uint64)__builtin_bit_cast(uint, w) << 32));
    return vcombine_f32(V0, V1);
}

__forceinline veci_t ARMCreateVecI(uint x, uint y, uint z, uint w) {
    uint32x2_t V0 = vcreate_s32(((uint64_t)x) | (((uint64_t)y) << 32));
    uint32x2_t V1 = vcreate_s32(((uint64_t)z) | (((uint64_t)w) << 32));
    return vcombine_u32(V0, V1);
}

__forceinline vec_t ARMVector3Norm(vec_t v) {
    float32x4_t vTemp = vmulq_f32(v, v);
    float32x2_t v1 = vget_low_f32(vTemp);
    float32x2_t v2 = vget_high_f32(vTemp);
    v1 = vpadd_f32(v1, v1);
    v2 = vdup_lane_f32(v2, 0);
    v1 = vadd_f32(v1, v2); // Dot3
    v2 = vrsqrte_f32(v1); // Reciprocal sqrt (estimate)
    return vmulq_f32(v, vcombine_f32(v2, v2)); // Normalize
}

__forceinline vec_t ARMVector3Dot(vec_t a, vec_t b) {
    float32x4_t vTemp = vmulq_f32(a, b);
    float32x2_t v1 = vget_low_f32(vTemp);
    float32x2_t v2 = vget_high_f32(vTemp);
    v1 = vpadd_f32(v1, v1);
    v2 = vdup_lane_f32(v2, 0);
    v1 = vadd_f32(v1, v2);
    return vcombine_f32(v1, v1);
}

__forceinline vec_t ARMVector3Length(vec_t v) {
    float32x4_t vTemp = vmulq_f32(v, v);
    float32x2_t v1 = vget_low_f32(vTemp);
    float32x2_t v2 = vget_high_f32(vTemp);
    v1 = vpadd_f32(v1, v1);
    v2 = vdup_lane_f32(v2, 0);
    v1 = vadd_f32(v1, v2);
    const float32x2_t zero = vdup_n_f32(0);
    uint32x2_t VEqualsZero = vceq_f32(v1, zero);
    float32x2_t Result = vrsqrte_f32(v1);
    Result = vmul_f32(v1, Result);
    Result = vbsl_f32(VEqualsZero, zero, Result);
    return vcombine_f32(Result, Result);
}

__forceinline vec_t ARMVectorNorm(vec_t v) {
    float32x4_t vTemp = vmulq_f32(v, v);
    float32x2_t v1 = vget_low_f32(vTemp);
    float32x2_t v2 = vget_high_f32(vTemp);
    v1 = vadd_f32(v1, v2);
    v1 = vpadd_f32(v1, v1);
    v2 = vrsqrte_f32(v1);
    return vmulq_f32(v, vcombine_f32(v2, v2));
}

__forceinline vec_t ARMVectorLength(vec_t v) {
    float32x4_t vTemp = vmulq_f32(v, v);
    float32x2_t v1 = vget_low_f32(vTemp);
    float32x2_t v2 = vget_high_f32(vTemp);
    v1 = vadd_f32(v1, v2);
    v1 = vpadd_f32(v1, v1);
    const float32x2_t zero = vdup_n_f32(0);
    uint32x2_t VEqualsZero = vceq_f32(v1, zero);
    float32x2_t Result = vrsqrte_f32(v1);
    Result = vmul_f32(v1, Result);
    Result = vbsl_f32(VEqualsZero, zero, Result);
    return vcombine_f32(Result, Result);
}

__forceinline vec_t ARMVectorDot(vec_t a, vec_t b) {
    float32x4_t vTemp = vmulq_f32(a, b);
    float32x2_t v1 = vget_low_f32(vTemp);
    float32x2_t v2 = vget_high_f32(vTemp);
    v1 = vadd_f32(v1, v2);
    v1 = vpadd_f32(v1, v1);
    return vcombine_f32(v1, v1);
}

__forceinline vec_t ARMVectorDevide(vec_t V1, vec_t V2) {
    // 2 iterations of Newton-Raphson refinement of reciprocal
    float32x4_t Reciprocal = vrecpeq_f32(V2);
    float32x4_t S = vrecpsq_f32(Reciprocal, V2);
    Reciprocal = vmulq_f32(S, Reciprocal);
    S = vrecpsq_f32(Reciprocal, V2);
    Reciprocal = vmulq_f32(S, Reciprocal);
    return vmulq_f32(V1, Reciprocal);
}

__forceinline int ARMVecMovemask(veci_t v) {
    int res = v[0] & 1; res <<= 1;
    res    |= v[1] & 1; res <<= 1;
    res    |= v[1] & 1; res <<= 1;
    res    |= v[1] & 1;
    return res;
}

template<int E0, int E1, int E2, int E3>
__forceinline vec_t ARMVectorSwizzle(vec_t v)
{
  float a = vgetq_lane_f32(v, E0); 
  float b = vgetq_lane_f32(v, E1); 
  float c = vgetq_lane_f32(v, E2); 
  float d = vgetq_lane_f32(v, E3); 
  return VecSetR(a, b, c, d); 
}

template<int E0, int E1, int E2, int E3>
__forceinline vec_t ARMVectorShuffle(vec_t v0, vec_t v1)
{
  float a = vgetq_lane_f32(v0, E0);
  float b = vgetq_lane_f32(v0, E1);
  float c = vgetq_lane_f32(v1, E2);
  float d = vgetq_lane_f32(v1, E3);
  return VecSetR(a, b, c, d);
}

#else // AX_SUPPORT_SSE
/*//////////////////////////////////////////////////////////////////////////*/
/*                                 No Vector                                */
/*//////////////////////////////////////////////////////////////////////////*/

struct vec_t {
    float x, y, z, w;
          float& operator[](int i)       { return ((float*)this)[i]; }
    const float  operator[](int i) const { return ((float*)this)[i]; }
};

struct veci_t {
    uint x, y, z, w;
          uint& operator[](uint i)       { return ((uint*)this)[i]; }
    const uint  operator[](uint i) const { return ((uint*)this)[i]; }
};

__forceinline vec_t MakeVec4(float x, float y, float z, float w) { return vec_t { x, y, z, w }; }
__forceinline vec_t MakeVec4(float x) { return vec_t { x, x, x, x }; }
__forceinline vec_t MakeVec4(const float* x) { return vec_t { x[0], x[1], x[2], x[3] }; }

__forceinline veci_t MakeVec4i(int x, int y, int z, int w) { return veci_t { x, y, z, w }; }
__forceinline veci_t MakeVec4i(int x) { return veci_t { x, x, x, x }; }

#define VecZero()           MakeVec4(0.0f)
#define VecOne()            MakeVec4(1.0f)
#define VecNegativeOne()    MakeVec4(-1.0f, -1.0f, -1.0f, -1.0f)
#define VecSet1(x)          MakeVec4(x)
#define VecSet(x, y, z, w)  MakeVec4(w, z, y, x)
#define VecSetR(x, y, z, w) MakeVec4(x, y, z, w)
#define VecLoad(x)          MakeVec4(x)
#define VecLoadA(x)         MakeVec4(x)

#define VecStore(ptr, a)       SmallMemCpy(ptr, &a.x, 4 * 4);
#define VecFromInt1(x)         MakeVec4i(x)
#define VecFromInt(x, y, z, w) MakeVec4i(x, y, z, w)
#define VecToInt(x)    BitCast<veci_t>(x)
#define VecFromVeci(x) BitCast<vec_t>(x)

// Getters setters
#define VecGetX(v) v.x
#define VecGetY(v) v.y
#define VecGetZ(v) v.z
#define VecGetW(v) v.w

#define VecSplatX(V1) MakeVec4(V1.x)
#define VecSplatY(V1) MakeVec4(V1.y)
#define VecSplatZ(V1) MakeVec4(V1.z)
#define VecSplatW(V1) MakeVec4(V1.w)

#define VecSetX(v, val) v.x = val
#define VecSetY(v, val) v.y = val
#define VecSetZ(v, val) v.z = val
#define VecSetW(v, val) v.w = val

#define VecSelect1000 MakeVec4i(0xFFFFFFFFu, 0x00000000u, 0x00000000u, 0x00000000u)
#define VecSelect1100 MakeVec4i(0xFFFFFFFFu, 0xFFFFFFFFu, 0x00000000u, 0x00000000u)
#define VecSelect1110 MakeVec4i(0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0x00000000u)
#define VecSelect1011 MakeVec4i(0xFFFFFFFFu, 0x00000000u, 0xFFFFFFFFu, 0xFFFFFFFFu)

#define VecIdentityR0 MakeVec4(1.0f, 0.0f, 0.0f, 0.0f)
#define VecIdentityR1 MakeVec4(0.0f, 1.0f, 0.0f, 0.0f)
#define VecIdentityR2 MakeVec4(0.0f, 0.0f, 1.0f, 0.0f)
#define VecIdentityR3 MakeVec4(0.0f, 0.0f, 0.0f, 1.0f)

#define VecMaskXY MakeVec4i(0xFFFFFFFFu, 0xFFFFFFFFu, 0x00000000u, 0x00000000u)
#define VecMask3  MakeVec4i(0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0x00000000u)
#define VecMaskX  MakeVec4i(0xFFFFFFFFu, 0x00000000u, 0x00000000u, 0x00000000u)
#define VecMaskY  MakeVec4i(0x00000000u, 0xFFFFFFFFu, 0x00000000u, 0x00000000u)
#define VecMaskZ  MakeVec4i(0x00000000u, 0x00000000u, 0xFFFFFFFFu, 0x00000000u)
#define VecMaskW  MakeVec4i(0x00000000u, 0x00000000u, 0x00000000u, 0xFFFFFFFFu)

#define VecSwizzle(vec, x, y, z, w) MakeVec4(vec[x], vec[y], vec[z], vec[w])

// return (vec1[x], vec1[y], vec2[z], vec2[w])
#define VecShuffle(vec1, vec2, x, y, z, w)  MakeVec4(vec1[x], vec1[y], vec2[z], vec2[w])
#define VecShuffleR(vec1, vec2, x, y, z, w) MakeVec4(vec1[w], vec1[z], vec2[y], vec2[x])

// special shuffle
#define VecShuffle_0101(vec1, vec2)  MakeVec4(vec1.x, vec1.x, vec2.y, vec2.y) 
#define VecShuffle_2323(vec1, vec2)  MakeVec4(vec1.z, vec1.z, vec2.w, vec2.w) 

#define VecAdd(a, b) MakeVec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w)
#define VecSub(a, b) MakeVec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w)
#define VecMul(a, b) MakeVec4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w)
#define VecDiv(a, b) MakeVec4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w)

#define VecAddf(a, b) MakeVec4(a.x + b, a.y + b, a.z + b, a.w + b)
#define VecSubf(a, b) MakeVec4(a.x - b, a.y - b, a.z - b, a.w - b)
#define VecMulf(a, b) MakeVec4(a.x * b, a.y * b, a.z * b, a.w * b)
#define VecDivf(a, b) MakeVec4(a.x / b, a.y / b, a.z / b, a.w / b)

#define VecHadd(a, b)    MakeVec4(a.x + b.y, a.z + a.w, b.x + b.y, b.z + b.w)
#define VecFmad(a, b, c) MakeVec4(a.x * b.x + c.x, a.y * b.y + c.y, a.z * b.z + c.z, a.w * b.w + c.w)

#define VecRcp(a) MakeVec4(1.0f / a.x, 1.0f / a.y, 1.0f / a.z, 1.0f / a.w)

#define VecAnd(a, b)    NoVectorAnd(a, b)
#define VecOr(a, b)     NoVectorOr(a, b)
#define VecXor(a, b)    NoVectorXor(a, b)
#define VecMask(a, msk) MakeVec4(msk.x ? a.x : 0.0f, msk.y ? a.y : 0.0f, msk.z ? a.z : 0.0f, msk.w ? a.w : 0.0f)

#define VecMax(a, b)   MakeVec4(MAX(a.x, b.x), MAX(a.y, b.y), MAX(a.z, b.z), MAX(a.w, b.w))
#define VecMin(a, b)   MakeVec4(MIN(a.x, b.x), MIN(a.y, b.y), MIN(a.z, b.z), MIN(a.w, b.w))

#define VecCmpGe(a, b) MakeVec4i(a.x >= b.x, a.y >= b.y, a.z >= b.z, a.w >= b.w) /* greater or equal */
#define VecCmpLt(a, b) MakeVec4i(a.x <  b.x, a.y <  b.y, a.z <  b.z, a.w <  b.w) /* less than */
#define VecCmpLe(a, b) MakeVec4i(a.x <= b.x, a.y <= b.y, a.z <= b.z, a.w <= b.w) /* less or equal */

#define VecDotf(a, b)  (a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w)
#define VecDot(a, b)   MakeVec4(a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w)
#define VecNorm(v)     VecDiv(v, Vec3Len(v))
#define VecLenf(v)     Sqrt(VecDot(v, v))
#define VecLen(v)      MakeVec4(Sqrt(VecDot(v, v)))

#define Vec3Dot(a, b)  (a.x * b.x + a.y * b.y + a.z * b.z)
#define Vec3Norm(v)    VecDiv(v, Vec3Len(v))
#define Vec3Lenf(v)    Sqrt(Vec3Dot(v, v))
#define Vec3Len(v)     MakeVec4(Sqrt(Vec3Dot(v, v)))

#define VecSqrt(a)     MakeVec4(Sqrt(a.x), Sqrt(a.y), Sqrt(a.z), Sqrt(a.w))

//_mm_or_ps(_mm_andnot_ps(Control, V1), _mm_and_ps(V2, Control));
#define VecSelect(V1, V2, Control)  MakeVec4(Control.x ? V2.x : V1.x, Control.y ? V2.y : V1.y, Control.z ? V2.z : V1.z, Control.w ? V2.w : V1.w)
#define VecBlend(a, b, c)           NoVectorSelect(a, b, c) 

__forceinline vec_t NoVectorAnd(vec_t a, vec_t b) {
    veci_t aa = BitCast<veci_t>(a), bb = BitCast<veci_t>(b);
    aa.x &= bb.x; aa.y &= bb.y; aa.z &= bb.z; aa.w &= bb.w;
    return BitCast<vec_t>(aa);
}

__forceinline vec_t NoVectorOr(vec_t a, vec_t b) {
    veci_t aa = BitCast<veci_t>(a), bb = BitCast<veci_t>(b);
    aa.x |= bb.x; aa.y |= bb.y; aa.z |= bb.z; aa.w |= bb.w;
    return BitCast<vec_t>(aa);
}

__forceinline vec_t NoVectorXor(vec_t a, vec_t b) {
    veci_t aa = BitCast<veci_t>(a), bb = BitCast<veci_t>(b);
    aa.x ^= bb.x; aa.y ^= bb.y; aa.z ^= bb.z; aa.w ^= bb.w;
    return BitCast<vec_t>(aa);
}

__forceinline vec_t NoVectorSelect(vec_t a, vec_t b, veci_t c) {
    veci_t ab = BitCast<veci_t>(a);
    veci_t bb = BitCast<veci_t>(b);
    bb.x &=  c.x; bb.y &=  c.y; bb.z &=  c.z; bb.w &=  c.w;
    ab.x &= ~c.x; ab.y &= ~c.y; ab.z &= ~c.z; ab.w &= ~c.w;
    veci_t resb = VecSetR(bb.x | ab.x, bb.y | ab.y, bb.z | ab.z, bb.w | ab.w);
    return BitCast<vec_t>(resb);
}
#endif

__forceinline float VECTORCALL Min3(vec_t ab)
{
    vec_t xy = VecMin(VecSplatX(ab), VecSplatY(ab));
    return VecGetX(VecMin(xy, VecSplatZ(ab)));
}

__forceinline float VECTORCALL Max3(vec_t ab)
{
    vec_t xy = VecMax(VecSplatX(ab), VecSplatY(ab));
    return VecGetX(VecMax(xy, VecSplatZ(ab)));
}

__forceinline void VECTORCALL Vec3Store(float* f, vec_t v)
{
    #if defined(__ARM_NEON__) || defined(_M_ARM64)
    vst1_f32(f, vget_low_f32(v));
    vst1q_lane_f32(f + 2, v, 2);
    #else
    f[0] = VecGetX(v);
    f[1] = VecGetY(v);
    f[2] = VecGetZ(v);
    #endif
}

__forceinline vec_t VECTORCALL Vec3Cross(const vec_t vec0, const vec_t vec1)
{
    #if defined(AX_ARM)
    float32x2_t v1xy = vget_low_f32(vec0);
    float32x2_t v2xy = vget_low_f32(vec1);
    float32x2_t v1yx = vrev64_f32(v1xy);
    float32x2_t v2yx = vrev64_f32(v2xy);
    float32x2_t v1zz = vdup_lane_f32(vget_high_f32(vec0), 0);
    float32x2_t v2zz = vdup_lane_f32(vget_high_f32(vec1), 0);
    int32x4_t FlipY = ARMCreateVecI(0x00000000, 0x80000000, 0x00000000, 0x00000000);
    vec_t vResult = vmulq_f32(vcombine_f32(v1yx, v1xy), vcombine_f32(v2zz, v2yx));
    vResult = vmlsq_f32(vResult, vcombine_f32(v1zz, v1yx), vcombine_f32(v2yx, v2xy));
    vResult = vreinterpretq_f32_u32(veorq_u32(vreinterpretq_u32_f32(vResult), FlipY));
    return vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(vResult), VecMask3));
    #else
    vec_t tmp0 = VecShuffleR(vec0, vec0, 3,0,2,1);
    vec_t tmp1 = VecShuffleR(vec1, vec1, 3,1,0,2);
    vec_t tmp2 = VecMul(tmp0, vec1);
    vec_t tmp3 = VecMul(tmp0, tmp1);
    vec_t tmp4 = VecShuffleR(tmp2, tmp2, 3,0,2,1);
    return VecSub(tmp3, tmp4);
    #endif
}

__forceinline float VECTORCALL VecHSum(vec_t v) {
    v = VecHadd(v, v); // high half -> low half
    v = VecHadd(v, v);
    return VecGetX(v);
}

__forceinline vec_t VECTORCALL VecFabs(vec_t x)
{
    veci_t y = VecCmpLe(x, VecZero());
    return VecSelect(x, VecSub(VecZero(), x), y);
}

__forceinline vec_t VECTORCALL VecCopySign(vec_t x, vec_t y)
{
    veci_t withoutSign  = VecFromInt1(0x7fffffff);
    veci_t signBit      = VecFromInt1(0x80000000);
    veci_t clearedX     = VecAnd(VecToInt(x), withoutSign);
    veci_t signY        = VecAnd(VecToInt(y), signBit);
    veci_t res = VecOr(clearedX, signY);
    return VecFromVeci(res);
}

#if defined(__GNUC__) || defined(__clang__)

inline vec_t VECTORCALL VecSin(vec_t x)
{ 
    vec_t xx = VecMul(x, VecMul(x, x));
    vec_t t  = VecSub(x, VecMul(xx, VecSet1(0.16666666666f))); 
    
    xx = VecMul(x, VecMul(x, x));
    t = VecAdd(t, VecMul(xx, VecSet1(0.00833333333f)));
    
    xx = VecMul(x, VecMul(x, x));
    t  = VecSub(t, VecMul(xx, VecSet1(0.00019841269f)));
    
    xx = VecMul(x, VecMul(x, x));
    t = VecAdd(t, VecMul(xx, VecSet1(2.75573e-06f)));
    return t;
}

inline vec_t VECTORCALL VecCos(vec_t x)
{
    vec_t xx = VecMul(x, x);
    vec_t t  = VecSub(VecSet1(1.0f), VecMul(xx, VecSet1(0.5f))); 
    
    xx = VecMul(x, VecMul(x, x));
    t  = VecAdd(t, VecMul(xx, VecSet1(0.04166666666f)));
    
    xx = VecMul(x, VecMul(x, x));
    t  = VecSub(t, VecMul(xx, VecSet1(0.00138888888f)));
    
    xx = VecMul(x, VecMul(x, x));
    t  = VecAdd(t, VecMul(xx, VecSet1(2.48016e-05f)));
    return t;
}

__forceinline vec_t VECTORCALL VecAtan(vec_t x)
{
    static const float sa1 =  0.99997726f, sa3 = -0.33262347f, sa5  = 0.19354346f,
                       sa7 = -0.11643287f, sa9 =  0.05265332f, sa11 = -0.01172120f;
      
    const vec_t xx = VecMul(x, x);
    // (a9 + x_sq * a11
    vec_t y = VecFmad(xx, VecSet1(sa11), VecSet1(sa9));
    y = VecFmad(xx, y, VecSet1(sa7));
    y = VecFmad(xx, y, VecSet1(sa5));
    y = VecFmad(xx, y, VecSet1(sa3));
    y = VecFmad(xx, y, VecSet1(sa1));
    return VecMul(x, y);
}

inline vec_t VECTORCALL VecAtan2(vec_t y, vec_t x)
{
    vec_t ay = VecFabs(y), ax = VecFabs(x);
    veci_t invert = VecCmpLt(ay, ax);
    vec_t z = VecSelect(VecDiv(ax, ay), VecDiv(ay, ax), invert);
    vec_t th = VecAtan(z);
    th = VecSelect(th, VecSub(VecSet1(PIDiv2), th), invert);
    th = VecSelect(th, VecSub(VecSet1(PI), th), VecCmpLt(x, VecZero()));
    return VecCopySign(th, y);
}

inline vec_t VECTORCALL VecSinCos(vec_t* cv, vec_t x)
{
    vec_t xx = VecMul(x, x);
    vec_t t  = VecSub(VecSet1(1.0f), VecMul(xx, VecSet1(0.5f))); 
    xx = VecMul(x, x);
    vec_t st = VecSub(x, VecMul(xx, VecSet1(0.16666666666f))); 
    
    xx = VecMul(x, x);
    t  = VecAdd(t, VecMul(xx, VecSet1(0.04166666666f)));
    xx = VecMul(x, x);
    st = VecAdd(st, VecMul(xx, VecSet1(0.00833333333f)));
    
    xx = VecMul(x, x);
    t  = VecSub(t, VecMul(xx, VecSet1(0.00138888888f)));
    xx = VecMul(x, x);
    st = VecSub(st, VecMul(xx, VecSet1(0.00019841269f)));
    
    xx = VecMul(x, x);
    t  = VecAdd(t, VecMul(xx, VecSet1(2.48016e-05f)));
    xx = VecMul(x, x);
    st = VecAdd(st, VecMul(xx, VecSet1(2.75573e-06f)));
    *cv = t;
    return st;
}

#else //__clang__ || __gnu

#define VecSin(x)         _mm_sin_ps(x)
#define VecCos(x)         _mm_cos_ps(x)
#define VecAtan(x)        _mm_atan_ps(x)
#define VecAtan2(y, x)    _mm_atan2_ps(y, x)
#define VecSinCos(dst, x) _mm_sincos_ps(dst, x)

#endif //__clang__ || __gnu


#ifdef AX_SUPPORT_AVX2

__forceinline __m256i VECTORCALL AVXSelect(const __m256i V1, const __m256i V2, const __m256i& Control)
{
    return _mm256_blendv_epi8(V1, V2, Control);
}

// from: Faster Population Counts Using AVX2 Instructions resource paper
__forceinline int64 VECTORCALL popcount256_epi64(__m256i v)
{
    const __m256i lookup = _mm256_setr_epi8(0, 1, 1, 2, 1, 2, 2, 3, 1, 2,
        2, 3, 2, 3, 3, 4, 0, 1, 1, 2, 1, 2, 2, 3,
        1, 2, 2, 3, 2, 3, 3, 4);
    const __m256i low_mask = _mm256_set1_epi8(0x0f);
    __m256i lo =  _mm256_and_si256(v, low_mask);
    __m256i hi = _mm256_and_si256(_mm256_srli_epi32(v, 4), low_mask);
    __m256i popcnt1 = _mm256_shuffle_epi8(lookup, lo);
    __m256i popcnt2 = _mm256_shuffle_epi8(lookup, hi);
    __m256i total = _mm256_add_epi8(popcnt1, popcnt2);
    v = _mm256_sad_epu8(total, _mm256_setzero_si256());
    return _mm256_cvtsi256_si32(v) + _mm256_extract_epi64(v, 1) + _mm256_extract_epi64(v, 2) + _mm256_extract_epi64(v, 3);
}

__forceinline __m256i VECTORCALL popcnt256si(__m256i v) // returns 4 64 bit integer that contains pop counts
{
    const __m256i lookup = _mm256_setr_epi8(0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4);
    const __m256i low_mask = _mm256_set1_epi8(0x0f);
    __m256i lo = _mm256_and_si256(v, low_mask);
    __m256i hi = _mm256_and_si256(_mm256_srli_epi32(v, 4), low_mask);
    __m256i popcnt1 = _mm256_shuffle_epi8(lookup, lo);
    __m256i popcnt2 = _mm256_shuffle_epi8(lookup, hi);
    return _mm256_sad_epu8(_mm256_add_epi8(popcnt1, popcnt2), _mm256_setzero_si256());
}

#endif // AX_SUPPORT_AVX2

struct Ray
{
    vec_t origin;
    vec_t direction;
};

inline Ray MakeRay(vec_t o, vec_t d)
{
    return { o, d };
}

AX_END_NAMESPACE 