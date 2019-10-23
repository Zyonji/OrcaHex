#define COLOR_MODE_LUMINANCE 0x1
#define COLOR_MODE_CHROMA    0x2
#define COLOR_MODE_ALPHA     0x4
#define COLOR_MODE_PRESSURE  0x8

struct display_state
{
    v2u Size;
};
struct menu_state
{
    v2u Size;
    v2 Origin;
    
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
    u32area ColorA;
    u32area ColorB;
    u32area Alpha;
    u32area L;
    u32area a;
    u32area b;
    u32area AlphaButton;
    u32area LButton;
    u32area abButton;
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
    u32 ColorMode;
};
struct pen_state : pen_target
{
    pen_base Delta;
    v2 Point;
    r32 NextWidth;
    u32 Buttons;
    b32 IsDown;
};
struct canvas_state
{
    v2u Size;
    r32 Scale;
    v2 XMap;
    v2 YMap;
    v2 Center;
    b32 Mirrored;
    b32 Hot;
    u64 StartingTime;
    s32 SecondsIdle;
};
#define PICK_COLOR(name) v4 name(v2 P, v2u Size)
typedef PICK_COLOR(pick_color);
#define UPDATE_MENU(name) void name(menu_state *Menu, v4 Color, u32 ColorMode)
typedef UPDATE_MENU(update_menu);
#define CREATE_NEW_FILE(name) void name(u32 MaxId, v2u DisplaySize)
typedef CREATE_NEW_FILE(create_new_file);
#define SAVE_EVERYTHING(name) void name()
typedef SAVE_EVERYTHING(save_everything);
#define LOAD_FILE(name) void name(canvas_state *Canvas, v2u DisplaySize, u32 MaxId)
typedef LOAD_FILE(load_file);
#define RENDER_BRUSH_STROKE(name) void name(canvas_state Canvas, pen_target OldPen, pen_target NewPen)
//#define RENDER_BRUSH_STROKE(name) void name(canvas_state Canvas, v2 OldP, v2 NewP, v2 OldV, v2 NewV, v4 OldColor, v4 NewColor, r32 OldAliasDistance, r32 NewAliasDistance, u32 ColorMode)
typedef RENDER_BRUSH_STROKE(render_brush_stroke);
struct orca_state
{
    display_state Display;
    menu_state Menu;
    pen_state Pen;
    canvas_state Canvas;
    u32 MaxId;
    
    pen_target *ReplayBuffer;
    pen_target *NextReplay;
    u32 ReplayCount;
    
