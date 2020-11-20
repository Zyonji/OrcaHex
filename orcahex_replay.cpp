internal void
InitReplay(replay_state *Replay)
{
    Replay->Version = 2;
    Replay->Magic   = 0x6F3678;
    Replay->Next = (pen_target *)Replay->Buffer;
    Replay->Next += Replay->Count;
}

internal void
ClearReplay(replay_state *Replay)
{
    Replay->Version = 2;
    Replay->Magic   = 0x6F3678;
    Replay->Count = 0;
    Replay->Next = (pen_target *)Replay->Buffer;
}

internal void
CompressReplay(replay_state *Source, void *Target, u32 *TargetSize)
{
    pen_target *Replay = (pen_target *)Source->Buffer;
    replay_chunk *Chunk = (replay_chunk *)Target;
    Chunk->Version = Source->Version;
    Chunk->Magic   = Source->Magic;
    Chunk->Count   = Source->Count;
    char *At = Source->BaseFile;
    char *To = Chunk->BaseFile;
    for(u32 Character = 0; Character < 260; Character++)
    {
        *To++ = *At++;
    }
    Chunk += 17;
    
    replay_chunk *Segment;
    while(Source->Next >= Replay)
    {
        Segment = Chunk++;
        Segment->SegmentLength = 0;
        Segment->Width = Replay->Width;
        Segment->Mode  = Replay->Mode;
        Segment->Color[0] = (u8)(Replay->Color.r * 255);
        Segment->Color[1] = (u8)(Replay->Color.g * 255);
        Segment->Color[2] = (u8)(Replay->Color.b * 255);
        Segment->Color[3] = (u8)(Replay->Color.a * 255);
        pen_target *Reference = Replay;
        while(Reference->Width == Replay->Width && Reference->Mode == Replay->Mode && Reference->Color.r == Replay->Color.r && Reference->Color.g == Replay->Color.g && Reference->Color.b == Replay->Color.b && Reference->Color.a == Replay->Color.a && Source->Next >= Replay)
        {
            Chunk->Pressure = Replay->Pressure;
            Chunk->Radians  = Replay->Radians;
            Chunk->P        = Replay->P;
            Chunk++;
            Replay++;
            Segment->SegmentLength++;
        }
    }
    
    *TargetSize = (u32)((umm)Chunk - (umm)Target);
}

internal void
UnpackReplay(replay_state *Target, void *Source, u32 FileSize)
{
    pen_target *Replay = (pen_target *)Target->Buffer;
    replay_chunk *Chunk = (replay_chunk *)Source;
    replay_chunk *SourceEnd = Chunk;
    SourceEnd += FileSize / sizeof(replay_chunk);
    Target->Version = 2;
    Target->Magic   = 0x6F3678;
    Target->Count = 0;
    char *At = Chunk->BaseFile;
    char *To = Target->BaseFile;
    for(u32 Character = 0; Character < 260; Character++)
    {
        *To++ = *At++;
    }
    Chunk += 17;
    
    u32 SegmentCount;
    r32 SegmentWidth;
    u32 SegmentMode;
    v4 SegmentColor;
    while(Chunk < SourceEnd)
    {
        SegmentCount = Chunk->SegmentLength;
        SegmentWidth = Chunk->Width;
        SegmentMode  = Chunk->Mode;
        SegmentColor.r = (r32)Chunk->Color[0] / 255.0f;
        SegmentColor.g = (r32)Chunk->Color[1] / 255.0f;
        SegmentColor.b = (r32)Chunk->Color[2] / 255.0f;
        SegmentColor.a = (r32)Chunk->Color[3] / 255.0f;
        Chunk++;
        while(SegmentCount--)
        {
            Replay->Pressure = Chunk->Pressure;
            Replay->Radians = Chunk->Radians;
            Replay->P = Chunk->P;
            Replay->Width = SegmentWidth;
            Replay->Mode = SegmentMode;
            Replay->Color = SegmentColor;
            Chunk++;
            Replay++;
            Target->Count++;
        }
    }
    Target->Next = Replay;
}

