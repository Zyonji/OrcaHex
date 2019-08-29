#include "types.h"
#include "orcahex.cpp"

#include <windows.h>
#include <GL/gl.h>
#include "wintab.h"
// TODO(Zyonji): Check for use of PK_ROTATION
#define PACKETDATA (PK_TIME | PK_X | PK_Y | PK_Z | PK_BUTTONS | PK_NORMAL_PRESSURE | PK_ORIENTATION | PK_CURSOR)
#define PACKETMODE 0
#include "pktdef.h"
#include "FreeImage.h"

#define FILE_NAME_MASK "%02d-%02d-%02d_%08d_%%dmin.png"

#define WGL_DRAW_TO_WINDOW_ARB                  0x2001
#define WGL_ACCELERATION_ARB                    0x2003
#define WGL_SUPPORT_OPENGL_ARB                  0x2010
#define WGL_DOUBLE_BUFFER_ARB                   0x2011
#define WGL_PIXEL_TYPE_ARB                      0x2013

#define WGL_FULL_ACCELERATION_ARB               0x2027
#define WGL_TYPE_RGBA_ARB                       0x202B

#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
#define WGL_CONTEXT_FLAGS_ARB                   0x2094

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB        0x00000001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB  0x0002
#define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126

#define GL_CONSTANT_COLOR                       0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR             0x8002
#define GL_BGR                                  0x80E0
#define GL_BGRA                                 0x80E1

#define GL_CLAMP_TO_EDGE                        0x812F
#define GL_TEXTURE0                             0x84C0

#define GL_ARRAY_BUFFER                         0x8892
#define GL_STATIC_DRAW                          0x88E4
#define GL_DYNAMIC_DRAW                         0x88E8

#define GL_FRAGMENT_SHADER                      0x8B30
#define GL_VERTEX_SHADER                        0x8B31
#define GL_LINK_STATUS                          0x8B82
#define GL_DRAW_FRAMEBUFFER                     0x8CA9
#define GL_COLOR_ATTACHMENT0                    0x8CE0
#define GL_FRAMEBUFFER                          0x8D40

typedef char GLchar;
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;