    pick_color *PickColor;
    update_menu *UpdateMenu;
    create_new_file *CreateNewFile;
    save_everything *SaveEverything;
    load_file *LoadFile;
    render_brush_stroke *RenderBrushStroke;
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
internal CREATE_NEW_FILE(CPUCreateNewFile)
{
    LogError("The undefined default file creating function was used.", "OrcaHex");
}
internal SAVE_EVERYTHING(CPUSaveEverything)
{
    LogError("The undefined default saving function was used.", "OrcaHex");
}
internal LOAD_FILE(CPULoadFile)
{
    LogError("The undefined default file loading function was used.", "OrcaHex");
}
internal RENDER_BRUSH_STROKE(CPURenderBrushStroke)
{
    LogError("The undefined default brush stroke rendering function was used.", "OrcaHex");
}

// NOTE(Zyonji): I currently use 2.2 and 2.4 exponents as approximation.  This is mainly visible in the darker ranges.
internal r32
GetAFromRGB(v3 Lab, v3 RGB, r32 Value)
{
    r32 Result;
    r64 CubicPart = (Value - RGB.y * pow(Lab.x, 3) - RGB.z * pow(Lab.x - Lab.z, 3)) / RGB.x;
    r32 Sign = 1;
    if(CubicPart < 0)
    {
        CubicPart = -CubicPart;
        Sign = -1;
    }
    Result = Sign * (r32)pow(CubicPart, 1.0/3.0) - Lab.x;
    return(Result);
}
internal v2
GetALimits(v3 Lab)
{
    v2 Result;
    r32 R0 = GetAFromRGB(Lab, {    78.0 /    29.0,   -37.0 /    29.0,   -12.0 /    29.0}, 0);
    r32 R1 = GetAFromRGB(Lab, {    78.0 /    29.0,   -37.0 /    29.0,   -12.0 /    29.0}, 1);
    r32 G0 = GetAFromRGB(Lab, { -2589.0 /  2533.0,  5011.0 /  2533.0,   111.0 /  2533.0}, 0);
    r32 G1 = GetAFromRGB(Lab, { -2589.0 /  2533.0,  5011.0 /  2533.0,   111.0 /  2533.0}, 1);
    r32 B0 = GetAFromRGB(Lab, {     3.0 /    49.0,   -11.0 /    49.0,    57.0 /    49.0}, 0);
    r32 B1 = GetAFromRGB(Lab, {     3.0 /    49.0,   -11.0 /    49.0,    57.0 /    49.0}, 1);
    Result.x = Maximum(Maximum(Minimum(R0, R1), Minimum(G0, G1)), Minimum(B0, B1));
    Result.y = Minimum(Minimum(Maximum(R0, R1), Maximum(G0, G1)), Maximum(B0, B1));
    return(Result);
}
internal r32
GetBFromRGB(v3 Lab, v3 RGB, r32 Value)
{
    r32 Result;
    r64 CubicPart = (Value - RGB.y * pow(Lab.x, 3) - RGB.x * pow(Lab.x + Lab.y, 3)) / RGB.z;
    r32 Sign = 1;
    if(CubicPart < 0)
    {
        CubicPart = -CubicPart;
        Sign = -1;
    }
    Result =  Lab.x - Sign * (r32)pow(CubicPart, 1.0/3.0);
    return(Result);
}
internal v2
GetBLimits(v3 Lab)
{
    v2 Result;
    r32 R0 = GetBFromRGB(Lab, {    78.0 /    29.0,   -37.0 /    29.0,   -12.0 /    29.0}, 0);
    r32 R1 = GetBFromRGB(Lab, {    78.0 /    29.0,   -37.0 /    29.0,   -12.0 /    29.0}, 1);
    r32 G0 = GetBFromRGB(Lab, { -2589.0 /  2533.0,  5011.0 /  2533.0,   111.0 /  2533.0}, 0);
    r32 G1 = GetBFromRGB(Lab, { -2589.0 /  2533.0,  5011.0 /  2533.0,   111.0 /  2533.0}, 1);
    r32 B0 = GetBFromRGB(Lab, {     3.0 /    49.0,   -11.0 /    49.0,    57.0 /    49.0}, 0);
    r32 B1 = GetBFromRGB(Lab, {     3.0 /    49.0,   -11.0 /    49.0,    57.0 /    49.0}, 1);
    Result.x = Maximum(Maximum(Minimum(R0, R1), Minimum(G0, G1)), Minimum(B0, B1));
    Result.y = Minimum(Minimum(Maximum(R0, R1), Maximum(G0, G1)), Maximum(B0, B1));
    return(Result);
}

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

// TODO(Zyonji): Ensure that Grey includes no 0 or 1.
internal v3
ValidateRGB(v3 RGB, v3 Grey)
{
    v3 Result = RGB;
    v3 RGBVector = Grey - RGB;
    v3 ScaleA = {RGBVector.x / Grey.x, RGBVector.y / Grey.y, RGBVector.z / Grey.z};
    v3 ScaleB = {-RGBVector.x / (1 - Grey.x), -RGBVector.y / (1 - Grey.y), -RGBVector.z / (1 - Grey.z)};
    r32 Distance = Maximum(Maximum(Maximum(ScaleA.x, ScaleB.x), Maximum(ScaleA.y, ScaleB.y)), Maximum(ScaleA.z, ScaleB.z));
    if(Distance > 1.0)
    {
        Result = Grey - (RGBVector / Distance);
    }
    return(Result);
}

internal void
ResetCanvasTransform(canvas_state *Canvas, v2u WindowSize)
{
    Canvas->Scale = 1 / (r32)(Maximum(Canvas->Size.Width / WindowSize.Width, Canvas->Size.Height / WindowSize.Height) + 1);
    Canvas->XMap = {1, 0};
    Canvas->YMap = {0, 1};
    Canvas->Center = {0, 0};
    Canvas->Mirrored = false;
}

internal v2
MapFrameToCanvas(v2 P, canvas_state Canvas)
{
    v2 Result = {};
    v2 Temp = P - Canvas.Center;
    Result.x = Inner(Canvas.XMap, Temp) / Canvas.Scale;
    Result.y = Inner(Canvas.YMap, Temp) / Canvas.Scale;
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
    if(!Canvas->Mirrored)
    {
        Radians = -Radians;
    }
    v2 TempX = { cosf(Radians), sinf(Radians)};
    v2 TempY = {-sinf(Radians), cosf(Radians)};
    v2 XX = {Canvas->XMap.x, Canvas->YMap.x};
    v2 YY = {Canvas->XMap.y, Canvas->YMap.y};
    Canvas->XMap = {Inner(XX, TempX), Inner(YY, TempX)};
    Canvas->YMap = {Inner(XX, TempY), Inner(YY, TempY)};
    if(!Canvas->Mirrored)
    {
        TempX.y = -TempX.y;
        TempY.x = -TempY.x;
    }
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
ProcessBrushMove(orca_state *OrcaState, pen_target PenTarget, v2 Point,  u32 Buttons, b32 IsPenClose, b32 FreshClick, b32 RecordReplay)
{
    pen_state *PenState = &OrcaState->Pen;
    
    if(Point.y + OrcaState->Menu.Origin.y <= OrcaState->Menu.Size.Height && Point.x + OrcaState->Menu.Origin.x <= OrcaState->Menu.Size.Width  && PenState->Pressure == 0)
    {
        menu_state Menu = OrcaState->Menu;
        v2 P = {Point.x + OrcaState->Menu.Origin.x, Point.y + OrcaState->Menu.Origin.y};
        
        if((0x2 & Buttons) && !(0x2 & PenState->Buttons))
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
                PenTarget.Color = OrcaState->PickColor(Point, OrcaState->Display.Size);
            }
            OrcaState->UpdateMenu(&OrcaState->Menu, PenTarget.Color, PenTarget.ColorMode);
        }
        
        if(FreshClick)
        {
            if(Inside(Menu.New, P))
            {
                OrcaState->CreateNewFile(OrcaState->MaxId, OrcaState->Display.Size);
            }
            else if(Inside(Menu.Open, P))
            {
                OrcaState->LoadFile(&OrcaState->Canvas, OrcaState->Display.Size, OrcaState->MaxId);
            }
            else if(Inside(Menu.Save, P))
            {
                OrcaState->SaveEverything();
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
                ResetCanvasTransform(&OrcaState->Canvas, OrcaState->Display.Size);
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
            else if(Inside(Menu.ColorA, P))
            {
                PenTarget.ColorMode |= COLOR_MODE_PRESSURE;
                OrcaState->UpdateMenu(&OrcaState->Menu, PenTarget.Color, PenTarget.ColorMode);
            }
            else if(Inside(Menu.ColorB, P))
            {
                PenTarget.ColorMode &= ~COLOR_MODE_PRESSURE;
                OrcaState->UpdateMenu(&OrcaState->Menu, PenTarget.Color, PenTarget.ColorMode);
            }
            else if(Inside(Menu.AlphaButton, P))
            {
                PenTarget.ColorMode ^= COLOR_MODE_ALPHA;
                OrcaState->UpdateMenu(&OrcaState->Menu, PenTarget.Color, PenTarget.ColorMode);
            }
            else if(Inside(Menu.LButton, P))
            {
                PenTarget.ColorMode ^= COLOR_MODE_LUMINANCE;
                OrcaState->UpdateMenu(&OrcaState->Menu, PenTarget.Color, PenTarget.ColorMode);
            }
            else if(Inside(Menu.abButton, P))
            {
                PenTarget.ColorMode ^= COLOR_MODE_CHROMA;
                OrcaState->UpdateMenu(&OrcaState->Menu, PenTarget.Color, PenTarget.ColorMode);
            }
        }
    }
    else
    {
        v2 DeltaP = PenTarget.P - PenState->P;
        r32 CanvasEdgeX = (r32)OrcaState->Canvas.Size.Width / 2.0f;
        r32 CanvasEdgeY = (r32)OrcaState->Canvas.Size.Height / 2.0f;
        if(PenTarget.P.x > -CanvasEdgeX && PenTarget.P.x < CanvasEdgeX && PenTarget.P.y > -CanvasEdgeY && PenTarget.P.y < CanvasEdgeY && IsPenClose) 
        {
            if(0x2 & Buttons)
            {
                PenTarget.Color = OrcaState->PickColor(Point, OrcaState->Display.Size);
                OrcaState->UpdateMenu(&OrcaState->Menu, PenTarget.Color, PenTarget.ColorMode);
            }
            if(0x4 & Buttons)
            {
                OrcaState->Canvas.Center += Point - PenState->Point;
            }
        }
        
        // TODO(Zyonji): Make sure the stroke doesn't cross up with itself by having the for corners of the stroke render make an hourglass shape.
        if((PenTarget.Pressure > 0 || PenState->Pressure > 0) && !(0x4 & Buttons) && OrcaState->Canvas.Size.Height && OrcaState->Canvas.Size.Width)
        {
            if(OrcaState->ReplayCount < 10000000 && RecordReplay)
            {
                if(PenState->Pressure <= 0)
                {
                    *OrcaState->NextReplay = *PenState;
                    OrcaState->NextReplay++;
                    OrcaState->ReplayCount++;
                }
                *OrcaState->NextReplay = PenTarget;
                OrcaState->NextReplay++;
                OrcaState->ReplayCount++;
            }
            
            //PenTarget.Radians = PenState->Radians;
            
            // TODO(Zyonji): Smooth things out and allow active turning.
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
            
            //v2 Normal = {cosf(PenState->Radians), -sinf(PenState->Radians)};
            if(Inner(DeltaP, Normal) * Inner(PenState->Delta.P, Normal) < 0)
            {
                pen_target OffPen = *PenState;
                if(Inner(Normal, PenState->P - PenTarget.P) < 0)
                {
                    Normal = -Normal;
                }
                OffPen.P += Normal;
                OffPen.Color.a = 0;
                
                OrcaState->RenderBrushStroke(OrcaState->Canvas, *PenState, OffPen);
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
    PenState->ColorMode = PenTarget.ColorMode;
    
    return true;
}