internal void
AnimateReplay(HWND Window, b32 *Alive, b32 SleepIsGranular)
{
    GlobalReplayModeChange = false;
    
    replay_state *Replay = &OrcaState->Replay;
    Replay->Next = (pen_target *)Replay->Buffer;
    u32 ReplayCountMax = Replay->Count;
    Replay->Count = 0;
    u32 Count = 0;
    
    if(*Replay->BaseFile)
    {
        OrcaState->Image = LoadImage(Replay->BaseFile, OrcaState->MaxId);
    }
    else
    {
        OrcaState->Image.Memory = 0;
    }
    ChangeCanvas(&OrcaState->OpenGL, OrcaState->Image, &OrcaState->Canvas, OrcaState->PaintingRegion);
    FreeBitmap(&OrcaState->Image);
    
    r32 SecondsElapsed;
    r32 TargetSecondsPerFrame = 1.0f / 60.0f;
    // NOTE(Zyonji): 15 minute long recordings.
    u32 ReplaysPerFrame = ReplayCountMax / (60 * 60 * 15) + 1;
    LARGE_INTEGER FrameStart;
    LARGE_INTEGER CurrentCounter;
    QueryPerformanceCounter(&FrameStart);
    
    while(Replay->Count < ReplayCountMax && *Alive)
    {
        ProcessBrushMove(OrcaState, *Replay->Next, {0,0}, 0, 0, 0, false);
        
        Replay->Next++;
        Replay->Count++;
        Count++;
        
        if(Count >= ReplaysPerFrame)
        {
            OrcaState->UpdateMenu(&OrcaState->Menu, OrcaState->Pen.Color, OrcaState->Pen.Mode);
            
            MSG Message;
            while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
            {
                if(Message.message == WM_QUIT)
                {
                    *Alive = false;
                }
                TranslateMessage(&Message);
                DispatchMessageA(&Message);
            }
            
            QueryPerformanceCounter(&CurrentCounter);
            SecondsElapsed = Win32GetSecondsElapsed(FrameStart, CurrentCounter);
            
            if(SecondsElapsed < TargetSecondsPerFrame)
            {                        
                if(SleepIsGranular)
                {
                    DWORD SleepMS = (DWORD)(1000.0f * (TargetSecondsPerFrame -
                                                       SecondsElapsed));
                    if(SleepMS > 0)
                    {
                        Sleep(SleepMS);
                    }
                }
                
                while(SecondsElapsed < TargetSecondsPerFrame)
                {                            
                    QueryPerformanceCounter(&CurrentCounter);
                    SecondsElapsed = Win32GetSecondsElapsed(FrameStart, CurrentCounter);
                }
            }
            
            QueryPerformanceCounter(&FrameStart);
            
            HDC WindowDC = GetDC(Window);
            wglMakeCurrent(WindowDC, OrcaState->OpenGL.RenderingContext);
            DisplayBuffer(&OrcaState->OpenGL, OrcaState->PaintingRegion, OrcaState->Pen, OrcaState->Canvas, OrcaState->Menu, 0);
            ReleaseDC(Window, WindowDC);
            if(OrcaState->StreamWindow)
            {
                u32 PenHistoryOffset = Minimum(200 * ReplaysPerFrame, Replay->Count);
                u32 PenHistoryWindow = PenHistoryOffset + 
                    Minimum(200 * ReplaysPerFrame, ReplayCountMax - Replay->Count);
                pen_target *PenHistory = Replay->Next - PenHistoryOffset;
                
                HDC StreamDC = GetDC(OrcaState->StreamWindow);
                wglMakeCurrent(StreamDC, OrcaState->OpenGL.RenderingContext);
                DisplayStreamFrame(&OrcaState->OpenGL, OrcaState->Pen, PenHistory,
                                   PenHistoryOffset, PenHistoryWindow, OrcaState->Canvas);
                ReleaseDC(OrcaState->StreamWindow, StreamDC);
            }
            
            Count = 0;
        }
        
        if(GlobalReplayModeChange && !Replay->Next->Pressure)
        {
            GlobalReplayModeChange = false;
            break;
        }
    }
    HDC WindowDC = GetDC(Window);
    wglMakeCurrent(WindowDC, OrcaState->OpenGL.RenderingContext);
    RenderTimer(&OrcaState->OpenGL, OrcaState->Menu, {0, 0, 1, 1});
    ReleaseDC(Window, WindowDC);
}