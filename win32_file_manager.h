#include "FreeImage.h"

struct initial_state
{
    u32 WindowStyle;// NOTE(Zyonji): DWORD type.
    s32 WindowX;
    s32 WindowY;
    s32 WindowWidth;
    s32 WindowHeight;
    u32 MaxId;
    v4 Color;
    u32 Mode;
    r32 Width;
};

struct image_data
{
    char NameBase[MAX_PATH]; // NOTE(Zyonji): YY-MM-DD_IIIIIIII_?min.jpg
    char ReplayPath[MAX_PATH];
    FIBITMAP *Bitmap;
    void *Memory;
    u32 Id;
    u32 Width;
    u32 Height;
    u32 Minutes;
};

internal void RequestFileLoad(canvas_state *Canvas, u32area PaintingRegion, u32 MaxId);
internal void RequestEmptyFile(u32 MaxId, u32area PaintingRegion, u32 Width, u32 Height);
internal b32 SaveEverything();