#define PEN_BRUSH_STYLE          0x000F
#define COLOR_MODE_LUMINANCE     0x0010
#define COLOR_MODE_CHROMA        0x0020
#define COLOR_MODE_ALPHA         0x0040
#define COLOR_MODE_PRESSURE      0x0080
#define TILT_MODE_DISABLE        0x0100
#define PEN_BUTTON_MODE_FLIPPED  0x0200
#define MENU_MODE_AB             0x0400
#define PEN_MODE_AREA_FILL       0x0800
#define PEN_MODE_RESIZE          0x1000

struct menu_state
{
    v2u Size;
    v2 Origin;
    b32 ABMode;
    
    u32 Block;
    u32 Gap;
    u32 Steps;
    v2s Offset;
    u32area New;
    u32area Open;
    u32area Save;
    u32area Time;
    u32area RotateR;
    u32area RotateL;
    u32area OneToOne;
    u32area Minus;
    u32area Plus;
    u32area Reset;
    u32area Small;
    u32area Big;
    u32area Mirror;
    u32area PenButtonsToggle;
    u32area TiltToggle;
    u32area AreaFillToggle;
    u32area ResizeToggle;
    u32area BrushStyleSwitch;
    u32area ColorA;
    u32area ColorB;
    u32area ColorButtonA;
    u32area ColorButtonB;
    u32area Alpha;
    u32area LSmooth;
    u32area L;
    u32area a;
    u32area b;
    u32area AlphaButton;
    u32area LButton;
    u32area abButton;
    u32area chButton;
};
struct resize_state
{
    r32 Right;
    r32 Left;
    r32 Top;
    r32 Bottom;
    u32 Edge;
    b32 Active;
};
struct pen_base
{
    v2 P;
    r32 Pressure;
    r32 Radians;
    r32 Width;
};
struct pen_target : pen_base
{
    v4 Color;
    u32 Mode;
};
struct pen_state : pen_target
{
    pen_base Delta;
    v2 Point;
    r32 PointRadians;
    v2 Origin;
    v2 Spread;
    r32 NextWidth;
    u32 Buttons;
    b32 IsDown;
    resize_state Resize;
};
struct canvas_state
{
    v2u Size;
    v2u ReferenceSize;
    r32 Scale;
    r32 RadiansMap;
    v2 XMap;
    v2 YMap;
    v2 Center;
    b32 Mirrored;
    b32 Hot;
    u64 StartingTime;
    s32 SecondsIdle;
};
#define PICK_COLOR(name) v4 name(v2 P)
typedef PICK_COLOR(pick_color);
#define UPDATE_MENU(name) void name(menu_state *Menu, v4 Color, u32 ColorMode)
typedef UPDATE_MENU(update_menu);
#define RENDER_BRUSH_STROKE(name) void name(canvas_state Canvas, pen_target OldPen, pen_target NewPen)
typedef RENDER_BRUSH_STROKE(render_brush_stroke);
#define RENDER_AREA_FLIP(name) void name(canvas_state Canvas, pen_target OldPen, pen_target NewPen, v2 Origin, v2 Spread, v4 Color, u32 Step)
typedef RENDER_AREA_FLIP(render_area_flip);
#define RESIZE_CANVAS(name) void name(canvas_state *Canvas, resize_state Resize)
typedef RESIZE_CANVAS(resize_canvas);

#include "orcahex_replay.h"
#include "win32_file_manager.h"

struct orca_state
{
    v2u DisplaySize;
    u32area PaintingRegion;
    menu_state Menu;
    pen_state Pen;
    canvas_state Canvas;
    u32 MaxId;
    
    replay_state Replay;
    
    pick_color *PickColor;
    update_menu *UpdateMenu;
    render_brush_stroke *RenderBrushStroke;
    render_area_flip *RenderAreaFlip;
    resize_canvas *ResizeCanvas;
};

// NOTE(Zyonji): Functions defined in the platform layer.
internal void LogError(char *Text, char *Caption);

