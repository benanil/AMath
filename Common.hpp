#ifndef ASTL_COMMON
#define ASTL_COMMON

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <float.h>

typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned int       uint32;
typedef unsigned long long uint64;
typedef char      int8;
typedef short     int16;
typedef int       int32;
typedef long long int64;

typedef uint8_t  uchar;
typedef uint16_t ushort;
typedef uint32_t uint;
typedef uint64_t ulong;

#ifdef AX_EXPORT
	#define AX_API __declspec(dllexport)
#else
	#define AX_API __declspec(dllimport)
#endif

#ifdef _MSC_VER
# // do nothing	it already has __forceinline
#elif __CLANG__
#       define __forceinline [[clang::always_inline]] 
#elif __GNUC__
#ifndef __forceinline
#       define __forceinline inline __attribute__((always_inline))
#endif
#endif

#ifdef _MSC_VER
#   include <intrin.h>
#	define VECTORCALL __vectorcall
#elif __CLANG__
#   define VECTORCALL [[clang::vectorcall]] 
#elif __GNUC__
#   define VECTORCALL  
#endif

#if defined(__GNUC__)
#    define AX_PACK(decl) decl __attribute__((__packed__))
#elif defined(_MSC_VER)
#    define AX_PACK(decl) __pragma(pack(push, 1)) decl __pragma(pack(pop))
#else
#error you should define pack function
#endif

#if defined(__GNUC__) || defined(__MINGW32__)
    #define RESTRICT __restrict__
#elif defined(_MSC_VER)
    #define RESTRICT __restrict
#else
    #define RESTRICT
#endif

#ifndef AXGLOBALCONST
#	if _MSC_VER
#		define AXGLOBALCONST extern const __declspec(selectany)
#	elif defined(__GNUC__) && !defined(__MINGW32__)
#		define AXGLOBALCONST extern const __attribute__((weak))
#   else 
#       define AXGLOBALCONST extern const 
#	endif
#endif

#if defined(__GNUC__) || defined(__INTEL_COMPILER) || defined(__clang__)
#    define AX_LIKELY(x) __builtin_expect(x, 1)  
#    define AX_UNLIKELY(x) __builtin_expect(x, 0)
#else
#    define AX_LIKELY(x) (x)   
#    define AX_UNLIKELY(x) (x) 
#endif

// https://nullprogram.com/blog/2022/06/26/
#if defined(_DEBUG) || defined(Debug)
#  if __GNUC__
#    define ASSERT(c) if (!(c)) __builtin_trap()
#  elif _MSC_VER
#    define ASSERT(c) if (!(c)) __debugbreak()
#  else
#    define ASSERT(c) if (!(c)) *(volatile int *)0 = 0
#  endif
#else
#  define ASSERT(c)
#endif

#if defined(__has_builtin)
#   define AX_COMPILER_HAS_BUILTIN(x) __has_builtin(x)
#else
#   define AX_COMPILER_HAS_BUILTIN(x) 0
#endif

#if AX_COMPILER_HAS_BUILTIN(__builtin_assume)
#   define AX_ASSUME(x) __builtin_assume(x)
#elif defined(_MSC_VER)
#   define AX_ASSUME(x) __assume(x)
#else
#   define AX_ASSUME(x) (void)(x)
#endif

#if AX_COMPILER_HAS_BUILTIN(__builtin_unreachable)
#   define AX_UNREACHABLE() __builtin_unreachable()
#elif _MSC_VER
#   define AX_UNREACHABLE() __assume(0)
#else
#   define AX_UNREACHABLE() 
#endif

#if AX_COMPILER_HAS_BUILTIN(__builtin_prefetch)
#   define AX_PREFETCH(x) __builtin_prefetch(x)
#elif defined(_MSC_VER)
#   define AX_PREFETCH(x) _mm_prefetch(x, _MM_HINT_T0)
#else
#   define AX_PREFETCH(x)
#endif

// https://gist.github.com/boxmein/7d8e5fae7febafc5851e
// https://en.wikipedia.org/wiki/CPUID
// example usage:
// void get_cpu_model(char *cpu_model) { // return example: "AMD Ryzen R5 1600"
//     int* cpumdl = (int*)cpu_model;
//     AX_CPUID(0x80000002, cpumdl); cpumdl += 4;
//     AX_CPUID(0x80000003, cpumdl); cpumdl += 4;
//     AX_CPUID(0x80000004, cpumdl); 
// }
// int arr[4];
// AX_CPUID(1, arr);
// int numCores = (arr[1] >> 16) & 0xff; // virtual cores included
#if defined(__clang__) || defined(__GNUC__)
//#   include <cpuid.h>
//#   define AX_CPUID(num, regs)       __cpuid(num, regs[0], regs[1], regs[2], regs[3])
//#   define AX_CPUID2(num, sub, regs) __cpuid_count(num, sub, regs[0], regs[1], regs[2], regs[3])
#   define AX_CPUID(num, regs)       
#else
#   define AX_CPUID(num, regs) __cpuid(regs, num)
#endif

/* Architecture Detection */
// detection code from mini audio
// you can define AX_NO_SSE2 or AX_NO_AVX2 in order to disable this extensions

