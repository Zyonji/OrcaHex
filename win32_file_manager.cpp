internal void
GenerateIniPath(char *Path, u32 PathSize)
{
    u32 NameLength = GetModuleFileNameA(0, Path, PathSize);
    char *At = Path + NameLength;
    while(At >= Path && *At != '/' && *At != '\\')
    {
        --At;
    }
    At++;
    char *Mask = "OrcaHex.ini";
    while(*Mask)
    {
        *At++ = *Mask++;
    }
    *At = *Mask;
}

internal DWORD
LoadInitialState(win32_orca_state *State, char *Path)
{
    DWORD WindowStyle;
    initial_state InitialState = {};
    HANDLE FileHandle = CreateFileA(Path, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if(FileHandle != INVALID_HANDLE_VALUE)
    {
        DWORD BytesRead = 0;
        ReadFile(FileHandle, &InitialState, sizeof(InitialState), &BytesRead, 0);
        CloseHandle(FileHandle);
    }
    
    if(!InitialState.MaxId)
    {
        WindowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
        State->MaxId = 1;
        State->Pen.Color = {0.5f, 0.5f, 0.5f, 1.0f};
        State->Pen.Mode = COLOR_MODE_PRESSURE | COLOR_MODE_CHROMA | COLOR_MODE_LUMINANCE;
        State->Pen.NextWidth = 16.0f;
        State->WindowData.Pos.x = 10;
        State->WindowData.Pos.y = 10;
        State->WindowData.Size.Width = 920;
        State->WindowData.Size.Height = 680;
        State->Canvas.Scale = 1;
    }
    else
    {
        WindowStyle = InitialState.WindowStyle;
        State->MaxId = InitialState.MaxId;
        State->Pen.Color = InitialState.Color;
        State->Pen.Mode = InitialState.Mode;
        State->Pen.NextWidth = InitialState.Width;
        State->WindowData.Pos.x = InitialState.WindowX;
        State->WindowData.Pos.y = InitialState.WindowY;
        State->WindowData.Size.Width = InitialState.WindowWidth;
        State->WindowData.Size.Height = InitialState.WindowHeight;
        State->Canvas.Scale = 1;
    }
    
    return(WindowStyle);
}

internal void
SaveInitialState(initial_state InitialState, char *Path)
{
    HANDLE FileHandle = CreateFileA(Path, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if(FileHandle != INVALID_HANDLE_VALUE)
    {
        DWORD BytesWritten = 0;
        WriteFile(FileHandle, &InitialState, sizeof(InitialState), &BytesWritten, 0);
        CloseHandle(FileHandle);
    }
}

internal char*
RequestFileChoice(char *FileName, u32 NameSize) // NOTE(Zyonji): NameSize = sizeof(FileName) = MAX_PATH
{
    OPENFILENAME FileSelector = {};
    
    FileSelector.lStructSize = sizeof(FileSelector);
    FileSelector.lpstrFile = FileName;
    FileSelector.lpstrFile[0] = '\0';
    FileSelector.nMaxFile = NameSize;
    FileSelector.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    GetOpenFileNameA(&FileSelector);
    return(FileName);
}
internal b32
RequestFileDestination(char *FileName, u32 NameSize) // NOTE(Zyonji): NameSize = sizeof(FileName) = 260
{
    b32 Return;
    OPENFILENAME FileSelector = {};
    
    FileSelector.lStructSize = sizeof(FileSelector);
    FileSelector.lpstrFile = FileName;
    FileSelector.lpstrFile[0] = '\0';
    FileSelector.nMaxFile = NameSize;
    FileSelector.Flags = OFN_PATHMUSTEXIST;
    
    Return = GetSaveFileNameA(&FileSelector);
    return(Return);
}
internal void
CreateFilePathMask(char *MaskBuffer, char *FilePath, char *PathEnd)
{
    char *At = MaskBuffer;
    char *From = FilePath;
    char *End = PathEnd;
    while(End >= FilePath && *End != '/' && *End != '\\')
    {
        --End;
    }
    while(From <= End)
    {
        *At++ = *From++;
    }
    char *Mask = FILE_NAME_MASK;
    while(*Mask)
    {
        *At++ = *Mask++;
    }
    *At = *Mask;
}
internal void
FillFilePathMask(char *FilePathBase, char *FilePathMask, u32 Id)
{
    SYSTEMTIME SystemTime;
    GetLocalTime(&SystemTime);
    wsprintf(FilePathBase, FilePathMask, (SystemTime.wYear - 2000), SystemTime.wMonth, SystemTime.wDay, Id);
}

internal image_data
LoadImageAndReplayFiles(char* Filename, u32 MaxId, b32 LoadReplayData)
{
    image_data Result = {};
    
    // TODO(Zyonji): Replace FreeImage code with my own.
    FREE_IMAGE_FORMAT ImageFormat = FIF_UNKNOWN;
    ImageFormat = FreeImage_GetFileType(Filename, 0);
    if(ImageFormat == FIF_UNKNOWN)
    {
        ImageFormat= FreeImage_GetFIFFromFilename(Filename);
    }
    
    FIBITMAP *TempBitmap = 0;
    if(FreeImage_FIFSupportsReading(ImageFormat))
    {
        TempBitmap = FreeImage_Load(ImageFormat, Filename);
    }
    if(!TempBitmap)
    {
        return(Result);
    }
    
    char ReplayFile[MAX_PATH];
    char *At = Filename;
    char *To = ReplayFile;
    while(*At)
    {
        *To++ = *At++;
    }
    *To = 0;
    *--To = 'x';
    *--To = '6';
    *--To = 'o';
    
    char FilePathMask[MAX_PATH];
    CreateFilePathMask(FilePathMask, Filename, At);
    At -= 8;
    if(At > Filename + 18)
    {
        u32 Order = 1;
        for(; *At >= '0' && *At <= '9'; --At)
        {
            Result.Minutes += Order * (*At - '0');
            Order *= 10;
        }
        if(*At-- == '_')
        {
            Order = 1;
            To = Result.NameBase + 44;
            for(u32 I = 0; I < 8; ++I, --To, --At)
            {
                Result.Id += Order * (*At - '0');
                Order *= 10;
                if(*At < '0' || *At > '9')
                {
                    Result.Id = MaxId;
                    break;
                }
            }
        }
        else
        {
            Result.Id = MaxId;
        }
    }
    
    FillFilePathMask(Result.NameBase, FilePathMask, Result.Id);
    
    FIBITMAP *Bitmap = FreeImage_ConvertTo32Bits(TempBitmap);
    FreeImage_Unload(TempBitmap);
    
    Result.Bitmap = Bitmap;
    Result.Memory = FreeImage_GetBits(Bitmap);
    Result.Width = FreeImage_GetWidth(Bitmap);
    Result.Height = FreeImage_GetHeight(Bitmap);
    
    if(LoadReplayData)
    {
        HANDLE FileHandle = CreateFileA(ReplayFile, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if(FileHandle != INVALID_HANDLE_VALUE)
        {
            DWORD BytesRead = 0;
            u32 MaxFileSize = (17 + REPLAY_MAX * 2) * sizeof(replay_chunk);
            LPVOID FileMemory = VirtualAlloc(0, MaxFileSize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
            
            ReadFile(FileHandle, FileMemory, MaxFileSize, &BytesRead, 0);
            CloseHandle(FileHandle);
            if(!UnpackReplay(&OrcaState->Replay, FileMemory, BytesRead))
            {
                FileHandle = INVALID_HANDLE_VALUE;
            }
            VirtualFree(FileMemory, 0, MEM_RELEASE);
        }
        if(FileHandle == INVALID_HANDLE_VALUE)
        {
            At = Filename;
            To = OrcaState->Replay.BaseFile;
            while(*At)
            {
                *To++ = *At++;
            }
            *To = 0;
        }
    }
    
    return(Result);
}

internal image_data
LoadImageFile(char* Filename, u32 MaxId)
{
    return(LoadImageAndReplayFiles(Filename, MaxId, false));
}

internal b32
SaveImage(image_data ImageData, u32 NewTime)
{
    b32 Result = 0;
    char FileName[MAX_PATH];
    wsprintf(FileName, ImageData.NameBase, NewTime);
    
    FIBITMAP *Bitmap = FreeImage_ConvertTo24Bits(ImageData.Bitmap);
    //Result = FreeImage_Save(FIF_JPEG, Bitmap, FileName, JPEG_QUALITYSUPERB);
    Result = FreeImage_Save(FIF_PNG, Bitmap, FileName, PNG_Z_BEST_SPEED);
    FreeImage_Unload(Bitmap);
    
    char *At = FileName;
    for(; *At; ++At)
    {
    }
    *--At = 'x';
    *--At = '6';
    *--At = 'o';
    
    HANDLE FileHandle = CreateFileA(FileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,0);
    if(FileHandle != INVALID_HANDLE_VALUE)
    {
        u32 FileSize;
        u32 MaxFileSize = (17 + REPLAY_MAX * 2) * sizeof(replay_chunk);
        LPVOID FileMemory = VirtualAlloc(0, MaxFileSize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
        
        CompressReplay(&OrcaState->Replay, FileMemory, &FileSize);
        
        DWORD BytesWritten = 0;
        WriteFile(FileHandle, FileMemory, FileSize, &BytesWritten, 0);
        CloseHandle(FileHandle);
        VirtualFree(FileMemory, 0, MEM_RELEASE);
    }
    
    return(Result);
}

// NOTE(Zyonji): Returns true on Yes and No.  Returns false on Cancel.
internal b32
OfferToSaveHotCanvas()
{
    b32 Result = false;
    if(OrcaState->Canvas.Hot)
    {
        gpWTEnable(OrcaState->TabletContext, false);
        s32 MessageBoxValue = MessageBox(OrcaState->Map.Window, "Do you want to save the current canvas?", "unsaved changes", MB_YESNOCANCEL | MB_DEFBUTTON1 | MB_APPLMODAL);
        if(MessageBoxValue == IDYES)
        {
            Result= SaveEverything();
        }
        if(MessageBoxValue == IDNO)
        {
            Result = true;
        }
        gpWTEnable(OrcaState->TabletContext, true);
    }
    else
    {
        Result = true;
    }
    return(Result);
}

internal void
LoadCanvas(open_gl *OpenGL, image_data *Image, canvas_state *Canvas, char* FileName, u32area PaintingRegion, u32 MaxId)
{
    image_data ImageData = LoadImageAndReplayFiles(FileName, MaxId, true);
    if(ImageData.Bitmap)
    {
        ChangeCanvas(OpenGL, ImageData, Canvas, PaintingRegion);
        FreeBitmap(&ImageData);
        *Image = ImageData;
    }
    else
    {
        LogError("Unable to load the image.", "FreeImage");
    }
}

internal void
RequestEmptyFile(u32 MaxId, u32area PaintingRegion, u32 Width, u32 Height)
{
    if(OfferToSaveHotCanvas())
    {
        image_data ImageData = {};
        ImageData.Width = Width;
        ImageData.Height = Height;
        
        ChangeCanvas(&OrcaState->OpenGL, ImageData, &OrcaState->Canvas, PaintingRegion);
        FreeBitmap(&ImageData);
        OrcaState->Image = ImageData;
        OrcaState->Image.Id = MaxId;
        
        ClearReplay(&OrcaState->Replay);
    }
}

internal void
RequestFileLoad(canvas_state *Canvas, u32area PaintingRegion, u32 MaxId)
{
    gpWTEnable(OrcaState->TabletContext, false);
    char FileName[MAX_PATH];
    RequestFileChoice(FileName, sizeof(FileName));
    if(*FileName)
    {
        LoadCanvas(&OrcaState->OpenGL, &OrcaState->Image, Canvas, FileName, PaintingRegion, MaxId);
    }
    gpWTEnable(OrcaState->TabletContext, true);
}

// TODO(Zyonji): Get memory and such on this tread and then do the processing and actual saving on another thread to not slow down painting.
internal b32
SaveEverything()
{
    if(!*(OrcaState->Image.NameBase))
    {
        char FileName[MAX_PATH];
        if(RequestFileDestination(FileName, sizeof(FileName)))
        {
            char *At = FileName;
            for(; *At; ++At)
            {
                
            }
            char FilePathMask[MAX_PATH];
            CreateFilePathMask(FilePathMask, FileName, At);
            FillFilePathMask(OrcaState->Image.NameBase, FilePathMask, OrcaState->Image.Id);
        }
    }
    if(*(OrcaState->Image.NameBase))
    {
        RenderTimer(&OrcaState->OpenGL, OrcaState->Menu, {0, 0, 1, 1});
        
        if(OrcaState->Canvas.Size.Width && OrcaState->Canvas.Size.Height)
        {
            if(OrcaState->Image.Id >= OrcaState->MaxId)
            {
                OrcaState->MaxId = OrcaState->Image.Id + 1;
            }
            OrcaState->Image.Width = OrcaState->Canvas.Size.Width;
            OrcaState->Image.Height = OrcaState->Canvas.Size.Height;
            CreateBitmap(&OrcaState->OpenGL, &OrcaState->Image);
            u32 NewTime = (u32)(((GetFileTime() - OrcaState->Canvas.StartingTime) / 10000000
                                 - OrcaState->Canvas.SecondsIdle) / 60);
            if(SaveImage(OrcaState->Image, NewTime))
            {
                OrcaState->Image.Minutes = NewTime;
                OrcaState->Canvas.Hot = false;
            }
            else
            {
                LogError("Unable to save the image.", "FreeImage");
            }
        }
        
        WINDOWINFO WindowInfo = {};
        GetWindowInfo(OrcaState->Map.Window, &WindowInfo);
        
        initial_state CurrentState = {};
        CurrentState.WindowStyle = WindowInfo.dwStyle;
        CurrentState.WindowX = OrcaState->WindowData.Pos.x;
        CurrentState.WindowY = OrcaState->WindowData.Pos.y;
        CurrentState.WindowWidth = OrcaState->WindowData.Size.Width;
        CurrentState.WindowHeight = OrcaState->WindowData.Size.Height;
        CurrentState.MaxId = OrcaState->MaxId;
        CurrentState.Color = OrcaState->Pen.Color;
        CurrentState.Mode = OrcaState->Pen.Mode;
        CurrentState.Width = OrcaState->Pen.Width;
        SaveInitialState(CurrentState, OrcaState->IniPath);
        
        return(true);
    }
    else
    {
        return(false);
    }
}