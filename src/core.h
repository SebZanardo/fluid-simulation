#ifndef CORE_H
#define CORE_H

// EXTERNAL INCLUDES //////////////////////////////////////////////////////////
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

// BASE ///////////////////////////////////////////////////////////////////////
// Units
#define KiB(n) (((u64)(n)) << 10)
#define MiB(n) (((u64)(n)) << 20)
#define GiB(n) (((u64)(n)) << 30)
#define Thousand(n) ((n) * 1000)
#define Million(n)  ((n) * 1000000)
#define Billion(n)  ((n) * 1000000000)

// Helper macros
#define Min(a, b) (((a)<(b))?(a):(b))
#define Max(a, b) (((a)>(b))?(a):(b))
#define Clamp(v, a, b) (((v)<(a))?(a):((v)>(b))?(b):(v))

#define Mod(x, n) ((((x) % (n)) + (n)) % (n))

#define ArrayCount(a) (sizeof(a) / sizeof((a)[0]))

#define FlagGet(n, flag)    ((n) & (flag))
#define FlagSet(n, flag)    ((n) |= (flag))
#define FlagClear(n, flag)  ((n) &= ~(flag))
#define FlagToggle(n, flag) ((n) ^= (flag))

#define AlignUpPow2(n, p) (((u64)(n) + ((u64)(p) - 1)) & (~((u64)(p) - 1)))

// Unsigned integer types
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// Signed integer types
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

// Boolean types
typedef i8       b8;
typedef i16      b16;
typedef i32      b32;
typedef i64      b64;

// Floating point types
typedef float    f32;
typedef double   f64;

// Constants
static const u64 MAX_U64 = 0XFfffffffffffffffull;
static const u32 MAX_U32 = 0XFfffffff;
static const u16 MAX_U16 = 0XFfff;
static const u8  MAX_U8  = 0XFf;

static const i64 MAX_I64 = (i64)0x7fffffffffffffffll;
static const i32 MAX_I32 = (i32)0x7fffffff;
static const i16 MAX_I16 = (i16)0x7fff;
static const i8  MAX_I8  = (i8) 0x7f;

static const i64 MIN_I64 = (i64)0x8000000000000000ll;
static const i32 MIN_I32 = (i32)0x80000000;
static const i16 MIN_I16 = (i16)0x8000;
static const i8  MIN_I8  = (i8) 0x80;

static const f32 MAX_F32 = 3.402823466e+38F;
static const f64 MAX_F64 = 1.7976931348623158e+308;

static const u32 BITMASK1  = 0x00000001;
static const u32 BITMASK2  = 0x00000003;
static const u32 BITMASK3  = 0x00000007;
static const u32 BITMASK4  = 0x0000000f;
static const u32 BITMASK5  = 0x0000001f;
static const u32 BITMASK6  = 0x0000003f;
static const u32 BITMASK7  = 0x0000007f;
static const u32 BITMASK8  = 0x000000ff;
static const u32 BITMASK9  = 0x000001ff;
static const u32 BITMASK10 = 0x000003ff;
static const u32 BITMASK11 = 0x000007ff;
static const u32 BITMASK12 = 0x00000fff;
static const u32 BITMASK13 = 0x00001fff;
static const u32 BITMASK14 = 0x00003fff;
static const u32 BITMASK15 = 0x00007fff;
static const u32 BITMASK16 = 0x0000ffff;
static const u32 BITMASK17 = 0x0001ffff;
static const u32 BITMASK18 = 0x0003ffff;
static const u32 BITMASK19 = 0x0007ffff;
static const u32 BITMASK20 = 0x000fffff;
static const u32 BITMASK21 = 0x001fffff;
static const u32 BITMASK22 = 0x003fffff;
static const u32 BITMASK23 = 0x007fffff;
static const u32 BITMASK24 = 0x00ffffff;
static const u32 BITMASK25 = 0x01ffffff;
static const u32 BITMASK26 = 0x03ffffff;
static const u32 BITMASK27 = 0x07ffffff;
static const u32 BITMASK28 = 0x0fffffff;
static const u32 BITMASK29 = 0x1fffffff;
static const u32 BITMASK30 = 0x3fffffff;
static const u32 BITMASK31 = 0x7fffffff;
static const u32 BITMASK32 = 0xffffffff;