#if defined(__x86_64__) || defined(_M_X64)
#   define AX_X64
#elif defined(__i386) || defined(_M_IX86)
#   define AX_X86
#elif defined(_M_ARM) || defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64) || defined(_M_ARM64EC) || __arm__ || __aarch64__
    #define AX_ARM
#endif

#if defined(AX_ARM)
    #if defined(_MSC_VER) && !defined(__clang__) && (defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64) || defined(_M_ARM64EC) || defined(__aarch64__))
        #include <arm64_neon.h>
    #else
        #include <arm_neon.h>
    #endif
#endif

// write AX_NO_SSE2 or AX_NO_AVX2 to disable vector instructions

/* Intrinsics Support */
#if (defined(AX_X64) || defined(AX_X86)) && !defined(AX_ARM)
    #if defined(_MSC_VER) && !defined(__clang__)
        #if _MSC_VER >= 1400 && !defined(AX_NO_SSE2)   /* 2005 */
            #define AX_SUPPORT_SSE
        #endif
        #if _MSC_VER >= 1700 && !defined(AX_NO_AVX2)   /* 2012 */
            #define AX_SUPPORT_AVX2
        #endif
    #else
        #if defined(__SSE2__) && !defined(AX_NO_SSE2)
            #define AX_SUPPORT_SSE
        #endif
        #if defined(__AVX2__) && !defined(AX_NO_AVX2)
            #define AX_SUPPORT_AVX2
        #endif
    #endif
    
    /* If at this point we still haven't determined compiler support for the intrinsics just fall back to __has_include. */
    #if !defined(__GNUC__) && !defined(__clang__) && defined(__has_include)
        #if !defined(AX_SUPPORT_SSE)   && !defined(AX_NO_SSE2)   && __has_include(<emmintrin.h>)
            #define AX_SUPPORT_SSE
        #endif
        #if !defined(AX_SUPPORT_AVX2)   && !defined(AX_NO_AVX2)   && __has_include(<immintrin.h>)
            #define AX_SUPPORT_AVX2
        #endif
    #endif

    #if defined(AX_SUPPORT_AVX2) || defined(AX_SUPPORT_AVX)
        #include <immintrin.h>

    #elif defined(AX_SUPPORT_SSE)
        #include <emmintrin.h>
    #endif
#endif

#ifndef AX_NO_UNROLL
    #if defined(__clang__)
    #   define AX_NO_UNROLL _Pragma("clang loop unroll(disable)") _Pragma("clang loop vectorize(disable)")
    #elif defined(__GNUC__) >= 8
    #   define AX_NO_UNROLL _Pragma("GCC unroll 0")
    #elif defined(_MSC_VER)
    #   define AX_NO_UNROLL __pragma(loop(no_vector))
    #else
    #   define AX_NO_UNROLL
    #endif
#endif

#if defined(__clang__) || defined(__GNUC__)
    #define AX_CPP_VERSION __cplusplus
    #define AX_CPP14 201402L
    #define AX_CPP17 201703L
    #define AX_CPP20 202002L
#elif defined(_MSC_VER)
    #define AX_CPP_VERSION _MSC_VER
    #define AX_CPP14 1900
    #define AX_CPP17 1910
    #define AX_CPP20 1920
#endif

#if AX_CPP_VERSION < AX_CPP14
    // below c++ 14 does not support constexpr functions
    #define __constexpr
#else
    #define __constexpr constexpr
#endif

#if AX_CPP_VERSION >= AX_CPP17
#   define if_constexpr if constexpr
#else
#   define if_constexpr if
#endif

#if AX_CPP_VERSION < AX_CPP14
    // below c++ 14 does not support constexpr
    #define __const const
#else
    #define __const constexpr
#endif

#ifdef _MSC_VER
    #define SmallMemCpy(dst, src, size) __movsb((unsigned char*)dst, (unsigned char*)src, size);
#else
    #define SmallMemCpy(dst, src, size) __builtin_memcpy(dst, src, size);
#endif

#ifdef _MSC_VER
    #define SmallMemSet(dst, val, size) __stosb((unsigned char*)dst, val, size);
#else
    #define SmallMemSet(dst, val, size) __builtin_memset(dst, val, size);
#endif

#define MemsetZero(dst, size) SmallMemSet(dst, 0, size)

// #define AX_USE_NAMESPACE
#ifdef AX_USE_NAMESPACE
#   define AX_NAMESPACE namespace ax {
#   define AX_END_NAMESPACE }
#else
#   define AX_NAMESPACE
#   define AX_END_NAMESPACE
#endif

AX_NAMESPACE

// change it as is mobile maybe?
inline constexpr bool IsAndroid()
{
#ifdef __ANDROID__
    return true;
#else
    return false;
#endif
}

template<typename T, int N>
__constexpr int ArraySize(const T (&)[N]) { return N; }

