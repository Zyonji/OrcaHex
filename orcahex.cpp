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
struct pen_state
{
    r32 Pressure;
    r32 AliasDistance;
    r32 Width;
    r32 NextWidth;
    v2 P;
    v2 V;
    v2 Vector;
    v4 Color;
    u32 ColorMode;
    b32 IsDown;
    u64 Time;
    u32 Buttons;
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
    u64 MilliSeconds;
};
#define PICK_COLOR(name) v4 name(v2 P, v2u Size)
typedef PICK_COLOR(pick_color);
#define UPDATE_MENU(name) void name(menu_state *Menu, pen_state Pen)
typedef UPDATE_MENU(update_menu);
#define CREATE_NEW_FILE(name) void name(u32 MaxId, v2u DisplaySize)
typedef CREATE_NEW_FILE(create_new_file);
#define SAVE_EVERYTHING(name) void name()
typedef SAVE_EVERYTHING(save_everything);
#define LOAD_FILE(name) void name(canvas_state *Canvas, v2u DisplaySize, u32 MaxId)
typedef LOAD_FILE(load_file);
#define RENDER_BRUSH_STROKE(name) void name(canvas_state Canvas, v2 OldP, v2 NewP, v2 OldV, v2 NewV, v4 OldColor, v4 NewColor, r32 OldAliasDistance, r32 NewAliasDistance, u32 ColorMode)
typedef RENDER_BRUSH_STROKE(render_brush_stroke);
struct orca_state
{
    display_state Display;
    menu_state Menu;
    pen_state Pen;
    canvas_state Canvas;
    u32 MaxId;
    
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

internal b32 // NOTE(Zyonji): true if the the frame needs to be updated.
ProcessBrushMove(orca_state *OrcaState, pen_state OldPenState, pen_state NewPenState, b32 IsPenClose)
{
    b32 Result = false;
    
    if(NewPenState.P.y + OrcaState->Menu.Origin.y <= OrcaState->Menu.Size.Height && NewPenState.P.x + OrcaState->Menu.Origin.x <= OrcaState->Menu.Size.Width  && OldPenState.Pressure == 0)
    {
        menu_state Menu = OrcaState->Menu;
        v2 P = {NewPenState.P.x + OrcaState->Menu.Origin.x, NewPenState.P.y + OrcaState->Menu.Origin.y};
        
        if((0x2 & NewPenState.Buttons) && !(0x2 & OldPenState.Buttons))
        {
            if(P.x >= Menu.Alpha.x + (Menu.Steps - 1) * Menu.Offset.x && P.x < Menu.Alpha.x + Menu.Alpha.Width&& P.y >= Menu.Alpha.y && P.y < Menu.Alpha.y + Menu.Alpha.Height + (Menu.Steps - 1) * Menu.Offset.y)
            {
                r32 Distance = fabsf(P.y - Menu.Alpha.y);
                r32 Temp = fabsf(P.x - (Menu.Alpha.x + Menu.Alpha.Width));
                if(Temp > Distance)
                {
                    Distance = Temp;
                }
                NewPenState.Color.a = 1.0f - (r32)((u32)Distance / Menu.Alpha.Height) / (Menu.Steps - 1);
            }
            else
            {
                NewPenState.Color = OrcaState->PickColor(NewPenState.P, OrcaState->Display.Size);
            }
            OrcaState->UpdateMenu(&OrcaState->Menu, NewPenState);
        }
        OrcaState->Pen = NewPenState;
        
        if(!OldPenState.IsDown && NewPenState.IsDown)
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
                OrcaState->Pen.ColorMode |= COLOR_MODE_PRESSURE;
                OrcaState->UpdateMenu(&OrcaState->Menu, OrcaState->Pen);
            }
            else if(Inside(Menu.ColorB, P))
            {
                OrcaState->Pen.ColorMode &= ~COLOR_MODE_PRESSURE;
                OrcaState->UpdateMenu(&OrcaState->Menu, OrcaState->Pen);
            }
            else if(Inside(Menu.AlphaButton, P))
            {
                OrcaState->Pen.ColorMode ^= COLOR_MODE_ALPHA;
                OrcaState->UpdateMenu(&OrcaState->Menu, OrcaState->Pen);
            }
            else if(Inside(Menu.LButton, P))
            {
                OrcaState->Pen.ColorMode ^= COLOR_MODE_LUMINANCE;
                OrcaState->UpdateMenu(&OrcaState->Menu, OrcaState->Pen);
            }
            else if(Inside(Menu.abButton, P))
            {
                OrcaState->Pen.ColorMode ^= COLOR_MODE_CHROMA;
                OrcaState->UpdateMenu(&OrcaState->Menu, OrcaState->Pen);
            }
        }
        Result = true;
    }
    else
    {
        NewPenState.Vector = OldPenState.P - NewPenState.P;
        r32 Distance = Length(NewPenState.Vector);
        // TODO(Zyonji): Make sure the stroke doesn't cross up with itself by having the for corners of the stroke render make an hourglass shape.
        if(Distance >= 1.0f)
        {
            // TODO(Zyonji): Considder making it possible to change the width. Maybe with Packet.pkOrientation.orAltitude
            v2 Perpendicular = Perp(NewPenState.Vector);
            v2 Normal = (1.0f / Distance) * Perpendicular;
            
            r32 CanvasEdgeX = (r32)OrcaState->Canvas.Size.Width / 2.0f;
            r32 CanvasEdgeY = (r32)OrcaState->Canvas.Size.Height / 2.0f;
            v2 P = MapFrameToCanvas(NewPenState.P, OrcaState->Canvas);
            if(P.x > -CanvasEdgeX && P.x < CanvasEdgeX && P.y > -CanvasEdgeY && P.y < CanvasEdgeY && IsPenClose) 
            {
                if(0x2 & NewPenState.Buttons)
                {
                    NewPenState.Color = OrcaState->PickColor(NewPenState.P, OrcaState->Display.Size);
                    OrcaState->UpdateMenu(&OrcaState->Menu, NewPenState);
                }
                if(0x4 & NewPenState.Buttons)
                {
                    OrcaState->Canvas.Center -= NewPenState.Vector;
                }
            }
            r32 Scalar = Inner(NewPenState.V, Normal);
            NewPenState.AliasDistance = fabsf(Scalar);
            
            if((NewPenState.Pressure > 0 || OldPenState.Pressure > 0) && OrcaState->Canvas.Size.Height && OrcaState->Canvas.Size.Width && !(0x4 & NewPenState.Buttons))
            {
                v2 OldP = MapFrameToCanvas(OldPenState.P, OrcaState->Canvas);
                v2 OldV = {Inner(OrcaState->Canvas.XMap, OldPenState.V), Inner(OrcaState->Canvas.YMap, OldPenState.V)};
                
                v2 NewP = MapFrameToCanvas(NewPenState.P, OrcaState->Canvas);
                v2 NewV = {Inner(OrcaState->Canvas.XMap, NewPenState.V), Inner(OrcaState->Canvas.YMap, NewPenState.V)};
                
                v4 NewColor = NewPenState.Color;
                v4 OldColor = OldPenState.Color;
                if(NewPenState.ColorMode & COLOR_MODE_PRESSURE)
                {
                    NewColor.a *= NewPenState.Pressure;
                    OldColor.a *= OldPenState.Pressure;
                }
                else
                {
                    NewColor.a = NewPenState.Pressure;
                    OldColor.a = OldPenState.Pressure;
                }
                
                OrcaState->RenderBrushStroke(OrcaState->Canvas, OldP, NewP, OldV, NewV, OldColor, NewColor, OldPenState.AliasDistance, NewPenState.AliasDistance, NewPenState.ColorMode);
                // TODO(Zyonji): This seems to not anti alias the edges correctly.  When the canvas is mirrored, the edges seem to become aliased.
                if(Inner(NewPenState.Vector, Perp(OldPenState.V)) * Inner(OldPenState.Vector, Perp(OldPenState.V)) < 0)
                {
                    v2 Offset = Normalize(Perp(OldV));
                    if(Inner(Offset, OldP - NewP) < 0)
                    {
                        Offset = -Offset;
                    }
                    v2 OffP = OldP + Offset;
                    v4 OffColor = OldColor;
                    OldColor.a *= 2;
                    if(OldColor.a > 1)
                    {
                        OldColor.a = 1;
                    }
                    OffColor.a = 0;
                    
                    OrcaState->RenderBrushStroke(OrcaState->Canvas, OldP, OffP, OldV, OldV, OldColor, OffColor, OldPenState.AliasDistance, OldPenState.AliasDistance, OldPenState.ColorMode);
                }
                OrcaState->Canvas.Hot = true;
            }
            OrcaState->Pen = NewPenState;
            Result = true;
        }
    }
    return(Result);
}