static const u32 BIT1  = (1<<0);
static const u32 BIT2  = (1<<1);
static const u32 BIT3  = (1<<2);
static const u32 BIT4  = (1<<3);
static const u32 BIT5  = (1<<4);
static const u32 BIT6  = (1<<5);
static const u32 BIT7  = (1<<6);
static const u32 BIT8  = (1<<7);
static const u32 BIT9  = (1<<8);
static const u32 BIT10 = (1<<9);
static const u32 BIT11 = (1<<10);
static const u32 BIT12 = (1<<11);
static const u32 BIT13 = (1<<12);
static const u32 BIT14 = (1<<13);
static const u32 BIT15 = (1<<14);
static const u32 BIT16 = (1<<15);
static const u32 BIT17 = (1<<16);
static const u32 BIT18 = (1<<17);
static const u32 BIT19 = (1<<18);
static const u32 BIT20 = (1<<19);
static const u32 BIT21 = (1<<20);
static const u32 BIT22 = (1<<21);
static const u32 BIT23 = (1<<22);
static const u32 BIT24 = (1<<23);
static const u32 BIT25 = (1<<24);
static const u32 BIT26 = (1<<25);
static const u32 BIT27 = (1<<26);
static const u32 BIT28 = (1<<27);
static const u32 BIT29 = (1<<28);
static const u32 BIT30 = (1<<29);
static const u32 BIT31 = (1<<30);
static const u32 BIT32 = (1<<31);

// ARENA //////////////////////////////////////////////////////////////////////
typedef struct {
    u64 reserve_size;
    u64 commit_size;
    u64 base;
    u64 base_next;
} Arena;

Arena* ArenaCreate(u64 reserve_size, u64 commit_size);
void ArenaDestroy(Arena* arena);
void* ArenaPush(Arena* arena, u64 size, b32 zero);
void ArenaPop(Arena* arena, u64 size);
void ArenaPopTo(Arena* arena, u64 pos);
void ArenaClear(Arena* arena);

#define ArenaPushStruct(arena, T) (T*)ArenaPush((arena), sizeof(T), true)
#define ArenaPushStructNonZero(arena, T) (T*)ArenaPush((arena), sizeof(T), false)
#define ArenaPushArray(arena, T, n) (T*)ArenaPush((arena), sizeof(T) * (n), true)
#define ArenaPushArrayNonZero(arena, T, n) (T*)ArenaPush((arena), sizeof(T) * (n), false)

// MATH ///////////////////////////////////////////////////////////////////////
typedef struct {
    i32 x;
    i32 y;
} IntVector2;

#define abs_i64(v) (i64)llabs(v)

#define sqrt_f32(v)   sqrtf(v)
#define log_f32(v)    logf(v)
#define cbrt_f32(v)   cbrtf(v)
#define mod_f32(a, b) fmodf((a), (b))
#define pow_f32(b, e) powf((b), (e))
#define ceil_f32(v)   ceilf(v)
#define floor_f32(v)  floorf(v)
#define round_f32(v)  roundf(v)
#define abs_f32(v)    fabsf(v)
#define sin_f32(v)    sinf(v)
#define cos_f32(v)    cosf(v)
#define tan_f32(v)    tanf(v)

#define sqrt_f64(v)   sqrt(v)
#define log_f64(v)    log(v)
#define cbrt_f64(v)   cbrt(v)
#define mod_f64(a, b) fmod((a), (b))
#define pow_f64(b, e) pow((b), (e))
#define ceil_f64(v)   ceil(v)
#define floor_f64(v)  floor(v)
#define round_f64(v)  round(v)
#define abs_f64(v)    fabs(v)
#define sin_f64(v)    sin(v)
#define cos_f64(v)    cos(v)
#define tan_f64(v)    tan(v)

// RANDOM /////////////////////////////////////////////////////////////////////
// Based on the PCG random number generator (https://www.pcg-random.org/)
// Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)
typedef struct {
    u64 state;
    u64 inc;
} RNG;

#define PCG32_INITIALIZER { 0x853c49e6748fea9bULL, 0xda3e39cb94b95bdbULL }

void RandomSeed(RNG* rng, u64 initstate, u64 initseq);
u32 Random_u32(RNG* rng);
f32 Random_f32(RNG* rng);
f32 RandomNormBetween(RNG* rng, f32 min, f32 max);
Vector2 RandomCircle(RNG* rng, Vector2 position, f32 radius);

#endif // CORE_H