typedef BOOL WINAPI type_wglChoosePixelFormatARB(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
typedef HGLRC WINAPI type_wglCreateContextAttribsARB(HDC hDC, HGLRC hShareContext,
                                                     const int *attribList);

typedef void WINAPI type_glBindFramebuffer(GLenum target, GLuint framebuffer);
typedef void WINAPI type_glUseProgram(GLuint program);
typedef void WINAPI type_glEnableVertexAttribArray(GLuint index);
typedef void WINAPI type_glDisableVertexAttribArray(GLuint index);
typedef void WINAPI type_glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef GLuint WINAPI type_glCreateShader(GLenum type);
typedef void WINAPI type_glShaderSource(GLuint shader, GLsizei count, GLchar **string, GLint *length);
typedef void WINAPI type_glCompileShader(GLuint shader);
typedef GLuint WINAPI type_glCreateProgram(void);
typedef void WINAPI type_glAttachShader(GLuint program, GLuint shader);typedef void WINAPI type_glLinkProgram(GLuint program);
typedef void WINAPI type_glDeleteShader(GLuint shader);
typedef GLint WINAPI type_glGetAttribLocation(GLuint program, const GLchar *name);
typedef GLint WINAPI type_glGetUniformLocation(GLuint program, const GLchar *name);
typedef void WINAPI type_glUniform1f(GLint location, GLfloat v0);
typedef void WINAPI type_glUniform2fv(GLint location, GLsizei count, const GLfloat *value);
typedef void WINAPI type_glUniform4fv(GLint location, GLsizei count, const GLfloat *value);
typedef void WINAPI type_glUniform2iv(GLint location, GLsizei count, const GLint *value);
typedef void WINAPI type_glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void WINAPI type_glGenFramebuffers(GLsizei n, GLuint *framebuffers);
typedef void WINAPI type_glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void WINAPI type_glDeleteFramebuffers(GLsizei n, GLuint *framebuffers);
typedef void WINAPI type_glDrawBuffers(GLsizei n, const GLenum *bufs);
typedef void WINAPI type_glActiveTexture(GLenum texture);
typedef void WINAPI type_glGenBuffers(GLsizei n, GLuint *buffers);
typedef void WINAPI type_glBindBuffer(GLenum target, GLuint buffer);
typedef void WINAPI type_glBufferData(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void WINAPI type_glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void *data);
typedef void WINAPI type_glValidateProgram(GLuint program);
typedef void WINAPI type_glGetProgramiv(GLuint program, GLenum pname, GLint *params);
typedef void WINAPI type_glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void WINAPI type_glGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void WINAPI type_glGenVertexArrays(GLsizei n, GLuint *arrays);
typedef void WINAPI type_glBindVertexArray(GLuint array);
typedef void WINAPI type_glBlendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void WINAPI type_glGenerateMipmap(GLenum target);

#define OpenGLGlobalFunction(Name) global type_##Name *Name;
OpenGLGlobalFunction(wglChoosePixelFormatARB)
OpenGLGlobalFunction(wglCreateContextAttribsARB)

OpenGLGlobalFunction(glEnableVertexAttribArray)
OpenGLGlobalFunction(glDisableVertexAttribArray)
OpenGLGlobalFunction(glVertexAttribPointer)
OpenGLGlobalFunction(glCreateShader)
OpenGLGlobalFunction(glShaderSource)
OpenGLGlobalFunction(glCompileShader)
OpenGLGlobalFunction(glAttachShader)
OpenGLGlobalFunction(glDeleteShader)
OpenGLGlobalFunction(glCreateProgram)
OpenGLGlobalFunction(glLinkProgram)
OpenGLGlobalFunction(glValidateProgram)
OpenGLGlobalFunction(glUseProgram)
OpenGLGlobalFunction(glGetProgramiv)
OpenGLGlobalFunction(glGetShaderInfoLog)
OpenGLGlobalFunction(glGetProgramInfoLog)
OpenGLGlobalFunction(glGetAttribLocation)
OpenGLGlobalFunction(glGetUniformLocation)
OpenGLGlobalFunction(glUniformMatrix4fv)
OpenGLGlobalFunction(glUniform1f)
OpenGLGlobalFunction(glUniform2fv)
OpenGLGlobalFunction(glUniform4fv)
OpenGLGlobalFunction(glUniform2iv)

OpenGLGlobalFunction(glBindFramebuffer)
OpenGLGlobalFunction(glGenFramebuffers)
OpenGLGlobalFunction(glFramebufferTexture2D)
OpenGLGlobalFunction(glDeleteFramebuffers)
OpenGLGlobalFunction(glActiveTexture)
OpenGLGlobalFunction(glGenerateMipmap)

OpenGLGlobalFunction(glDrawBuffers)
OpenGLGlobalFunction(glGenBuffers)
OpenGLGlobalFunction(glBindBuffer)
OpenGLGlobalFunction(glBufferData)
OpenGLGlobalFunction(glBufferSubData)
OpenGLGlobalFunction(glGenVertexArrays)
OpenGLGlobalFunction(glBindVertexArray)

OpenGLGlobalFunction(glBlendColor)

internal void
LoadOpenGLFunctions()
{
#define Win32GetOpenGLFunction(Name) Name = (type_##Name *)wglGetProcAddress(#Name)
    Win32GetOpenGLFunction(glBindFramebuffer);
    Win32GetOpenGLFunction(glUseProgram);
    Win32GetOpenGLFunction(glEnableVertexAttribArray);
    Win32GetOpenGLFunction(glDisableVertexAttribArray);
    Win32GetOpenGLFunction(glVertexAttribPointer);
    Win32GetOpenGLFunction(glCreateShader);
    Win32GetOpenGLFunction(glShaderSource);
    Win32GetOpenGLFunction(glCompileShader);
    Win32GetOpenGLFunction(glCreateProgram);
    Win32GetOpenGLFunction(glAttachShader);
    Win32GetOpenGLFunction(glLinkProgram);
    Win32GetOpenGLFunction(glDeleteShader);
    Win32GetOpenGLFunction(glGetAttribLocation);
    Win32GetOpenGLFunction(glGetUniformLocation);
    Win32GetOpenGLFunction(glGenFramebuffers);
    Win32GetOpenGLFunction(glFramebufferTexture2D);
    Win32GetOpenGLFunction(glDeleteFramebuffers);
    Win32GetOpenGLFunction(glDrawBuffers);
    Win32GetOpenGLFunction(glActiveTexture);
    Win32GetOpenGLFunction(glGenBuffers);
    Win32GetOpenGLFunction(glBindBuffer);
    Win32GetOpenGLFunction(glBufferData);
    Win32GetOpenGLFunction(glBufferSubData);
    Win32GetOpenGLFunction(glValidateProgram);
    Win32GetOpenGLFunction(glGetProgramiv);
    Win32GetOpenGLFunction(glGetShaderInfoLog);
    Win32GetOpenGLFunction(glGetProgramInfoLog);
    Win32GetOpenGLFunction(glGenVertexArrays);
    Win32GetOpenGLFunction(glBindVertexArray);
    Win32GetOpenGLFunction(glUniformMatrix4fv);
    Win32GetOpenGLFunction(glUniform1f);
    Win32GetOpenGLFunction(glUniform2fv);
    Win32GetOpenGLFunction(glUniform4fv);
    Win32GetOpenGLFunction(glUniform2iv);
    Win32GetOpenGLFunction(glBlendColor);
    Win32GetOpenGLFunction(glGenerateMipmap);
}

#include "opengl_orcahex.cpp"

typedef UINT (WINAPI * type_WTInfoA) (UINT, UINT, LPVOID);
typedef HCTX (WINAPI * type_WTOpenA) (HWND, LPLOGCONTEXTA, BOOL);
typedef BOOL (WINAPI * type_WTClose) (HCTX);
typedef int  (WINAPI * type_WTPacketsGet) (HCTX, int, LPVOID);
typedef BOOL (WINAPI * type_WTPacket) (HCTX, UINT, LPVOID);
typedef BOOL (WINAPI * type_WTEnable) (HCTX, BOOL);
typedef BOOL (WINAPI * type_WTOverlap) (HCTX, BOOL);
typedef BOOL (WINAPI * type_WTConfig) (HCTX, HWND);
typedef BOOL (WINAPI * type_WTGetA) (HCTX, LPLOGCONTEXT);
typedef BOOL (WINAPI * type_WTSetA) (HCTX, LPLOGCONTEXT);
typedef BOOL (WINAPI * type_WTExtGet) (HCTX, UINT, LPVOID);
typedef BOOL (WINAPI * type_WTExtSet) (HCTX, UINT, LPVOID);
typedef BOOL (WINAPI * type_WTSave) (HCTX, LPVOID);
typedef HCTX (WINAPI * type_WTRestore) (HWND, LPVOID, BOOL);
typedef int  (WINAPI * type_WTDataPeek) (HCTX, UINT, UINT, int, LPVOID, LPINT);
typedef BOOL (WINAPI * type_WTQueueSizeSet) (HCTX, int);

#define WinTabGlobalFunction(Name) global type_##Name gp##Name;
WinTabGlobalFunction(WTInfoA)
WinTabGlobalFunction(WTOpenA)
WinTabGlobalFunction(WTClose)
WinTabGlobalFunction(WTPacketsGet)
WinTabGlobalFunction(WTPacket)
WinTabGlobalFunction(WTEnable)
WinTabGlobalFunction(WTOverlap)
WinTabGlobalFunction(WTConfig)
WinTabGlobalFunction(WTGetA)
WinTabGlobalFunction(WTSetA)
WinTabGlobalFunction(WTExtGet)
WinTabGlobalFunction(WTExtSet)
WinTabGlobalFunction(WTSave)
WinTabGlobalFunction(WTRestore)
WinTabGlobalFunction(WTDataPeek)
WinTabGlobalFunction(WTQueueSizeSet)

internal void
LoadWintabFunctions()
{
    HINSTANCE ghWintab = LoadLibraryA("Wintab32.dll");
#define GETPROCADDRESS(func) gp##func = (type_##func)GetProcAddress(ghWintab, #func);
    GETPROCADDRESS(WTOpenA);
    GETPROCADDRESS(WTInfoA);
    GETPROCADDRESS(WTGetA);
    GETPROCADDRESS(WTSetA);
    GETPROCADDRESS(WTPacket);
    GETPROCADDRESS(WTClose);
    GETPROCADDRESS(WTEnable);
    GETPROCADDRESS(WTOverlap);
    GETPROCADDRESS(WTSave);
    GETPROCADDRESS(WTConfig);
    GETPROCADDRESS(WTRestore);
    GETPROCADDRESS(WTExtSet);
    GETPROCADDRESS(WTExtGet);
    GETPROCADDRESS(WTQueueSizeSet);
    GETPROCADDRESS(WTDataPeek);
    GETPROCADDRESS(WTPacketsGet);
}

struct window_mapping
{
    HWND Window;
    v2 Scalar;
    v2 Offset;
    v3 AxisMax;
    r32 PressureMax;
};

struct window_data
{
    v2s Pos;
    v2s Size;
    b32 Fullscreen;
};

struct image_data
{
    char NameBase[MAX_PATH]; // NOTE(Zyonji): YY-MM-DD_IIIIIIII_?min.jpg
    FIBITMAP *Bitmap;
    void *Memory;
    u32 Id;
    u32 Width;
    u32 Height;
    u32 Minutes;
};

struct win32_orca_state : orca_state
{
    open_gl OpenGL;
    char IniPath[MAX_PATH];
    
    window_mapping Map;
    window_data WindowData;
    image_data Image;
    b32 Active;
};

struct initial_state
{
    DWORD WindowStyle;
    s32 WindowX;
    s32 WindowY;
    s32 WindowWidth;
    s32 WindowHeight;
    u32 MaxId;
    v4 Color;
    u32 ColorMode;
    r32 Width;
};

global win32_orca_state *OrcaState;

internal void
LogError(char *Text, char *Caption)
{
    if(!OrcaState || !OrcaState->MaxId)
    {
        MessageBox(NULL, Text, Caption, MB_OK | MB_ICONHAND);
    }
    char PathBuffer[MAX_PATH];
    char *At = PathBuffer;
    char *From = OrcaState->IniPath;
    if(OrcaState)
    {
        while(*From)
        {
            *At++ = *From++;
        }
        At -= 4;
    }
    char *Error = "Error.txt";
    while(*Error)
    {
        *At++ = *Error++;
    }
    *At = *Error;
    HANDLE FileHandle = CreateFileA(PathBuffer, FILE_APPEND_DATA, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if(FileHandle != INVALID_HANDLE_VALUE)
    {
        DWORD BytesWritten = 0;
        u32 BytesToWrite = 0;
        At = Caption;
        while(*At)
        {
            At++;
            BytesToWrite++;
        }
        WriteFile(FileHandle, "\r\n", 2, &BytesWritten, 0);
        WriteFile(FileHandle, Caption, BytesToWrite, &BytesWritten, 0);
        WriteFile(FileHandle, ":\r\n", 3, &BytesWritten, 0);
        BytesToWrite = 0;
        At = Text;
        while(*At)
        {
            At++;
            BytesToWrite++;
        }
        WriteFile(FileHandle, Text, BytesToWrite, &BytesWritten, 0);
        CloseHandle(FileHandle);
    }
}

internal b32
InitWinTab(HWND Window, window_mapping *Map)
{
    b32 Result = false;
    
    LoadWintabFunctions();
    
    LOGCONTEXT Tablet;
    gpWTInfoA(WTI_DEFCONTEXT, 0, &Tablet);
    // TODO(Zyonji): check if I would need to enable in the Options to see the touch ring messages.
    Tablet.lcOptions |= CXO_MESSAGES;
    Tablet.lcPktData = PACKETDATA;
    Tablet.lcPktMode = PACKETMODE;
    Tablet.lcMoveMask = PACKETDATA;
    Tablet.lcBtnUpMask = Tablet.lcBtnDnMask;
    
    AXIS TabletX;
    AXIS TabletY;
    AXIS TabletZ;
    AXIS Pressure;
    
    gpWTInfoA(WTI_DEVICES, DVC_X, &TabletX);
    gpWTInfoA(WTI_DEVICES, DVC_Y, &TabletY);
    gpWTInfoA(WTI_DEVICES, DVC_Z, &TabletZ);
    gpWTInfoA(WTI_DEVICES, DVC_NPRESSURE, &Pressure);
    Tablet.lcInOrgX = 0;
    Tablet.lcInOrgY = 0;
    Tablet.lcInExtX = TabletX.axMax;
    Tablet.lcInExtY = TabletY.axMax;
    
    Map->AxisMax.x = (r32)TabletX.axMax;
    Map->AxisMax.y = (r32)TabletY.axMax;
    Map->AxisMax.z = (r32)TabletZ.axMax;
    Map->PressureMax = (r32)Pressure.axMax;
    
    Result = (gpWTOpenA(Window, &Tablet, TRUE) != 0) && TabletX.axMax && TabletY.axMax && TabletZ.axMax && Pressure.axMax;
    
    return(Result);
}

internal void
SetPixelFormat(open_gl *OpenGL, HDC WindowDC)
{
    int MatchingPixelFormatIndex = 0;
    GLuint ExtendedPick = 0;
    if(wglChoosePixelFormatARB)
    {
        int IntAttribList[] =
        {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,                 // 0
            WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB, // 1
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,                 // 2
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,                  // 3
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,           // 4
            0,
        };
        wglChoosePixelFormatARB(WindowDC, IntAttribList, 0, 1, &MatchingPixelFormatIndex, &ExtendedPick);
    }
    
    if(!ExtendedPick)
    {
        PIXELFORMATDESCRIPTOR DesiredPixelFormat = {};
        DesiredPixelFormat.nSize = sizeof(DesiredPixelFormat);
        DesiredPixelFormat.nVersion = 1;
        DesiredPixelFormat.iPixelType = PFD_TYPE_RGBA;
        DesiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW|PFD_DOUBLEBUFFER;
        DesiredPixelFormat.cColorBits = 32;
        DesiredPixelFormat.cAlphaBits = 8;
        DesiredPixelFormat.cDepthBits = 24;
        DesiredPixelFormat.iLayerType = PFD_MAIN_PLANE;
        
        MatchingPixelFormatIndex = ChoosePixelFormat(WindowDC, &DesiredPixelFormat);
    }
    
    PIXELFORMATDESCRIPTOR MatchingPixelFormat;
    DescribePixelFormat(WindowDC, MatchingPixelFormatIndex, sizeof(MatchingPixelFormat), &MatchingPixelFormat);
    SetPixelFormat(WindowDC, MatchingPixelFormatIndex, &MatchingPixelFormat);
}
internal void
LoadWGLExtensions(open_gl *OpenGL)
{
    WNDCLASSA WindowClass = {};
    WindowClass.lpfnWndProc = DefWindowProcA;
    WindowClass.hInstance = GetModuleHandle(0);
    WindowClass.lpszClassName = "OrcaHexWGLLoader";
    
    if(RegisterClassA(&WindowClass))
    {
        HWND Window = CreateWindowExA(
            0,
            WindowClass.lpszClassName,
            "OrcaHex",
            0,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            WindowClass.hInstance,
            0);
        
        HDC WindowDC = GetDC(Window);
        SetPixelFormat(OpenGL, WindowDC);
        HGLRC OpenGLRC = wglCreateContext(WindowDC);
        if(wglMakeCurrent(WindowDC, OpenGLRC))
        {
            wglChoosePixelFormatARB = (type_wglChoosePixelFormatARB *)wglGetProcAddress("wglChoosePixelFormatARB");
            wglCreateContextAttribsARB = (type_wglCreateContextAttribsARB *)wglGetProcAddress("wglCreateContextAttribsARB");
            wglMakeCurrent(0, 0);
        }
        
        wglDeleteContext(OpenGLRC);
        ReleaseDC(Window, WindowDC);
        DestroyWindow(Window);
    }
}
internal b32
InitOpenGL(open_gl *OpenGL, HDC WindowDC)
{
    LoadWGLExtensions(OpenGL);
    SetPixelFormat(OpenGL, WindowDC);
    
    b32 ModernContext = true;
    HGLRC OpenGLRC = 0;
    int Win32OpenGLAttribs[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
    };
    
    if(wglCreateContextAttribsARB)
    {
        OpenGLRC = wglCreateContextAttribsARB(WindowDC, 0, Win32OpenGLAttribs);
    }
    if(!OpenGLRC)
    {
        ModernContext = false;
        OpenGLRC = wglCreateContext(WindowDC);
    }
    
    if(wglMakeCurrent(WindowDC, OpenGLRC))
    {
        LoadOpenGLFunctions();
        
        if(glEnableVertexAttribArray && 
           glDisableVertexAttribArray && 
           glVertexAttribPointer && 
           glCreateShader && 
           glShaderSource && 
           glCompileShader && 
           glAttachShader && 
           glDeleteShader && 
           glCreateProgram && 
           glLinkProgram && 
           glValidateProgram && 
           glUseProgram && 
           glGetProgramiv && 
           glGetShaderInfoLog && 
           glGetProgramInfoLog && 
           glGetAttribLocation && 
           glGetUniformLocation && 
           glUniformMatrix4fv && 
           glUniform1f && 
           glUniform2fv && 
           glUniform4fv && 
           glUniform2iv)
        {
            if(glBindFramebuffer && 
               glGenFramebuffers && 
               glFramebufferTexture2D && 
               glDeleteFramebuffers && 
               glActiveTexture && 
               glGenerateMipmap)
            {
                if(glDrawBuffers && 
                   glGenBuffers && 
                   glBindBuffer && 
                   glBufferData && 
                   glBufferSubData && 
                   glGenVertexArrays && 
                   glBindVertexArray)
                {
                    if(glBlendColor)
                    {
                        OpenGL->Initialized = OpenGLInitPrograms(OpenGL);
                    }
                    else
                    {
                        LogError("Unable to set blend modes.", "OpenGL");
                    }
                }
                else
                {
                    LogError("Unable to set brush shape.", "OpenGL");
                }
            }
            else
            {
                LogError("Unable to draw on textures.", "OpenGL");
            }
        }
        else
        {
            LogError("Unable to use custom shaders.", "OpenGL");
        }
    }
    else
    {
        LogError("Unable to set OpenGL context.", "OpenGL");
    }
    
    return(OpenGL->Initialized);
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

// TODO(Zyonji): Load and save pngs myself.
internal image_data
LoadImage(char* Filename, u32 MaxId)
{
    image_data Result = {};
    
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
    
    char *At = Filename;
    for(; *At; ++At)
    {
        
    }
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
            char *To = Result.NameBase + 44;
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
    
    return(Result);
}

internal void
CreateBitmap(open_gl *OpenGL, image_data *ImageData)
{
    ImageData->Bitmap = FreeImage_Allocate(ImageData->Width, ImageData->Height, 32);
    ImageData->Memory = FreeImage_GetBits(ImageData->Bitmap);
    
    if(ImageData->Bitmap)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, OpenGL->CanvasFramebuffer.FramebufferHandle);
        glReadPixels(0, 0, ImageData->Width, ImageData->Height, GL_BGRA, GL_UNSIGNED_BYTE, ImageData->Memory);
    }
}

internal void
FreeBitmap(image_data *ImageData)
{
    FreeImage_Unload(ImageData->Bitmap);
    ImageData->Bitmap = 0;
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
    
    return(Result);
}

internal void
ChangeCanvas(open_gl *OpenGL, image_data ImageData, canvas_state *Canvas, v2u OutFrameSize)
{
    Canvas->MilliSeconds = ImageData.Minutes * 60000;
    Canvas->Size.Width = ImageData.Width;
    Canvas->Size.Height = ImageData.Height;
    ResetCanvasTransform(Canvas, OutFrameSize);
    
    ConvertImageToBuffer(OpenGL, ImageData.Memory, ImageData.Width, ImageData.Height, &OpenGL->CanvasFramebuffer, &OpenGL->SwapFramebuffer);
}

internal void
UpdateDisplayFrameData(open_gl *OpenGL, HWND Window, display_state *Display, menu_state *Menu)
{
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Display->Size = {(u32)(ClientRect.right - ClientRect.left), (u32)(ClientRect.bottom - ClientRect.top)};
    
    if(OpenGL->DisplayFramebuffer.FramebufferHandle)
    {
        FreeFramebuffer(&OpenGL->DisplayFramebuffer);
    }
    OpenGL->DisplayFramebuffer = CreateFramebuffer(OpenGL, Display->Size.Width, Display->Size.Height, 0);
    
    u32 Block;
    u32 Gap;
    u32area TimeBorder;
    u32area ColorBorder;
    u32area Pallet;
    if(Display->Size.Width > Display->Size.Height)
    {
        Menu->Size.Height = Display->Size.Height;
        Block = 8 * (Menu->Size.Height / 320);
        Gap = Block / 4;
        Menu->Block = Block;
        Menu->Gap = Gap;
        Menu->Size.Width = Block * 4;
        
        u32 Y = Menu->Size.Height - 2 * Gap - Block;
        Menu->New = {Gap, Y, Block, Block};
        Menu->Open = {2 * Gap + Block, Y, Block, Block};
        Menu->Save = {3 * Gap + 2 * Block, Y, Block, Block};
        Y -= 3 * Gap;
        Menu->Time = {0, Y, Block * 4, Gap};
        TimeBorder = {0, Y - Gap / 2, Block * 4, 2 * Gap};
        Y -= 2 * Gap + Block;
        Menu->RotateR = {Gap, Y, Block, Block};
        Menu->OneToOne = {2 * Gap + Block, Y, Block, Block};
        Menu->RotateL = {3 * Gap + 2 * Block, Y, Block, Block};
        Y -= Gap + Block;
        Menu->Minus = {Gap, Y, Block, Block};
        Menu->Reset = {2 * Gap + Block, Y, Block, Block};
        Menu->Plus = {3 * Gap + 2 * Block, Y, Block, Block};
        Y -= Gap + Block;
        Menu->Small = {Gap, Y, Block, Block};
        Menu->Mirror = {2 * Gap + Block, Y, Block, Block};
        Menu->Big = {3 * Gap + 2 * Block, Y, Block, Block};
        Y -= 4 * Gap + Block;
        Menu->ColorA = {3 * Gap, Y, Gap + Block, Gap + Block};
        Menu->ColorB = {4 * Gap + Block, Y, Gap + Block, Gap + Block};
        ColorBorder = {2 * Gap, Y - Gap, 4 * Gap + 2 * Block, 3 * Gap + Block};
        Y -= 5 * Gap + 3 * Block;
        Pallet = {Gap, Y, 2 * Gap + 3 * Block, 2 * Gap + 3 * Block};
        Y -= 3 * Gap;
        Menu->Steps = 12;
        Y -= Menu->Steps * Block;
        Menu->Alpha = {3 * Gap, Y, Block, Block};
        Menu->L = {Gap + 2 * Block, Y, Block, Block};
        Menu->Offset = {0, (s32)Block};
        Y -= 2 * Gap + Block;
        Menu->AlphaButton = {3 * Gap, Y, Block, Block};
        Menu->LButton = {Gap + 2 * Block, Y, Block, Block};
        Y -= 2 * Gap + Block;
        Menu->abButton = {2 * Gap + Block, Y, Block, Block};
        Y -= 2 * Gap;
        
        u32 Remainder = Y - 2 * Gap;
        Menu->a = {Gap, Y - Remainder, 5 * Gap / 2 + Block, Remainder};
        Menu->b = {Gap / 2 + 2 * Block, Y - Remainder, 5 * Gap / 2 + Block, Remainder};
    }
    else
    {
        Menu->Size.Width = Display->Size.Width;
        Block = 8 * (Menu->Size.Width / 320);
        Gap = Block / 4;
        Menu->Block = Block;
        Menu->Gap = Gap;
        Menu->Size.Height = Menu->Block * 4;
        
        u32 X = 2 * Gap;
        Menu->New = {X, Gap, Block, Block};
        Menu->Open = {X, 2 * Gap + Block, Block, Block};
        Menu->Save = {X, 3 * Gap + 2 * Block, Block, Block};
        X += Block + 2 * Gap;
        Menu->Time = {X, 0, Gap, Block * 4};
        TimeBorder = {X - Gap / 2, 0, 2 * Gap, Block * 4};
        X += 3 * Gap;
        Menu->Small = {X, Gap, Block, Block};
        Menu->Minus = {X, 2 * Gap + Block, Block, Block};
        Menu->RotateR = {X, 3 * Gap + 2 * Block, Block, Block};
        X += Gap + Block;
        Menu->Mirror = {X, Gap, Block, Block};
        Menu->Reset = {X, 2 * Gap + Block, Block, Block};
        Menu->OneToOne = {X, 3 * Gap + 2 * Block, Block, Block};
        X += Gap + Block;
        Menu->Big = {X, Gap, Block, Block};
        Menu->Plus = {X, 2 * Gap + Block, Block, Block};
        Menu->RotateL = {X, 3 * Gap + 2 * Block, Block, Block};
        X += 3 * Gap + Block;
        Menu->ColorA = {X, 3 * Gap, Gap + Block, Gap + Block};
        Menu->ColorB = {X, 4 * Gap + Block, Gap + Block, Gap + Block};
        ColorBorder = {X - Gap, 2 * Gap, 3 * Gap + Block, 4 * Gap + 2 * Block};
        X += 4 * Gap + Block;
        Pallet = {X, Gap, 2 * Gap + 3 * Block, 2 * Gap + 3 * Block};
        X += 5 * Gap + 2 * Block;
        Menu->Steps = 12;
        X += Menu->Steps * Block;
        Menu->Alpha = {X, 3 * Gap, Block, Block};
        Menu->L = {X, Gap + 2 * Block, Block, Block};
        Menu->Offset = {-(s32)Block, 0};
        X += 2 * Gap + Block;
        Menu->AlphaButton = {X, 3 * Gap, Block, Block};
        Menu->LButton = {X, Gap + 2 * Block, Block, Block};
        X += 2 * Gap + Block;
        Menu->abButton = {X, 2 * Gap + Block, Block, Block};
        X += 2 * Gap + Block;
        
        u32 Remainder = Menu->Size.Width - X - 2 * Gap;
        Menu->a = {X, Gap, Remainder, 5 * Gap / 2 + Block};
        Menu->b = {X, Gap / 2 + 2 * Block, Remainder, 5 * Gap / 2 + Block};
    }
    // TODO(Zyonji): Is this still required?
    Menu->Origin = {(r32)(Display->Size.Width / 2), (r32)(Display->Size.Height / 2)};
    
    if(OpenGL->MenuFramebuffer.FramebufferHandle)
    {
        FreeFramebuffer(&OpenGL->MenuFramebuffer);
    }
    OpenGL->MenuFramebuffer = CreateFramebuffer(OpenGL, Menu->Size.Width, Menu->Size.Height, 0);
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGL->MenuFramebuffer.FramebufferHandle);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->ScreenFillVertexBuffer);
    glViewport(0, 0, Menu->Size.Width, Menu->Size.Height);
    glScissor(0, 0, Menu->Size.Width, Menu->Size.Height);
    glClearColor(0.9, 0.9, 0.9, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    DrawButtonNew(OpenGL, Menu->New);
    DrawButtonOpen(OpenGL, Menu->Open);
    DrawButtonSave(OpenGL, Menu->Save);
    DrawButton(TimeBorder);
    DrawButtonRotateR(OpenGL, Menu->RotateR);
    DrawButtonRotateL(OpenGL, Menu->RotateL);
    DrawButtonOneToOne(OpenGL, Menu->OneToOne);
    DrawButtonReset(OpenGL, Menu->Reset);
    DrawButtonMinus(OpenGL, Menu->Minus);
    DrawButtonPlus(OpenGL, Menu->Plus);
    DrawButtonSmall(OpenGL, Menu->Small);
    DrawButtonBig(OpenGL, Menu->Big);
    DrawButtonMirror(OpenGL, Menu->Mirror);
    DrawButton(ColorBorder);
    DrawPallet(Pallet, Gap);
    if(Display->Size.Width > Display->Size.Height)
    {
        DrawToggleTop(OpenGL, Menu->AlphaButton, Gap);
        DrawToggleTop(OpenGL, Menu->LButton, Gap);
        DrawToggleBottom(OpenGL, Menu->abButton, Gap);
    }
    else
    {
        DrawToggleLeft(OpenGL, Menu->AlphaButton, Gap);
        DrawToggleLeft(OpenGL, Menu->LButton, Gap);
        DrawToggleRight(OpenGL, Menu->abButton, Gap);
    }
}

