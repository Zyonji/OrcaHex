#define REPLAY_MAX 10000000

struct replay_state
{
    u32 Version;
    u32 Magic;
    u32 Count;
    char BaseFile[260];//MAX_PATH, which is defined as 260
    
    pen_target *Next;
    char Buffer[REPLAY_MAX * sizeof(pen_target)];
};

// NOTE(Zyonji): Each chunk is 16 chars big. For the Base file, the Header plus 16 empty chunks are required.
union replay_chunk
{
    struct
    {
        u32 Version;
        u32 Magic;
        u32 Count;
        char BaseFile[4];
    };
    struct
    {
        u32 SegmentLength;
        r32 Width;
        u32 Mode;
        u8 Color[4];
    };
    struct
    {
        r32 Pressure;
        r32 Radians;
        v2 P;
    };
    char E[16];
};
