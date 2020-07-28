#include <intrin.h>
#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <float.h>
#include "math.h"

#define internal static
#define local_persist static
#define global static

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef intptr_t intptr;
typedef uintptr_t uintptr;

typedef size_t memory_index;

typedef float real32;
typedef double real64;

typedef int8 s8;
typedef int8 s08;
typedef int16 s16;
typedef int32 s32;
typedef int64 s64;
typedef bool32 b32;

typedef uint8 u8;
typedef uint8 u08;
typedef uint16 u16;
typedef uint32 u32;
typedef uint64 u64;

typedef real32 r32;
typedef real64 r64;
typedef real32 f32;
typedef real64 f64;

typedef uintptr_t umm;
typedef intptr_t smm;

#define U16Max 65535
#define S32Min ((s32)0x80000000)
#define S32Max ((s32)0x7fffffff)
#define U32Min 0
#define U32Max ((u32)-1)
#define U64Max ((u64)-1)
#define F32Max FLT_MAX
#define F32Min -FLT_MAX

#define Pi32 3.14159265359f

#define U32FromPointer(Pointer) ((u32)(memory_index)(Pointer))
#define PointerFromU32(type, Value) (type *)((memory_index)Value)

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))
#define OffsetOf(type, Member) (umm)&(((type *)0)->Member)

#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}

#define Minimum(A, B) ((A < B) ? (A) : (B))
#define Maximum(A, B) ((A > B) ? (A) : (B))

inline f32
SquareRoot(f32 Real32)
{
    f32 Result = _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(Real32)));
    return(Result);
}

union v2
{
    struct
    {
        r32 x, y;
    };
    struct
    {
        r32 u, v;
    };
    struct
    {
        r32 Width, Height;
    };
    r32 E[2];
};

union v2s
{
    struct
    {
        s32 x, y;
    };
    struct
    {
        s32 Width, Height;
    };
    s32 E[2];
};

union v2u
{
    struct
    {
        u32 x, y;
    };
    struct
    {
        u32 Width, Height;
    };
    u32 E[2];
};

union u32area
{
    struct
    {
        u32 x, y, Width, Height;
    };
    struct
    {
        u32 Left, Bottom, Ignored0_, Ignored1_;
    };
    struct
    {
        v2u Origin;
        v2u Size;
    };
    struct
    {
        v2u xy;
        v2u Ignored2_;
    };
    u32 E[4];
};

union v3
{
    struct
    {
        r32 x, y, z;
    };
    struct
    {
        r32 u, v, __;
    };
    struct
    {
        r32 r, g, b;
    };
    struct
    {
        v2 xy;
        r32 Ignored0_;
    };
    struct
    {
        r32 Ignored1_;
        v2 yz;
    };
    struct
    {
        v2 uv;
        r32 Ignored2_;
    };
    struct
    {
        r32 Ignored3_;
        v2 v__;
    };
    r32 E[3];
};

union v4
{
    struct
    {
        union
        {
            v3 xyz;
            struct
            {
                r32 x, y, z;
            };
        };
        
        r32 w;
    };
    struct
    {
        union
        {
            v3 rgb;
            struct
            {
                r32 r, g, b;
            };
        };
        
        r32 a;
    };
    struct
    {
        v2 xy;
        r32 Ignored0_;
        r32 Ignored1_;
    };
    struct
    {
        r32 Ignored2_;
        v2 yz;
        r32 Ignored3_;
    };
    struct
    {
        r32 Ignored4_;
        r32 Ignored5_;
        v2 zw;
    };
    struct
    {
        r32 Right;
        r32 Left;
        r32 Top;
        r32 Bottom;
    };
    r32 E[4];
};

struct m4x4
{
    // NOTE(Zyonji): E[row][column]
    f32 E[4][4];
};

inline v2
Perp(v2 A)
{
    v2 Result = {-A.y, A.x};
    return(Result);
}

inline v2
operator*(real32 A, v2 B)
{
    v2 Result;
    
    Result.x = A*B.x;
    Result.y = A*B.y;
    
    return(Result);
}

inline v2
operator*(v2 B, real32 A)
{
    v2 Result = A*B;
    
    return(Result);
}