internal PICK_COLOR(CPUPickColor)
{
    v4 Result = {};
    LogError("The undefined default color picking function was used.", "OrcaHex");
    return(Result);
}
internal UPDATE_MENU(CPUUpdateMenu)
{
    LogError("The undefined default menu updating function was used.", "OrcaHex");
}
internal RENDER_BRUSH_STROKE(CPURenderBrushStroke)
{
    LogError("The undefined default brush stroke rendering function was used.", "OrcaHex");
}
internal RENDER_AREA_FLIP(CPURenderAreaFlip)
{
    LogError("The undefined default area flip rendering function was used.", "OrcaHex");
}
internal RESIZE_CANVAS(CPUResizeCanvas)
{
    LogError("The undefined default resisize canvas function was used.", "OrcaHex");
}

#if 0
// NOTE(Zyonji): This uses  2.2 and 2.4 exponents as approximation.  This is mainly visible in the darker ranges.
internal v3
ConvertRGBtoXYZ(v3 RGB)
{
    v3 Result;
    Result = {Inner(RGB, { 20416.0 / 41085.0,  2533.0 /  7470.0,   245.0 /  1494.0}), Inner(RGB, {   319.0 /  1245.0,  2533.0 /  3735.0,    49.0 /   747.0}), Inner(RGB, {   957.0 / 41085.0,  2533.0 / 22410.0,  3871.0 /  4482.0})};
    return(Result);
}
internal v3
ConvertXYZtoRGB(v3 XYZ)
{
    v3 Result;
    Result = {Inner(XYZ, {    78.0 /    29.0,   -37.0 /    29.0,   -12.0 /    29.0}),Inner(XYZ, { -2589.0 /  2533.0,  5011.0 /  2533.0,   111.0 /  2533.0}), Inner(XYZ, {     3.0 /    49.0,   -11.0 /    49.0,    57.0 /    49.0})};
    return(Result);
}
internal v3
ConvertRGBToLab(v3 RGB)
{
    v3 Result = {};
    v3 Linear = {(r32)pow(RGB.r, 2.2f), (r32)pow(RGB.g, 2.2f), (r32)pow(RGB.b, 2.2f)};
    v3 XYZ = ConvertRGBtoXYZ(Linear);
    v3 Cubic = {(r32)pow(XYZ.x, 1.0 / 2.4f), (r32)pow(XYZ.y, 1.0 / 2.4f), (r32)pow(XYZ.z, 1.0 / 2.4f)};
    Result = {Cubic.y, Cubic.x - Cubic.y, Cubic.y - Cubic.z};
    return(Result);
}
internal v3
ConvertLabToRGB(v3 Lab)
{
    v3 Result = {};
    v3 Cubic = {Lab.y + Lab.x, Lab.x, Lab.x - Lab.z};
    v3 XYZ = {(r32)pow(Cubic.x, 3), (r32)pow(Cubic.y, 3), (r32)pow(Cubic.z, 3)};
    v3 Linear = ConvertXYZtoRGB(XYZ);
    Result = {(r32)pow(Linear.r, 1.0 / 2.4f), (r32)pow(Linear.g, 1.0 / 2.4f), (r32)pow(Linear.b, 1.0 / 2.4f)};
    return(Result);
}
internal v4
GetABLimits(v3 RGB)
{
    v4 Result = {};
    v3 Linear = {(r32)pow(RGB.r, 2.2f), (r32)pow(RGB.g, 2.2f), (r32)pow(RGB.b, 2.2f)};
    r32 Y = Inner(Linear, {   319.0 /  1245.0,  2533.0 /  3735.0,    49.0 /   747.0});
    v3 Color[] = {
        {Y / (319.0f / 1245.0f), 0, 0},
        {0, Y / (2533.0f / 3735.0f), 0},
        {0, 0, Y / (49.0f / 747.0f)},
        {(Y - 2533.0f / 3735.0f) / (319.0f / 1245.0f), 1, 0},
        {0, (Y - 49.0f / 747.0f) / (2533.0f / 3735.0f), 1},
        {1, 0, (Y - 319.0f / 1245.0f) / (49.0f / 747.0f)},
        {(Y - 49.0f / 747.0f) / (319.0f / 1245.0f), 0, 1},
        {1, (Y - 319.0f / 1245.0f) / (2533.0f / 3735.0f), 0},
        {0, 1, (Y - 2533.0f / 3735.0f) / (49.0f / 747.0f)},
        {(Y - 2533.0f / 3735.0f - 49.0f / 747.0f) / (319.0f / 1245.0f), 1, 1},
        {1, (Y - 49.0f / 747.0f - 319.0f / 1245.0f) / (2533.0f / 3735.0f), 1},
        {1, 1, (Y - 319.0f / 1245.0f - 2533.0f / 3735.0f) / (49.0f / 747.0f)},
    };
    b32 Empty = true;
    for(u32 I = 0; I < 12; ++I)
    {
        if(Color[I].r >= 0 && Color[I].r <= 1 && Color[I].g >= 0 && Color[I].g <= 1 && Color[I].b >= 0 && Color[I].b <= 1)
        {
            v3 Lab = ConvertRGBToLab(Color[I]);
            if(Lab.y < Result.x || Empty)
            {
                Result.x = Lab.y;
            }
            if(Lab.y > Result.y || Empty)
            {
                Result.y = Lab.y;
            }
            if(Lab.z < Result.z || Empty)
            {
                Result.z = Lab.z;
            }
            if(Lab.z > Result.w || Empty)
            {
                Result.w = Lab.z;
            }
            Empty = false;
        }
    }
    return(Result);
}