template<typename T>
__forceinline __constexpr T PopCount(T x)
{
    // according to intel intrinsic, popcnt instruction is 3 cycle (equal to mulps, addps) 
    // throughput is even double of mulps and addps which is 1.0 (%100)
    // https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html
#if defined(__ARM_NEON__)
    return vcnt_u8((int8x8_t)x);
#elif defined(AX_SUPPORT_SSE)
	if_constexpr (sizeof(T) == 4) return _mm_popcnt_u32(x);
    else if (sizeof(T) == 8) return _mm_popcnt_u64(x);
#elif defined(__GNUC__) || !defined(__MINGW32__)
	if_constexpr (sizeof(T) == 4) return __builtin_popcount(x);
	else if (sizeof(T) == 8) return __builtin_popcountl(x);
#else
	if_constexpr (sizeof(T) == 4)
	{
		x =  x - ((x >> 1) & 0x55555555);        // add pairs of bits
		x = (x & 0x33333333) + ((x >> 2) & 0x33333333);  // quads
		x = (x + (x >> 4)) & 0x0F0F0F0F;        // groups of 8
		return (x * 0x01010101) >> 24;          // horizontal sum of bytes	
	}
	else if (sizeof(T) == 8) // standard popcount; from wikipedia
	{
		x -= ((x >> 1) & 0x5555555555555555ull);
		x = (x & 0x3333333333333333ull) + (x >> 2 & 0x3333333333333333ull);
		return ((x + (x >> 4)) & 0xf0f0f0f0f0f0f0full) * 0x101010101010101ull >> 56;
	}
#endif
    ASSERT(0);
}

template<typename T>
__forceinline __constexpr T TrailingZeroCount(T x) 
{
#ifdef _MSC_VER
	if_constexpr (sizeof(T) == 4) return _tzcnt_u32(x);
	else if (sizeof(T) == 8) return _tzcnt_u64(x);
#elif defined(__GNUC__) || !defined(__MINGW32__)
	if_constexpr (sizeof(T) == 4) return __builtin_ctz(x);
	else if (sizeof(T) == 8) return __builtin_ctzll(x);
#else
	return PopCount((x & -x) - 1);
#endif
}

template<typename T>
__forceinline __constexpr T LeadingZeroCount(T x)
{
#ifdef _MSC_VER
	if_constexpr (sizeof(T) == 4) return _lzcnt_u32(x);
	else if (sizeof(T) == 8) return _lzcnt_u64(x);
#elif defined(__GNUC__) || !defined(__MINGW32__)
	if_constexpr (sizeof(T) == 4) return __builtin_clz(x);
	else if (sizeof(T) == 8) return __builtin_clzll(x);
#else
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	return 32 - PopCount(x);
#endif
}

__forceinline __constexpr int NextSetBit(int* bits)
{
    *bits &= ~1;
    int tz = TrailingZeroCount(*bits);
    *bits >>= tz;
    return tz;
}

__forceinline __constexpr bool EnumHasBit(int _enum, int bit)
{
    return !!(_enum & bit);
}

template<typename To, typename From>
__forceinline __constexpr To BitCast(const From& _Val) 
{
#if AX_CPP_VERSION < AX_CPP17
  return *reinterpret_cast<const To*>(&_Val);
#else
  return __builtin_bit_cast(To, _Val);
#endif
}

#ifndef MIN
    #if AX_CPP_VERSION >= AX_CPP17
    template<typename T> __forceinline __constexpr T MIN(T a, T b) { return a < b ? a : b; }
    template<typename T> __forceinline __constexpr T MAX(T a, T b) { return a > b ? a : b; }
    #else
    // using macro if less than 17 because we want this to be constexpr
    #   ifndef MIN
    #       define MIN(a, b) ((a) < (b) ? (a) : (b))
    #       define MAX(a, b) ((a) > (b) ? (a) : (b))
    #   endif
    #endif
#endif

template<typename T> __forceinline __constexpr T Clamp(T x, T a, T b) { return MAX(a, MIN(b, x)); }

__forceinline __constexpr int64_t Abs(int64_t x) 
{
    return x & ~(1ull << 63ull);
}

__forceinline __constexpr int Abs(int x)
{
    return x & ~(1 << 31);
}

__forceinline __constexpr float Abs(float x)
{
    int ix = BitCast<int>(x) & 0x7FFFFFFF; // every bit except sign mask
    return BitCast<float>(ix);
}

__forceinline __constexpr double Abs(double x)
{
    uint64_t  ix = BitCast<uint64_t >(x) & (~(1ull << 63ull));// every bit except sign mask
    return BitCast<double>(ix);
}

template<typename T> __forceinline __constexpr 
bool IsPowerOfTwo(T x) { return (x != 0) && ((x & (x - 1)) == 0); }

__forceinline __constexpr int NextPowerOf2(int x)
{
    x--;
    x |= x >> 1; x |= x >> 2; x |= x >> 4;
    x |= x >> 8; x |= x >> 16;
    return ++x;
}

__forceinline __constexpr int64_t NextPowerOf2(int64_t x)
{
    x--;
    x |= x >> 1; x |= x >> 2;  x |= x >> 4;
    x |= x >> 8; x |= x >> 16; x |= x >> 32;
    return ++x;
}

AX_END_NAMESPACE

#endif // ASTL_COMMON
