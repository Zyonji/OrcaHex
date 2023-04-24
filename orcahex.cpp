#define PEN_BRUSH_STYLE          0x000F
#define COLOR_MODE_LUMINANCE     0x0010
#define COLOR_MODE_CHROMA        0x0020
#define COLOR_MODE_ALPHA         0x0040
#define COLOR_MODE_PRESSURE      0x0080
#define TILT_MODE_DISABLE        0x0100
#define PEN_BUTTON_MODE_FLIPPED  0x0200
#define MENU_MODE_AB             0x0400
#define LINE_PEN_MODE            0x1000
#define COLOR_LIMIT_LUMINANCE    0x6000
#define COLOR_LIMIT_DARK         0x2000
#define COLOR_LIMIT_LIGHT        0x4000
#define COLOR_LIMIT_OFFSET       13

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
    u32area LinePenToggle;
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
    u32area LLimitToggle;
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
    r32 NextWidth;
    u32 Buttons;
    b32 IsDown;
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
#define RENDER_LINE(name) void name(canvas_state Canvas, pen_target OldPen, pen_target NextPen)
typedef RENDER_LINE(render_line);

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
    render_line *RenderLine;
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
internal RENDER_LINE(CPURenderLine)
{
    LogError("The undefined default line rendering function was used.", "OrcaHex");
}

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
    Result.y = (r32)(1.25 * (x - y) + 0.5);
    Result.z = (r32)(0.5 * (y - z) + 0.5);
    
    return(Result);
}
internal v3
ConvertLabToRGB(v3 Lab)
{
    v3 Result;
    
    r64 y = (Lab.x + 0.160) / 1.160;
    r64 x = (Lab.y - 0.5) / 1.25 + y;
    r64 z = y - (Lab.z - 0.5) / 0.5;
    
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

internal v3
ValidatedLabForRGB(v3 Lab)
{
    v3 Result = Lab;
    
    v3 RGB = ConvertLabToRGB(Lab);
    v3 Grey = ConvertLabToRGB({Lab.x, 0.5, 0.5});
    v3 RGBVector = Grey - RGB;
    v3 ScaleA = {RGBVector.x / Grey.x, RGBVector.y / Grey.y, RGBVector.z / Grey.z};
    v3 ScaleB = {-RGBVector.x / (1 - Grey.x), -RGBVector.y / (1 - Grey.y), -RGBVector.z / (1 - Grey.z)};
    r32 Distance = Maximum(Maximum(Maximum(ScaleA.x, ScaleB.x), Maximum(ScaleA.y, ScaleB.y)), Maximum(ScaleA.z, ScaleB.z));
    
    if(Lab.x <= 0)
    {
        Result = {0, 0.5, 0.5};
    }
    else if(Lab.x >= 1)
    {
        Result = {1, 0.5, 0.5};
    }
    else if(Distance > 1.0)
    {
        Result = ConvertRGBToLab(Grey - (RGBVector / Distance));
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
        if(InverseScale >= 1.5f && InverseScale < 2)
        {
            InverseScale = 1.5;
        }
        else
        {
            InverseScale = floorf(InverseScale);
        }
    }
    if(InverseScale > 1 && Factor < 1)
    {
        if(InverseScale <= 1.5f)
        {
            InverseScale = 1.5f;
        }
        else
        {
            InverseScale = ceilf(InverseScale);
        }
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
            PenTarget.Color = OrcaState->PickColor(Point);
            OrcaState->UpdateMenu(&OrcaState->Menu, PenTarget.Color, PenTarget.Mode);
        }
        
        if(FreshClick)
        {
            if(Inside(Menu.New, P))
            {
                RequestEmptyFile(OrcaState->MaxId, OrcaState->PaintingRegion, 2400, 3600);
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
            else if(Inside(Menu.LinePenToggle, P))
            {
                PenTarget.Mode ^= LINE_PEN_MODE;
                OrcaState->UpdateMenu(&OrcaState->Menu, PenTarget.Color, PenTarget.Mode);
            }
            else if(Inside(Menu.PenButtonsToggle, P))
            {
                PenTarget.Mode ^= PEN_BUTTON_MODE_FLIPPED;
                OrcaState->UpdateMenu(&OrcaState->Menu, PenTarget.Color, PenTarget.Mode);
            }
            else if(Inside(Menu.BrushStyleSwitch, P))
            {
                PenTarget.Mode = (PenTarget.Mode & (~PEN_BRUSH_STYLE)) | (((PenTarget.Mode & PEN_BRUSH_STYLE) + 1) % 3);
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
            else if(Inside(Menu.LLimitToggle, P))
            {
                u32 LimitMode = ((PenTarget.Mode + (1 << COLOR_LIMIT_OFFSET)) & COLOR_LIMIT_LUMINANCE);
                PenTarget.Mode &= ~COLOR_LIMIT_LUMINANCE;
                if(LimitMode != COLOR_LIMIT_LUMINANCE)
                {
                    PenTarget.Mode |= LimitMode;
                }
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
            
            
            // TODO(Zyonji): Temporary test line drawing code.
            if(PenTarget.Mode & LINE_PEN_MODE)
            {
                OrcaState->RenderLine(OrcaState->Canvas, *PenState, PenTarget);
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
                
                OrcaState->RenderBrushStroke(OrcaState->Canvas, *PenState, PenTarget);
                
                
                r32 NewProjection = Inner(PenTarget.P - PenState->P, Normal);
                r32 OldProjection = Inner(PenState->Delta.P, Normal);
                if((OldProjection <= 0 && NewProjection > 0) || (OldProjection >= 0 && NewProjection < 0))
                {
                    pen_target OffPen = *PenState;
                    if(NewProjection > 0)
                    {
                        Normal = -Normal;
                    }
                    OffPen.P += Normal;
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