internal v2
MapTabletInput(v2 TabletPoint, window_mapping Map)
{
    v2 Result = {};
    Result.x = TabletPoint.x * Map.Scalar.x - Map.Offset.x;
    Result.y = TabletPoint.y * Map.Scalar.y - Map.Offset.y;
    return(Result);
}

internal void
ComputeTabletMapping(window_mapping *Map)
{
    HMONITOR Monitor;
    Monitor = MonitorFromWindow(Map->Window, MONITOR_DEFAULTTONEAREST);
    MONITORINFO MonitorInfo;
    MonitorInfo.cbSize = sizeof(MonitorInfo);
    GetMonitorInfoA(Monitor, &MonitorInfo);
    r32 ScreenWidth = (r32)(MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left);
    r32 ScreenHeight = (r32)(MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top);
    r32 ScreenLeft = (r32)MonitorInfo.rcMonitor.left;
    r32 ScreenBottom = (r32)MonitorInfo.rcMonitor.bottom;
    
    RECT ClientRect;
    GetClientRect(Map->Window, &ClientRect);
    POINT Center;
    Center.x = (ClientRect.right + ClientRect.left) / 2;
    Center.y = (ClientRect.bottom + ClientRect.top) / 2;
    ClientToScreen(Map->Window, &Center);
    
    Map->Scalar.x = ScreenWidth / Map->AxisMax.x;
    Map->Scalar.y = ScreenHeight / Map->AxisMax.y;
    Map->Offset.x = (r32)Center.x - ScreenLeft;
    Map->Offset.y = ScreenBottom - (r32)Center.y;
}