#else

internal v3
ConvertRGBToLab(v3 RGB)
{
    v3 Result;
    
    r64 r = RGB.r;
    r64 g = RGB.g;
    r64 b = RGB.b;
    
    r = (r > 0.04045) ? pow((r + 0.055) / 1.055, 2.4) : (r / 12.92);
    g = (g > 0.04045) ? pow((g + 0.055) / 1.055, 2.4) : (g / 12.92);
    b = (b > 0.04045) ? pow((b + 0.055) / 1.055, 2.4) : (b / 12.92);
    
    r64 x = r * 0.4124564 + g * 0.3575761 + b * 0.1804375;
    r64 y = r * 0.2126729 + g * 0.7151522 + b * 0.0721750;
    r64 z = r * 0.0193339 + g * 0.1191920 + b * 0.9503041;
    
    x = x / 0.95047;
    y = y / 1.0000;
    z = z / 1.08883;
    
    x = (x > 0.008856) ? pow(x, 1.0 / 3.0) : (7.787 * x + 0.160 / 1.160);
    y = (y > 0.008856) ? pow(y, 1.0 / 3.0) : (7.787 * y + 0.160 / 1.160);
    z = (z > 0.008856) ? pow(z, 1.0 / 3.0) : (7.787 * z + 0.160 / 1.160);
    
    Result.x = (r32)((1.160 * y) - 0.160);
    Result.y = (r32)(5.000 * (x - y));
    Result.z = (r32)(2.000 * (y - z));
    
    return(Result);
}
internal v3
ConvertLabToRGB(v3 Lab)
{
    v3 Result;
    
    r64 y = (Lab.x + 0.160) / 1.160;
    r64 x = Lab.y / 5.000 + y;
    r64 z = y - Lab.z / 2.000;
    
    r64 x3 = x * x * x;
    r64 y3 = y * y * y;
    r64 z3 = z * z * z;
    
    x = ((x3 > 0.008856) ? x3 : ((x - 0.160 / 1.160) / 7.787)) * 0.95047;
    y = ((y3 > 0.008856) ? y3 : ((y - 0.160 / 1.160) / 7.787)) * 1.000;
    z = ((z3 > 0.008856) ? z3 : ((z - 0.160 / 1.160) / 7.787)) * 1.08883;
    
    r64 r = x *  3.2404542 + y * -1.5371385 + z * -0.4985314;
    r64 g = x * -0.9692660 + y *  1.8760108 + z *  0.0415560;
    r64 b = x *  0.0556434 + y * -0.2040259 + z *  1.0572252;
    
    r = (r > 0.0031308) ? (1.055 * pow(r, 1 / 2.4) - 0.055) : (12.92 * r);
    g = (g > 0.0031308) ? (1.055 * pow(g, 1 / 2.4) - 0.055) : (12.92 * g);
    b = (b > 0.0031308) ? (1.055 * pow(b, 1 / 2.4) - 0.055) : (12.92 * b);
    
    Result.r = (r32)r;
    Result.g = (r32)g;
    Result.b = (r32)b;
    
    return(Result);
}
internal r32
GetABLimit(v3 Lab)
{
    r32 Result = 1.35f;
    return(Result);
}
#endif