inline v2 &
operator*=(v2 &B, real32 A)
{
    B = A * B;
    
    return(B);
}

inline v2
operator-(v2 A)
{
    v2 Result;
    
    Result.x = -A.x;
    Result.y = -A.y;
    
    return(Result);
}

inline v2
operator+(v2 A, v2 B)
{
    v2 Result;
    
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    
    return(Result);
}

inline v2 &
operator+=(v2 &A, v2 B)
{
    A = A + B;
    
    return(A);
}

inline v2
operator-(v2 A, v2 B)
{
    v2 Result;
    
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    
    return(Result);
}

inline v2 &
operator-=(v2 &A, v2 B)
{
    A = A - B;
    
    return(A);
}

inline v2
Hadamard(v2 A, v2 B)
{
    v2 Result = {A.x*B.x, A.y*B.y};
    
    return(Result);
}

inline r32
Inner(v2 A, v2 B)
{
    r32 Result = A.x*B.x + A.y*B.y;
    
    return(Result);
}

inline r32
LengthSq(v2 A)
{
    r32 Result = Inner(A, A);
    
    return(Result);
}

inline r32
Length(v2 A)
{
    r32 Result = SquareRoot(LengthSq(A));
    return(Result);
}

inline v2
Normalize(v2 A)
{
    v2 Result = A * (1.0f / Length(A));
    
    return(Result);
}

inline b32
Inside(u32area Area, v2 P)
{
    b32 Result = false;
    if(P.x >= Area.x && P.x < Area.x + Area.Width && P.y >= Area.y && P.y < Area.y + Area.Height)
    {
        Result = true;
    }
    return(Result);
}

inline v3
operator*(real32 A, v3 B)
{
    v3 Result;
    
    Result.x = A*B.x;
    Result.y = A*B.y;
    Result.z = A*B.z;
    
    return(Result);
}

inline v3
operator*(v3 B, real32 A)
{
    v3 Result = A*B;
    
    return(Result);
}

inline v3 &
operator*=(v3 &B, real32 A)
{
    B = A * B;
    
    return(B);
}

inline v3
operator/(v3 B, real32 A)
{
    v3 Result = (1.0f/A)*B;
    
    return(Result);
}

inline v3
operator/(f32 B, v3 A)
{
    v3 Result =
    {
        B / A.x,
        B / A.y,
        B / A.z,
    };
    
    return(Result);
}

inline v3 &
operator/=(v3 &B, real32 A)
{
    B = B / A;
    
    return(B);
}

inline v3
operator-(v3 A)
{
    v3 Result;
    
    Result.x = -A.x;
    Result.y = -A.y;
    Result.z = -A.z;
    
    return(Result);
}

inline v3
operator+(v3 A, v3 B)
{
    v3 Result;
    
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;
    
    return(Result);
}

inline v3 &
operator+=(v3 &A, v3 B)
{
    A = A + B;
    
    return(A);
}

inline v3
operator-(v3 A, v3 B)
{
    v3 Result;
    
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;
    
    return(Result);
}

inline v3 &
operator-=(v3 &A, v3 B)
{
    A = A - B;
    
    return(A);
}

inline v3
Hadamard(v3 A, v3 B)
{
    v3 Result = {A.x*B.x, A.y*B.y, A.z*B.z};
    
    return(Result);
}

inline real32
Inner(v3 A, v3 B)
{
    real32 Result = A.x*B.x + A.y*B.y + A.z*B.z;
    
    return(Result);
}

inline v3
Cross(v3 A, v3 B)
{
    v3 Result;
    
    Result.x = A.y*B.z - A.z*B.y;
    Result.y = A.z*B.x - A.x*B.z;
    Result.z = A.x*B.y - A.y*B.x;
    
    return(Result);
}

inline real32
LengthSq(v3 A)
{
    real32 Result = Inner(A, A);
    
    return(Result);
}

inline real32
Length(v3 A)
{
    real32 Result = SquareRoot(LengthSq(A));
    return(Result);
}

inline v3
Normalize(v3 A)
{
    v3 Result = A * (1.0f / Length(A));
    
    return(Result);
}