internal void
ToggleFullscreen(HWND Window, window_data *WindowData, b32 Force16by9Full)
{
    if(WindowData->Fullscreen && !Force16by9Full)
    {
        SetWindowLongPtr(Window, GWL_STYLE, WS_VISIBLE | WS_OVERLAPPEDWINDOW);
        SetWindowPos(Window, HWND_NOTOPMOST, WindowData->Pos.x, WindowData->Pos.y, WindowData->Size.Width, WindowData->Size.Height, SWP_FRAMECHANGED);
        WindowData->Fullscreen = false;
    }
    else 
    {
        HMONITOR Monitor;
        Monitor = MonitorFromWindow(Window, MONITOR_DEFAULTTONEAREST);
        MONITORINFO MonitorInfo;
        MonitorInfo.cbSize = sizeof(MonitorInfo);
        if(GetMonitorInfoA(Monitor, &MonitorInfo))
        {
            s32 ScreenHeight = MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top;
            s32 ScreenWidth;
            if(Force16by9Full)
            {
                ScreenWidth = (ScreenHeight * 16) / 9;
            }
            else
            {
                // TODO(Zyonji): Find a way to prevent exclusive full screen mode.
                ScreenWidth = MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left + 1;
            }
            s32 ScreenLeft = MonitorInfo.rcMonitor.left;
            s32 ScreenTop = MonitorInfo.rcMonitor.top;
            
            WindowData->Fullscreen = true;
            SetWindowLongPtr(Window, GWL_STYLE, WS_VISIBLE | WS_POPUP);
            SetWindowPos(Window, HWND_NOTOPMOST, ScreenLeft, ScreenTop, ScreenWidth, ScreenHeight, SWP_FRAMECHANGED);
        }
        else
        {
            LogError("Unable to read monitor data.", "Windows");
        }
    }
}
internal void
ToggleFullscreen(HWND Window, window_data *WindowData)
{
    ToggleFullscreen(Window, WindowData, false);
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

internal initial_state
LoadInitialState(char *Path)
{
    initial_state Result = {};
    HANDLE FileHandle = CreateFileA(Path, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if(FileHandle != INVALID_HANDLE_VALUE)
    {
        DWORD BytesRead = 0;
        ReadFile(FileHandle, &Result, sizeof(Result), &BytesRead, 0);
        CloseHandle(FileHandle);
    }
    return(Result);
}

// TODO(Zyonji): Set the parameters to show the requirements of the function.  Maybe split this into multiple functions to make reading code easier.
SAVE_EVERYTHING(SaveEverything)
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
        
        if(OrcaState->Image.Height && OrcaState->Image.Width)
        {
            if(OrcaState->Image.Id >= OrcaState->MaxId)
            {
                OrcaState->MaxId = OrcaState->Image.Id + 1;
            }
            CreateBitmap(&OrcaState->OpenGL, &OrcaState->Image);
            u32 NewTime = (u32)(OrcaState->Canvas.MilliSeconds / 60000);
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
        CurrentState.ColorMode = OrcaState->Pen.ColorMode;
        CurrentState.Width = OrcaState->Pen.Width;
        SaveInitialState(CurrentState, OrcaState->IniPath);
    }
}

// TODO(Zyonji): Considder a choice for canvas size.
CREATE_NEW_FILE(CreateNewFile)
{
    image_data ImageData = {};
    ImageData.Width = 4000;
    ImageData.Height = 6000;
    ChangeCanvas(&OrcaState->OpenGL, ImageData, &OrcaState->Canvas, DisplaySize);
    FreeBitmap(&ImageData);
    OrcaState->Image = ImageData;
    OrcaState->Image.Id = MaxId;
}

internal void
LoadCanvas(open_gl *OpenGL, image_data *Image, canvas_state *Canvas, char* FileName, v2u OutFrameSize, u32 MaxId)
{
    image_data ImageData = LoadImage(FileName, MaxId);
    if(ImageData.Bitmap)
    {
        ChangeCanvas(OpenGL, ImageData, Canvas, OutFrameSize);
        FreeBitmap(&ImageData);
        *Image = ImageData;
    }
    else
    {
        LogError("Unable to load the image.", "FreeImage");
    }
}
LOAD_FILE(LoadFile)
{
    char FileName[MAX_PATH];
    RequestFileChoice(FileName, sizeof(FileName));
    if(*FileName)
    {
        LoadCanvas(&OrcaState->OpenGL, &OrcaState->Image, Canvas, FileName, DisplaySize, MaxId);
    }
}

PICK_COLOR(PickColor)
{
    return(PickColor(&OrcaState->OpenGL, P, Size));
}
UPDATE_MENU(UpdateMenu)
{
    UpdateMenu(&OrcaState->OpenGL, Menu, Pen);
}
RENDER_BRUSH_STROKE(RenderBrushStroke)
{
    RenderBrushStroke(&OrcaState->OpenGL, Canvas, OldP, NewP, OldV, NewV, OldColor, NewColor, OldAliasDistance, NewAliasDistance, ColorMode);
}

internal u64
GetFileTime()
{
    u64 Result;
    SYSTEMTIME SystemTime;
    FILETIME FileTime;
    GetSystemTime(&SystemTime);
    SystemTimeToFileTime(&SystemTime, &FileTime);
    Result = (((u64) FileTime.dwHighDateTime) << 32) | ((u64) FileTime.dwLowDateTime);
    return(Result);
}

LRESULT CALLBACK
MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;
    
    switch(Message)
    {
        case WM_DESTROY:
        case WM_CLOSE:
        {
            if(OrcaState->Canvas.Hot)
            {
                s32 MessageBoxValue = MessageBox(Window, "Save before quitting?", "Closing", MB_YESNOCANCEL | MB_DEFBUTTON1 | MB_APPLMODAL);
                if(MessageBoxValue == IDYES)
                {
                    OrcaState->SaveEverything();
                }
                if(MessageBoxValue == IDYES || MessageBoxValue == IDNO)
                {
                    PostQuitMessage(0);
                }
            }
            else
            {
                PostQuitMessage(0);
            }
        } break;
        
        case WM_SIZE:
        {
            if(OrcaState->OpenGL.Initialized)
            {
                UpdateDisplayFrameData(&OrcaState->OpenGL, Window, &OrcaState->Display, &OrcaState->Menu);
                OrcaState->UpdateMenu(&OrcaState->Menu, OrcaState->Pen);
                InvalidateRect(Window, 0, true);
            }
        }
        // NOTE(Zyonji): WM_SIZE executes WM_MOVE too.
        case WM_MOVE:
        {
            if(OrcaState)
            {
                if(!OrcaState->WindowData.Fullscreen)
                {
                    // NOTE(Zyonji): Saves the window data to return to after exiting fullscreen.
                    RECT WindowRect;
                    GetWindowRect(Window, &WindowRect);
                    OrcaState->WindowData.Pos.x = WindowRect.left;
                    OrcaState->WindowData.Pos.y = WindowRect.top;
                    OrcaState->WindowData.Size.Width = WindowRect.right - WindowRect.left;
                    OrcaState->WindowData.Size.Height = WindowRect.bottom - WindowRect.top;
                }
                
                ComputeTabletMapping(&OrcaState->Map);
            }
        } break;
        
        case WM_PAINT:
        {
            if(OrcaState->OpenGL.Initialized)
            {
                u32 Minutes = (u32)(OrcaState->Canvas.MilliSeconds / 60000) - OrcaState->Image.Minutes;
                DisplayBuffer(&OrcaState->OpenGL, OrcaState->Display, OrcaState->Pen, OrcaState->Canvas, OrcaState->Menu, Minutes);
            }
            ValidateRect(Window, 0);
        } break;
        
        case WM_KEYDOWN:
        {
            switch(WParam)
            {
                case 'K':
                {
                    ChangeBrushSize(&OrcaState->Pen, 0.8);
                    InvalidateRect(Window, 0, true);
                } break;
                case 'L':
                {
                    ChangeBrushSize(&OrcaState->Pen, 1.25);
                    InvalidateRect(Window, 0, true);
                } break;
                
                case 'O':
                {
                    ChangeCanvasScale(&OrcaState->Canvas, 0.8);
                    InvalidateRect(Window, 0, true);
                } break;
                case 'I':
                {
                    ChangeCanvasScale(&OrcaState->Canvas, 1.25);
                    InvalidateRect(Window, 0, true);
                } break;
                
                case 'H':
                {
                    FlipCanvas(&OrcaState->Canvas);
                    InvalidateRect(Window, 0, true);
                } break;
                
                case 'M':
                {
                    RotateCanvas(&OrcaState->Canvas, 0.0625f * Pi32);
                    InvalidateRect(Window, 0, true);
                } break;
                case 'N':
                {
                    RotateCanvas(&OrcaState->Canvas, -0.0625f * Pi32);
                    InvalidateRect(Window, 0, true);
                } break;
                
                case VK_F11:
                {
                    ToggleFullscreen(Window, &OrcaState->WindowData);
                } break;
                case VK_F12:
                {
                    ToggleFullscreen(Window, &OrcaState->WindowData, true);
                } break;
            }
        } break;
        
        case WT_PACKET:
        {
            PACKET Packet;
            if(gpWTPacket((HCTX)LParam, (UINT)WParam, &Packet)) 
            {
                v2 TabletPoint = {(r32)Packet.pkX, (r32)Packet.pkY};
                
                pen_state OldPenState = OrcaState->Pen;
                pen_state NewPenState = {};
                NewPenState.P = MapTabletInput(TabletPoint, OrcaState->Map);
                NewPenState.Color = OldPenState.Color;
                NewPenState.ColorMode = OldPenState.ColorMode;
                NewPenState.Width = OldPenState.NextWidth;
                NewPenState.NextWidth = NewPenState.Width;
                NewPenState.IsDown = Packet.pkNormalPressure;
                NewPenState.Time = GetFileTime() / 10000;
                NewPenState.Buttons = Packet.pkButtons;
                if(OldPenState.Pressure > 0 || NewPenState.P.x + OrcaState->Menu.Origin.x > OrcaState->Menu.Size.Width || NewPenState.P.y + OrcaState->Menu.Origin.y > OrcaState->Menu.Size.Width)
                {
                    NewPenState.Pressure = (r32)Packet.pkNormalPressure / OrcaState->Map.PressureMax;
                }
                r32 HalfWidth = NewPenState.Width / 2;
                r32 Radians = (Pi32 / 1800.0f) * (r32)Packet.pkOrientation.orAzimuth;
                NewPenState.V.x = HalfWidth * sinf(Radians);
                NewPenState.V.y = HalfWidth * cosf(Radians);
                
                b32 IsPenClose = (Packet.pkZ < 2 * OrcaState->Map.AxisMax.z / 3);
                
                if(OldPenState.Time + 5000 > NewPenState.Time)
                {
                    if(OldPenState.Time < NewPenState.Time)
                    {
                        OrcaState->Canvas.MilliSeconds += (NewPenState.Time - OldPenState.Time);
                    }
                    else if(OldPenState.Time > NewPenState.Time)
                    {
                        char Buffer[260];
                        wsprintf(Buffer, "Time went from %02d to %02d", OldPenState.Time, NewPenState.Time);
                        MessageBox(NULL, Buffer, "TimeRollover", MB_OK | MB_ICONHAND);
                    }
                }
                
                if(ProcessBrushMove(OrcaState, OrcaState->Pen, NewPenState, IsPenClose))
                {
                    InvalidateRect(Window, 0, true);
                }
            }
        } break;
        
        default:
        {
            Result = DefWindowProc(Window, Message, WParam, LParam);
        } break;
    }
    
    return(Result);
}