internal v3
ValidatedRGBfromLab(v3 Lab)
{
    v3 Result;
    
    v3 RGB = ConvertLabToRGB(Lab);
    v3 Grey = ConvertLabToRGB({Lab.x, 0, 0});
    v3 RGBVector = Grey - RGB;
    v3 ScaleA = {RGBVector.x / Grey.x, RGBVector.y / Grey.y, RGBVector.z / Grey.z};
    v3 ScaleB = {-RGBVector.x / (1 - Grey.x), -RGBVector.y / (1 - Grey.y), -RGBVector.z / (1 - Grey.z)};
    r32 Distance = Maximum(Maximum(Maximum(ScaleA.x, ScaleB.x), Maximum(ScaleA.y, ScaleB.y)), Maximum(ScaleA.z, ScaleB.z));
    
    if(Lab.x <= 0)
    {
        Result = {0, 0, 0};
    }
    else if(Lab.x >= 1)
    {
        Result = {1, 1, 1};
    }
    else if(Distance > 1.0)
    {
        Result = Grey - (RGBVector / Distance);
    }
    else
    {
        Result = RGB;
    }
    
    return(Result);
}

internal void
ResetCanvasTransform(canvas_state *Canvas, u32area PaintingRegion)
{
    Canvas->Scale = 1 / (r32)(Maximum(Canvas->Size.Width / PaintingRegion.Width, Canvas->Size.Height / PaintingRegion.Height) + 1);
    Canvas->RadiansMap = 0;
    Canvas->XMap = {1, 0};
    Canvas->YMap = {0, 1};
    Canvas->Center = {0, 0};
    Canvas->Mirrored = false;
}

internal v2
MapFrameToCanvas(v2 P, v2u DisplaySize, u32area PaintingRegion, canvas_state Canvas)
{
    v2 Result = {};
    v2 PaintingRegionCenterOffset = {
        0.5f * (r32)(DisplaySize.Width  - PaintingRegion.Width ), 
        0.5f * (r32)(DisplaySize.Height - PaintingRegion.Height)
    };
    v2 Offset = P - PaintingRegionCenterOffset - Canvas.Center;
    Result.x = Inner(Canvas.XMap, Offset) / Canvas.Scale;
    Result.y = Inner(Canvas.YMap, Offset) / Canvas.Scale;
    return(Result);
}

internal r32
MapFrameToCanvas(r32 Radians, canvas_state Canvas)
{
    r32 Result = Radians;
    if(Canvas.Mirrored)
    {
        Result = -Result;
    }
    Result += Canvas.RadiansMap;
    return(Result);
}

internal void
ChangeCanvasScale(canvas_state *Canvas, r32 Factor)
{
    r32 InverseScale = 1.0f / (Factor * Canvas->Scale);
    if(Canvas->Scale < 1 && Factor > 1)
    {
        InverseScale= floorf(InverseScale);
    }
    if(InverseScale > 1 && Factor < 1)
    {
        InverseScale = ceilf(InverseScale);
    }
    Canvas->Center = (1.0f / (InverseScale * Canvas->Scale)) * Canvas->Center;
    Canvas->Scale = 1.0f / InverseScale;
}
internal void
RotateCanvas(canvas_state *Canvas, r32 Radians)
{
    if(Canvas->Mirrored)
    {
        Canvas->RadiansMap += Radians;
        Canvas->XMap = {-cosf(Canvas->RadiansMap), sinf(Canvas->RadiansMap)};
        Canvas->YMap = { sinf(Canvas->RadiansMap), cosf(Canvas->RadiansMap)};
    }
    else
    {
        Canvas->RadiansMap -= Radians;
        Canvas->XMap = { cosf(Canvas->RadiansMap), sinf(Canvas->RadiansMap)};
        Canvas->YMap = {-sinf(Canvas->RadiansMap), cosf(Canvas->RadiansMap)};
    }
    v2 TempX = { cosf(Radians), sinf(Radians)};
    v2 TempY = {-sinf(Radians), cosf(Radians)};
    Canvas->Center = {Inner(Canvas->Center, TempX), Inner(Canvas->Center, TempY)};
}
internal void
FlipCanvas(canvas_state *Canvas)
{
    Canvas->XMap.x = -Canvas->XMap.x;
    Canvas->YMap.x = -Canvas->YMap.x;
    Canvas->Center.x = -Canvas->Center.x;
    Canvas->Mirrored = !Canvas->Mirrored;
}
internal void
ChangeBrushSize(pen_state *Pen, r32 Factor)
{
    Pen->NextWidth = Factor * Pen->NextWidth;
    if(Pen->NextWidth < 0.5)
    {
        Pen->NextWidth = 0.5;
    }
}
#define BRUSH_STEP_FACTOR          4.0f
internal void
SetRelativeBrushSize(pen_state *Pen, r32 Factor)
{
    r32 Step = (r32)pow(BRUSH_STEP_FACTOR, 0.5f);
    r32 Margin = (r32)pow(BRUSH_STEP_FACTOR, 0.25f);
    r32 Midpoint = 1;
    r32 OldWidth = Pen->NextWidth;
    r32 Offset = 0;
    
    while(OldWidth > Midpoint * Step * Margin)
    {
        Midpoint *= BRUSH_STEP_FACTOR;
        Offset += 1;
    }
    
    if(OldWidth > Midpoint * Margin && Factor < 0.5f)
        Offset += 1;
    
    Pen->NextWidth = (r32)pow(BRUSH_STEP_FACTOR, Factor + Offset - 0.5f);
}

