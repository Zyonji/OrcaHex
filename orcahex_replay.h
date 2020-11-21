#define REPLAY_MAX 10000000

struct replay_state
{
    u32 Version;
    u32 Magic;
    u32 Count;
    char BaseFile[MAX_PATH];
    
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

internal void ClearReplay(replay_state *Replay);
internal void CompressReplay(replay_state *Source, void *Target, u32 *TargetSize);
internal b32 UnpackReplay(replay_state *Target, void *Source, u32 FileSize);