int CALLBACK
WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CommandLine, int ShowCode)
{
    WNDCLASS WindowClass = {};
    
    WindowClass.lpfnWndProc = MainWindowCallback;
    WindowClass.hInstance = Instance;
    WindowClass.hCursor = LoadCursor(0, IDC_ARROW);
    //WindowClass.hIcon;
    WindowClass.lpszClassName = "OrcaHexClass";
    
    if(RegisterClassA(&WindowClass))
    {
        umm GeneralMemory = (umm)VirtualAlloc(0, sizeof(orca_state), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
        if(GeneralMemory)
        {
            OrcaState = (win32_orca_state *)GeneralMemory;
            OrcaState->PickColor = CPUPickColor;
            OrcaState->UpdateMenu = CPUUpdateMenu;
            OrcaState->CreateNewFile = CPUCreateNewFile;
            OrcaState->SaveEverything = CPUSaveEverything;
            OrcaState->LoadFile = LoadFile;
            OrcaState->RenderBrushStroke = CPURenderBrushStroke;
            
            char *Path = OrcaState->IniPath;
            {
                u32 Length = GetModuleFileNameA(0, Path, sizeof(OrcaState->IniPath));
                char *At = Path + Length;
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
            
            initial_state InitialState = LoadInitialState(Path);
            if(!InitialState.MaxId)
            {
                InitialState.WindowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
                InitialState.WindowX = 10;
                InitialState.WindowY = 10;
                InitialState.WindowWidth = 920;
                InitialState.WindowHeight = 680;
                InitialState.Color = {0.5f, 0.5f, 0.5f, 1.0f};
                InitialState.ColorMode = COLOR_MODE_PRESSURE | COLOR_MODE_CHROMA | COLOR_MODE_LUMINANCE;
                InitialState.Width = 16.0f;
                InitialState.MaxId = 1;
            }
            OrcaState->MaxId = InitialState.MaxId;
            OrcaState->Pen.Color = InitialState.Color;
            OrcaState->Pen.ColorMode = InitialState.ColorMode;
            OrcaState->Pen.NextWidth = InitialState.Width;
            OrcaState->WindowData.Pos.x = InitialState.WindowX;
            OrcaState->WindowData.Pos.y = InitialState.WindowY;
            OrcaState->WindowData.Size.Width = InitialState.WindowWidth;
            OrcaState->WindowData.Size.Height = InitialState.WindowHeight;
            OrcaState->Canvas.Scale = 1;
            
            HWND Window = CreateWindowExA(
                0,
                WindowClass.lpszClassName,
                "OrcaHex",
                WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                InitialState.WindowX,
                InitialState.WindowY,
                InitialState.WindowWidth,
                InitialState.WindowHeight,
                0,
                0,
                Instance,
                0);
            
            if(Window)
            {
                if(InitOpenGL(&OrcaState->OpenGL, GetDC(Window)))
                {
                    if(InitWinTab(Window, &OrcaState->Map))
                    {
                        OrcaState->PickColor = PickColor;
                        OrcaState->UpdateMenu = UpdateMenu;
                        OrcaState->CreateNewFile = CreateNewFile;
                        OrcaState->SaveEverything = SaveEverything;
                        OrcaState->RenderBrushStroke = RenderBrushStroke;
                        
                        // TODO(Zyonji): Move away from using an image file for the menu.
                        OrcaState->Map.Window = Window;
                        ComputeTabletMapping(&OrcaState->Map);
                        UpdateDisplayFrameData(&OrcaState->OpenGL, Window, &OrcaState->Display, &OrcaState->Menu);
                        UpdateMenu(&OrcaState->Menu, OrcaState->Pen);
                        
                        if(InitialState.WindowStyle & WS_POPUP)
                        {
                            ToggleFullscreen(Window, &OrcaState->WindowData);
                        }
                        
                        // TODO(Zyonji): This has some awkward load times at the start.  Look if I want to change that.
                        if(CommandLine && *CommandLine != '\0')
                        {
                            LoadCanvas(&OrcaState->OpenGL, &OrcaState->Image, &OrcaState->Canvas, CommandLine, OrcaState->Display.Size, OrcaState->MaxId);
                        }
                        
                        // NOTE(Zyonji): The main Message loop.
                        for(;;)
                        {
                            MSG Message;
                            BOOL MessageResult = GetMessageA(&Message, 0, 0, 0);
                            if(MessageResult > 0)
                            {
                                TranslateMessage(&Message);
                                DispatchMessageA(&Message);
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                    else
                    {
                        LogError("Unable to load a tablet context.", "WinTab");
                    }
                }
                else
                {
                    LogError("Unable to load OpenGL", "OpenGL");
                }
            }
            else
            {
                LogError("Unable to open a Window.", "Windows");
            }
        }
        else
        {
            LogError("Unable to allocate memory.", "Windows");
        }
    }
    else
    {
        LogError("Unable to register a Windowclass.", "Windows");
    }
    
    return(0);
}