internal b32 // NOTE(Zyonji): true if the the frame needs to be updated.
ProcessBrushMove(orca_state *OrcaState, pen_target PenTarget, v2 Point, u32 Buttons, b32 IsPenClose, b32 FreshClick, b32 RecordReplay)
{
    pen_state *PenState = &OrcaState->Pen;
    b32 PickColorButton;
    b32 MoveCanvasButton;
    if(PenTarget.Mode & PEN_BUTTON_MODE_FLIPPED)
    {
        PickColorButton = (0x4 & Buttons) && !(0x4 & PenState->Buttons);
        MoveCanvasButton = (0x2 & Buttons);
    }
    else
    {
        PickColorButton = (0x2 & Buttons) && !(0x2 & PenState->Buttons);
        MoveCanvasButton = (0x4 & Buttons);
    }
    
    if(Point.y + OrcaState->Menu.Origin.y <= OrcaState->Menu.Size.Height && Point.x + OrcaState->Menu.Origin.x <= OrcaState->Menu.Size.Width  && PenState->Pressure == 0)
    {
        menu_state Menu = OrcaState->Menu;
        v2 P = {Point.x + OrcaState->Menu.Origin.x, Point.y + OrcaState->Menu.Origin.y};
        
        if(PickColorButton)
        {
            if(P.x >= Menu.Alpha.x + (Menu.Steps - 1) * Menu.Offset.x && P.x < Menu.Alpha.x + Menu.Alpha.Width&& P.y >= Menu.Alpha.y && P.y < Menu.Alpha.y + Menu.Alpha.Height + (Menu.Steps - 1) * Menu.Offset.y)
            {
                r32 Distance = fabsf(P.y - Menu.Alpha.y);
                r32 Temp = fabsf(P.x - (Menu.Alpha.x + Menu.Alpha.Width));
                if(Temp > Distance)
                {
                    Distance = Temp;
                }
                PenTarget.Color.a = 1.0f - (r32)((u32)Distance / Menu.Alpha.Height) / (Menu.Steps - 1);
            }
            else
            {
                PenTarget.Color = OrcaState->PickColor(Point);
            }
            OrcaState->UpdateMenu(&OrcaState->Menu, PenTarget.Color, PenTarget.Mode);
        }
        
        if(FreshClick)
        {
            if(Inside(Menu.New, P))
            {
                RequestEmptyFile(OrcaState->MaxId, OrcaState->PaintingRegion, 4000, 6000);
            }
            else if(Inside(Menu.Open, P))
            {
                RequestFileLoad(&OrcaState->Canvas, OrcaState->PaintingRegion, OrcaState->MaxId);
            }
            else if(Inside(Menu.Save, P))
            {
                SaveEverything();
            }
            else if(Inside(Menu.Small, P))
            {
                ChangeBrushSize(&OrcaState->Pen, 0.8);
            }
            else if(Inside(Menu.Mirror, P))
            {
                FlipCanvas(&OrcaState->Canvas);
            }
            else if(Inside(Menu.Big, P))
            {
                ChangeBrushSize(&OrcaState->Pen, 1.25);
            }
            else if(Inside(Menu.Minus, P))
            {
                ChangeCanvasScale(&OrcaState->Canvas, 0.8);
            }
            else if(Inside(Menu.Reset, P))
            {
                ResetCanvasTransform(&OrcaState->Canvas, OrcaState->PaintingRegion);
            }
            else if(Inside(Menu.Plus, P))
            {
                ChangeCanvasScale(&OrcaState->Canvas, 1.25);
            }
            else if(Inside(Menu.RotateR, P))
            {
                RotateCanvas(&OrcaState->Canvas, 0.0625f * Pi32);
            }
            else if(Inside(Menu.OneToOne, P))
            {
                OrcaState->Canvas.Center *= 1 / OrcaState->Canvas.Scale;
                OrcaState->Canvas.Scale = 1;
            }
            else if(Inside(Menu.RotateL, P))
            {
                RotateCanvas(&OrcaState->Canvas, -0.0625f * Pi32);
            }
            else if(Inside(Menu.TiltToggle, P))
            {
                PenTarget.Mode ^= TILT_MODE_DISABLE;
                OrcaState->UpdateMenu(&OrcaState->Menu, PenTarget.Color, PenTarget.Mode);
            }
            else if(Inside(Menu.PenButtonsToggle, P))
            {
                PenTarget.Mode ^= PEN_BUTTON_MODE_FLIPPED;
                OrcaState->UpdateMenu(&OrcaState->Menu, PenTarget.Color, PenTarget.Mode);
            }
            else if(Inside(Menu.AreaFillToggle, P))
            {
                PenTarget.Mode ^= PEN_MODE_AREA_FILL;
                OrcaState->UpdateMenu(&OrcaState->Menu, PenTarget.Color, PenTarget.Mode);
            }
            else if(Inside(Menu.ResizeToggle, P))
            {
                PenTarget.Mode ^= PEN_MODE_RESIZE;
                OrcaState->UpdateMenu(&OrcaState->Menu, PenTarget.Color, PenTarget.Mode);
            }
            else if(Inside(Menu.BrushStyleSwitch, P))
            {
                PenTarget.Mode = (PenTarget.Mode & (~PEN_BRUSH_STYLE)) | (((PenTarget.Mode & PEN_BRUSH_STYLE) + 1) % 4);
                OrcaState->UpdateMenu(&OrcaState->Menu, PenTarget.Color, PenTarget.Mode);
            }
            else if(Inside(Menu.ColorButtonA, P))
            {
                PenTarget.Mode |= COLOR_MODE_PRESSURE;
                OrcaState->UpdateMenu(&OrcaState->Menu, PenTarget.Color, PenTarget.Mode);
            }
            else if(Inside(Menu.ColorButtonB, P))
            {
                PenTarget.Mode &= ~COLOR_MODE_PRESSURE;
                OrcaState->UpdateMenu(&OrcaState->Menu, PenTarget.Color, PenTarget.Mode);
            }
            else if(Inside(Menu.AlphaButton, P))
            {
                PenTarget.Mode ^= COLOR_MODE_ALPHA;
                OrcaState->UpdateMenu(&OrcaState->Menu, PenTarget.Color, PenTarget.Mode);
            }
            else if(Inside(Menu.LButton, P))
            {
                PenTarget.Mode ^= COLOR_MODE_LUMINANCE;
                OrcaState->UpdateMenu(&OrcaState->Menu, PenTarget.Color, PenTarget.Mode);
            }
            else if(Inside(Menu.abButton, P))
            {
                PenTarget.Mode ^= COLOR_MODE_CHROMA;
                OrcaState->UpdateMenu(&OrcaState->Menu, PenTarget.Color, PenTarget.Mode);
            }
            else if(Inside(Menu.chButton, P))
            {
                PenTarget.Mode ^= MENU_MODE_AB;
                OrcaState->UpdateMenu(&OrcaState->Menu, PenTarget.Color, PenTarget.Mode);
            }
        }
    }
    else
    {
        v2 DeltaP = PenTarget.P - PenState->P;
        if(IsPenClose) 
        {
            if(PickColorButton)
            {
                PenTarget.Color = OrcaState->PickColor(Point);
                OrcaState->UpdateMenu(&OrcaState->Menu, PenTarget.Color, PenTarget.Mode);
            }
            if(MoveCanvasButton)
            {
                OrcaState->Canvas.Center += Point - PenState->Point;
            }
        }
        
        if(PenTarget.Mode & TILT_MODE_DISABLE)
        {
            if(DeltaP.x || DeltaP.y)
            {
                PenTarget.Radians = -(r32)atan2(DeltaP.y, DeltaP.x);
            }
            else
            {
                PenTarget.Radians = PenState->Radians;
            }
        }
        
        if((PenTarget.Pressure > 0 || PenState->Pressure > 0) && !MoveCanvasButton && OrcaState->Canvas.Size.Height && OrcaState->Canvas.Size.Width)
        {
            if(OrcaState->Replay.Count < REPLAY_MAX - 1 && RecordReplay)
            {
                if(PenState->Pressure <= 0)
                {
                    *OrcaState->Replay.Next = *PenState;
                    OrcaState->Replay.Next++;
                    OrcaState->Replay.Count++;
                }
                *OrcaState->Replay.Next = PenTarget;
                OrcaState->Replay.Next++;
                OrcaState->Replay.Count++;
            }
            
            if((!(PenTarget.Mode & PEN_MODE_RESIZE)) && PenState->Resize.Active)
            {
                OrcaState->ResizeCanvas(&OrcaState->Canvas, PenState->Resize);
                PenState->Resize = {};
            }
            if(PenTarget.Mode & PEN_MODE_RESIZE)
            {
                if(!PenState->Resize.Active)
                {
                    PenState->Resize = {};
                    PenState->Resize.Active = true;
                }
                if(PenState->Pressure <= 0)
                {
                    if(fabsf(PenTarget.P.x / OrcaState->Canvas.Size.Width) > fabsf(PenTarget.P.y / OrcaState->Canvas.Size.Height))
                    {
                        if(PenTarget.P.x > 0)
                        {
                            PenState->Resize.Edge = 1;
                        }
                        else
                        {
                            PenState->Resize.Edge = 2;
                        }
                    }
                    else
                    {
                        if(PenTarget.P.y > 0)
                        {
                            PenState->Resize.Edge = 3;
                        }
                        else
                        {
                            PenState->Resize.Edge = 4;
                        }
                    }
                }
                else if(PenTarget.Pressure > 0)
                {
                    switch(PenState->Resize.Edge)
                    {
                        case 1:
                        {
                            PenState->Resize.Right  += DeltaP.x;
                        } break;
                        case 2:
                        {
                            PenState->Resize.Left   += DeltaP.x;
                        } break;
                        case 3:
                        {
                            PenState->Resize.Top    += DeltaP.y;
                        } break;
                        case 4:
                        {
                            PenState->Resize.Bottom += DeltaP.y;
                        } break;
                    }
                }
                else
                {
                    PenState->Resize.Edge = 0;
                }
            }
            else if(PenTarget.Mode & PEN_MODE_AREA_FILL)
            {
                local_persist v4 Color = {};
                local_persist r32 MaxDistance = 0;
                local_persist u32 Step = 0;
                if(PenState->Pressure == 0)
                {
                    PenState->Origin = PenState->P;
                    PenState->Spread = {0, 0};
                    Color = PenState->Color;
                    MaxDistance = 0;
                    Step = 0;
                }
                else
                {
                    if(PenTarget.Pressure == 0)
                    {
                        Step = 2;
                    }
                    else if(PenTarget.Mode & COLOR_MODE_PRESSURE)
                    {
                        r32 Distance = Length(PenTarget.P - PenState->Origin);
                        if(Distance >= MaxDistance)
                        {
                            MaxDistance = Distance;
                            Distance /= PenTarget.Pressure;
                            PenState->Spread = {(r32)OrcaState->Canvas.Size.Width / Distance, (r32)OrcaState->Canvas.Size.Height / Distance};
                        }
                    }
                    OrcaState->RenderAreaFlip(OrcaState->Canvas, *PenState, PenTarget, PenState->Origin, PenState->Spread, Color, Step);
                    Step = 1;
                }
            }
            else
            {
                v2 Normal = {cosf(PenState->Radians), -sinf(PenState->Radians)};
                r32 MaximumRadiansDelta = fabsf(Inner(Normal, DeltaP)) / Maximum(OrcaState->Pen.Width, PenTarget.Width);
                r32 RadiansDelta = PenTarget.Radians - PenState->Radians;
                if(RadiansDelta > Pi32)
                {
                    RadiansDelta -= 2 * Pi32;
                }
                else if(RadiansDelta < -Pi32)
                {
                    RadiansDelta += 2 * Pi32;
                }
                
                if(PenTarget.Mode & TILT_MODE_DISABLE)
                {
                    if(RadiansDelta > 0.5 * Pi32)
                    {
                        RadiansDelta -= Pi32;
                    }
                    else if(RadiansDelta < -0.5 * Pi32)
                    {
                        RadiansDelta += Pi32;
                    }
                }
                
                if( RadiansDelta < -MaximumRadiansDelta)
                    RadiansDelta = -MaximumRadiansDelta;
                if( RadiansDelta >  MaximumRadiansDelta)
                    RadiansDelta =  MaximumRadiansDelta;
                
                PenTarget.Radians = RadiansDelta + PenState->Radians;
                while(PenTarget.Radians < 0)
                    PenTarget.Radians += 2 * Pi32;
                while(PenTarget.Radians >= 2 * Pi32)
                    PenTarget.Radians -= 2 * Pi32;
                
                // TODO(Zyonji): Smooth out the corners of the width changes a bit.
                r32 MaximumWidthDelta = Length(DeltaP);
                r32 WidthDelta = PenTarget.Width - OrcaState->Pen.Width;
                if( WidthDelta < -MaximumWidthDelta)
                    WidthDelta = -MaximumWidthDelta;
                if( WidthDelta >  MaximumWidthDelta)
                    WidthDelta =  MaximumWidthDelta;
                PenTarget.Width = WidthDelta + OrcaState->Pen.Width;
                
                if((PenTarget.Mode & PEN_BRUSH_STYLE) == 2)
                {
                    r32 PressureDelta = PenTarget.Pressure - PenState->Pressure;
                    r32 PressureDeltaMax = Length(DeltaP) / PenTarget.Width;
                    if(PressureDelta > PressureDeltaMax)
                    {
                        PenTarget.Pressure = PenState->Pressure + PressureDeltaMax;
                    }
                    else if(PressureDelta < -PressureDeltaMax)
                    {
                        PenTarget.Pressure = PenState->Pressure - PressureDeltaMax;
                    }
                }
                
                OrcaState->RenderBrushStroke(OrcaState->Canvas, *PenState, PenTarget);
                
                r32 NewProjection = Inner(PenTarget.P - PenState->P, Normal);
                r32 OldProjection = Inner(PenState->Delta.P, Normal);
                if(((OldProjection <= 0 && NewProjection > 0) || (OldProjection >= 0 && NewProjection < 0)) &&(PenTarget.Mode & PEN_BRUSH_STYLE) != 2)
                {
                    pen_target OffPen = *PenState;
                    if(NewProjection < 0)
                    {
                        OffPen.P += Normal;
                    }
                    else
                    {
                        OffPen.P -= Normal;
                    }
#if 1
                    OffPen.Pressure = 0;
#else
                    OffPen.Pressure = 1;
                    OffPen.Color = {1, 0, 0, 1};
#endif
                    
                    OrcaState->RenderBrushStroke(OrcaState->Canvas, *PenState, OffPen);
                    OrcaState->RenderBrushStroke(OrcaState->Canvas, *PenState, OffPen);
                }
            }
            OrcaState->Canvas.Hot = true;
        }
    }
    
    PenState->Delta.P        = PenTarget.P        - PenState->P;
    PenState->Delta.Pressure = PenTarget.Pressure - PenState->Pressure;
    PenState->Delta.Radians  = PenTarget.Radians  - PenState->Radians;
    PenState->Delta.Width    = PenTarget.Width    - PenState->Width;
    
    PenState->P         = PenTarget.P;
    PenState->Pressure  = PenTarget.Pressure;
    PenState->Radians   = PenTarget.Radians;
    PenState->Width     = PenTarget.Width;
    PenState->Color     = PenTarget.Color;
    PenState->Mode      = PenTarget.Mode;
    
    return true;
}