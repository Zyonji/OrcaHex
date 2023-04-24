#include <windows.h>
#include <GL/gl.h>

#include "types.h"
#include "orcahex.cpp"

#include "wintab.h"
#define PACKETDATA (PK_X | PK_Y | PK_Z | PK_NORMAL_PRESSURE | PK_ORIENTATION | PK_TIME | PK_BUTTONS)
//(PK_X | PK_Y | PK_Z | PK_NORMAL_PRESSURE | PK_TANGENT_PRESSURE | PK_ORIENTATION | PK_TIME | PK_BUTTONS | PK_CURSOR)
#define PACKETMODE 0
#define PACKETTOUCHRING PKEXT_ABSOLUTE
#include "pktdef.h"

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
typedef void WINAPI type_glUniform1i(GLint location, GLint v0);
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
OpenGLGlobalFunction(glUniform1i)
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

internal b32
LoadOpenGLFunctions()
{
#define Win32GetOpenGLFunction(Name) if((Name = (type_##Name *)wglGetProcAddress(#Name)) == 0) return false
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
    Win32GetOpenGLFunction(glUniform1i);
    Win32GetOpenGLFunction(glUniform2iv);
    Win32GetOpenGLFunction(glBlendColor);
    Win32GetOpenGLFunction(glGenerateMipmap);
    
    return true;
}

#include "orcahex_data.cpp"
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

internal b32
LoadWintabFunctions()
{
    HINSTANCE Wintab32 = LoadLibraryA("Wintab32.dll");
#define GETPROCADDRESS(Name) if((gp##Name = (type_##Name)GetProcAddress(Wintab32, #Name)) == 0) return false
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
    
    return true;
}

struct window_mapping
{
    HWND Window;
    v2 Scalar;
    v2 Offset;
    v3 AxisMax;
    r32 PressureMax;
    r32 TouchRingMax;
};

struct window_data
{
    v2s Pos;
    v2s Size;
    b32 Fullscreen;
};

struct win32_orca_state : orca_state
{
    open_gl OpenGL;
    window_mapping Map;
    window_data WindowData;
    image_data Image;
    HWND StreamWindow;
    HCTX TabletContext;
    b32 SleepIsGranular;
    b32 Active;
    s64 LastFrameTime;
    r32 PerformanceFrequency;
    u64 LastPenTime;
    char IniPath[MAX_PATH];
    char RefPath[MAX_PATH];
};

global win32_orca_state *OrcaState;
global b32 GlobalReplayModeChange;

inline r32 
Win32GetSecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End)
{
    real32 Result = ((r32)(End.QuadPart - Start.QuadPart) /
                     OrcaState->PerformanceFrequency);
    return(Result);
}

internal void ChangeCanvas(open_gl *OpenGL, image_data ImageData, canvas_state *Canvas, u32area PaintingRegion);
internal void FreeBitmap(image_data *ImageData);

internal void CreateBitmap(open_gl *OpenGL, image_data *ImageData);
internal u64 GetFileTime();
#include "win32_file_manager.cpp"
#include "orcahex_replay.cpp"

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

internal HCTX
InitWinTab(HWND Window, window_mapping *Map)
{
    HCTX Result = 0;
    if(!LoadWintabFunctions())
        return(Result);
    
    WTPKT TouchRingMask = 0;
	UINT TouchRingIndex = 0xFFFFFFFF;
    for(UINT i = 0, ExtensionTag = 0; gpWTInfoA(WTI_EXTENSIONS + i, EXT_TAG, &ExtensionTag); i++)
    {
        if(ExtensionTag == WTX_TOUCHRING)
        {
            TouchRingIndex = i;
        }
    }
    
    if(TouchRingIndex != 0xFFFFFFFF)
    {
        gpWTInfoA(WTI_EXTENSIONS + TouchRingIndex, EXT_MASK, &TouchRingMask);
        AXIS TouchRing;
        gpWTInfoA(WTI_EXTENSIONS + TouchRingIndex, EXT_AXES, &TouchRing);
        Map->TouchRingMax = (r32)TouchRing.axMax + 1;
	}
    
    LOGCONTEXT Tablet;
    if(!gpWTInfoA(WTI_DEFCONTEXT, 0, &Tablet))
        return(Result);
    
    Tablet.lcOptions |= CXO_MESSAGES; //CXO_CSRMESSAGES for handling multiple cursors.
    // TODO(Zyonji): Check how locking CXL_INSIZE works.
    //Tablet.lcLock = CXL_INSIZE;
    Tablet.lcPktData = Tablet.lcMoveMask = PACKETDATA | TouchRingMask;
    Tablet.lcPktMode = PACKETMODE;
    Tablet.lcBtnUpMask = Tablet.lcBtnDnMask;
    Tablet.lcOutOrgX = 0;
    Tablet.lcOutExtX = Tablet.lcInExtX;
    Tablet.lcOutOrgY = 0;
    Tablet.lcOutExtY = Tablet.lcInExtY;
    
    AXIS Pressure; //, Orientation[3];
    gpWTInfoA(WTI_DEVICES, DVC_NPRESSURE, &Pressure);
    //gpWTInfoA(WTI_DEVICES, DVC_ORIENTATION, &Orientation);
    
    if(!Pressure.axMax)
        return(Result);
    
    Map->AxisMax.x = (r32)Tablet.lcOutExtX;
    Map->AxisMax.y = (r32)Tablet.lcOutExtY;
    Map->AxisMax.z = (r32)Tablet.lcOutExtZ;
    Map->PressureMax = (r32)Pressure.axMax;
    
    Result = gpWTOpenA(Window, &Tablet, TRUE);
    
    return(Result);
}

internal void
SetPixelFormat(HDC WindowDC)
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
LoadWGLExtensions()
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
        SetPixelFormat(WindowDC);
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
    LoadWGLExtensions();
    
    SetPixelFormat(WindowDC);
    
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
        if(!LoadOpenGLFunctions())
            return false;
        
        OpenGL->Initialized = OpenGLInitPrograms(OpenGL);
        OpenGL->RenderingContext = OpenGLRC;
        OpenGL->TilesetFramebuffer = CreateConvertedFramebuffer(OpenGL, TilesWidth, TilesHeight, TilesMemory);
    }
    else
    {
        LogError("Unable to set OpenGL context.", "OpenGL");
    }
    
    return(OpenGL->Initialized);
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
internal u32
SecondsPassed(u64 *LastTime)
{
    u32 Result;
    u64 CurrentTime = GetFileTime();
    Result = (u32)((CurrentTime - *LastTime) / 10000000);
    *LastTime = CurrentTime;
    return(Result);
}

internal void
CreateBitmap(open_gl *OpenGL, image_data *ImageData)
{
    ImageData->Bitmap = FreeImage_Allocate(ImageData->Width, ImageData->Height, 32);
    ImageData->Memory = FreeImage_GetBits(ImageData->Bitmap);
    
    if(ImageData->Bitmap)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGL->SwapFramebuffer.FramebufferHandle);
        glViewport(0, 0, ImageData->Width, ImageData->Height);
        glScissor(0, 0, ImageData->Width, ImageData->Height);
        glBindBuffer(GL_ARRAY_BUFFER, OpenGL->ScreenFillVertexBuffer);
        m4x4 Transform = {
            1, 0, 0, 0, 
            0, 1, 0, 0, 
            0, 0, 1, 0, 
            0, 0, 0, 1,};
        OpenGLProgramBegin(&OpenGL->ConvertToRGBProgram, Transform);
        glBindTexture(GL_TEXTURE_2D, OpenGL->CanvasFramebuffer.ColorHandle);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        OpenGLProgramEnd(&OpenGL->ConvertToRGBProgram);
        
        glBindFramebuffer(GL_FRAMEBUFFER, OpenGL->SwapFramebuffer.FramebufferHandle);
        glReadPixels(0, 0, ImageData->Width, ImageData->Height, GL_BGRA, GL_UNSIGNED_BYTE, ImageData->Memory);
    }
}

internal void
FreeBitmap(image_data *ImageData)
{
    FreeImage_Unload(ImageData->Bitmap);
    ImageData->Bitmap = 0;
}

internal void
ChangeCanvas(open_gl *OpenGL, image_data ImageData, canvas_state *Canvas, u32area PaintingRegion)
{
    Canvas->StartingTime = GetFileTime();
    Canvas->SecondsIdle = -60 * ImageData.Minutes;
    Canvas->Size.Width = ImageData.Width;
    Canvas->Size.Height = ImageData.Height;
    ResetCanvasTransform(Canvas, PaintingRegion);
    
    ConvertImageToBuffer(OpenGL, ImageData.Memory, ImageData.Width, ImageData.Height, &OpenGL->CanvasFramebuffer, &OpenGL->SwapFramebuffer);
}

internal void
UpdateDisplayFrameData(open_gl *OpenGL, HWND Window, v2u *DisplaySize, u32area *PaintingRegion, menu_state *Menu)
{
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    *DisplaySize = {(u32)(ClientRect.right - ClientRect.left), (u32)(ClientRect.bottom - ClientRect.top)};
    
    if(OpenGL->DisplayFramebuffer.FramebufferHandle)
    {
        FreeFramebuffer(&OpenGL->DisplayFramebuffer);
    }
    OpenGL->DisplayFramebuffer = CreateFramebuffer(OpenGL, DisplaySize->Width, DisplaySize->Height, 0);
    
    u32 Block;
    u32 Gap;
    u32area TimeBorder;
    u32area Pallet;
    if(DisplaySize->Width > DisplaySize->Height)
    {
        Menu->Size.Height = DisplaySize->Height;
        Gap = Menu->Size.Height / 320;
        Block = 8 * Gap;
        Menu->Block = Block;
        Menu->Gap = Gap;
        Menu->Size.Width = Block * 3 + Gap * 4;
        *PaintingRegion = {
            Menu->Size.Width, 0, DisplaySize->Width - Menu->Size.Width, DisplaySize->Height
        };
        
        u32 Y = Menu->Size.Height - 2 * Gap - Block;
        Menu->New = {Gap, Y, Block, Block};
        Menu->Open = {2 * Gap + Block, Y, Block, Block};
        Menu->Save = {3 * Gap + 2 * Block, Y, Block, Block};
        Y -= Block * 3 / 4;
        Menu->Time = {0, Y + Block / 4, Menu->Size.Width, Block / 4};
        TimeBorder = {0, Y, Menu->Size.Width, Block * 3 / 4};
        Y -= Block;
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
        Y -= Gap + Block / 2;
        Menu->PenButtonsToggle = {Gap, Y, Block / 2, Block / 2};
        Menu->TiltToggle = {2 * Gap + Block / 2, Y, Block / 2, Block / 2};
        Menu->LinePenToggle = {3 * Gap + Block, Y, Block / 2, Block / 2};
        Menu->BrushStyleSwitch = {7 * Gap + Block * 2, Y, Block / 2, Block / 2};
        Y -= Gap + Block;
        Menu->ColorA = {2 * Gap + Block / 2, Y, Block, Block};
        Menu->ColorB = {2 * Gap + Block * 3 / 2, Y, Block, Block};
        Menu->ColorButtonA = {2 * Gap, Y, Block / 2, Block};
        Menu->ColorButtonB = {2 * Gap + Block * 5 / 2, Y, Block / 2, Block};
        Y -= 2 * Gap + 3 * Block;
        Pallet = {2 * Gap, Y, 3 * Block, 3 * Block};
        Y -= 2 * Gap;
        Menu->Steps = 12;
        Y -= Menu->Steps * Block;
        Menu->Alpha = {2 * Gap + Block / 4, Y, Block, Block};
        Menu->LSmooth = {2 * Gap + Block* 6 / 4, Y, Block / 4, Menu->Steps * Block};
        Menu->L = {2 * Gap + Block* 7 / 4, Y, Block, Block};
        Menu->Offset = {0, (s32)Block};
        Y -= Block / 2;
        Menu->AlphaButton = {2 * Gap + Block / 4, Y, Block, Block / 2};
        Menu->LLimitToggle = {2 * Gap + Block * 5 / 4, Y, Block / 2, Block / 2};
        Menu->LButton = {2 * Gap + Block * 7 / 4, Y, Block, Block / 2};
        Y -= Gap + Block / 2;
        Menu->abButton = {2 * Gap + Block / 2, Y, 2 * Block, Block / 2};
        Menu->chButton = {Gap, Y, Block / 2, Block / 2};
        Y -= 0;
        
        u32 Remainder = Y - 2 * Gap;
        Menu->a = {Gap, Y - Remainder, Block * 3 / 2, Remainder};
        Menu->b = {3 * Gap + Block * 3 / 2, Y - Remainder, Block * 3 / 2, Remainder};
    }
    else
    {
        Menu->Size.Width = DisplaySize->Width;
        Gap = Menu->Size.Width / 320;
        Block = 8 * Gap;
        Menu->Block = Block;
        Menu->Gap = Gap;
        Menu->Size.Height = Block * 3 + Gap * 4;
        *PaintingRegion = {
            0, Menu->Size.Height, DisplaySize->Width, DisplaySize->Height - Menu->Size.Height
        };
        
        u32 X = 2 * Gap;
        Menu->New = {X, Gap, Block, Block};
        Menu->Open = {X, 2 * Gap + Block, Block, Block};
        Menu->Save = {X, 3 * Gap + 2 * Block, Block, Block};
        X += Block;
        Menu->Time = {X + Block / 4, 0, Block / 4, Menu->Size.Height};
        TimeBorder = {X, 0, Block * 3 / 4, Menu->Size.Height};
        X += Block * 3 / 4;
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
        X += Gap + Block;
        Menu->PenButtonsToggle = {X, Gap, Block / 2, Block / 2};
        Menu->TiltToggle = {X, 2 * Gap + Block / 2, Block / 2, Block / 2};
        Menu->LinePenToggle = {X, 3 * Gap + Block, Block / 2, Block / 2};
        Menu->BrushStyleSwitch = {X, 7 * Gap + Block * 2, Block / 2, Block / 2};
        X += Gap + Block / 2;
        Menu->ColorA = {X, 2 * Gap + Block / 2, Block, Block};
        Menu->ColorB = {X, 2 * Gap + Block * 3 / 2, Block, Block};
        Menu->ColorButtonA = {X, 2 * Gap, Block, Block / 2};
        Menu->ColorButtonB = {X, 2 * Gap + Block * 5 / 2, Block, Block / 2};
        X += 2 * Gap + Block;
        Pallet = {X, 2 * Gap, 3 * Block, 3 * Block};
        X += 2 * Gap + 2 * Block;
        Menu->Steps = 12;
        Menu->LSmooth = {X + Block, 2 * Gap + Block* 6 / 4, Menu->Steps * Block, Block / 4};
        X += Menu->Steps * Block;
        Menu->Alpha = {X, 2 * Gap + Block / 4, Block, Block};
        Menu->L = {X, 2 * Gap + Block * 7 / 4, Block, Block};
        Menu->Offset = {-(s32)Block, 0};
        X += Block;
        Menu->AlphaButton = {X, 2 * Gap + Block / 4, Block / 2, Block};
        Menu->LLimitToggle = {X, 2 * Gap + Block * 5 / 4, Block / 2, Block / 2};
        Menu->LButton = {X, 2 * Gap + Block * 7 / 4, Block / 2, Block};
        X += Gap + Block / 2;
        Menu->abButton = {X, 2 * Gap + Block / 2, Block / 2, 2 * Block};
        Menu->chButton = {X, Gap, Block / 2, Block / 2};
        X += Block / 2;
        
        u32 Remainder = Menu->Size.Width - X - 2 * Gap;
        Menu->a = {X, Gap, Remainder, Block * 3 / 2};
        Menu->b = {X, 3 * Gap + Block * 3 / 2, Remainder, Block * 3 / 2};
    }
    Menu->Origin = {(r32)(DisplaySize->Width / 2), (r32)(DisplaySize->Height / 2)};
    
    if(OpenGL->MenuFramebuffer.FramebufferHandle)
    {
        FreeFramebuffer(&OpenGL->MenuFramebuffer);
    }
    OpenGL->MenuFramebuffer = CreateFramebuffer(OpenGL, Menu->Size.Width, Menu->Size.Height, 0);
    
    v3 Lab = ConvertRGBToLab({0.9, 0.9, 0.9});
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGL->MenuFramebuffer.FramebufferHandle);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->ScreenFillVertexBuffer);
    glViewport(0, 0, Menu->Size.Width, Menu->Size.Height);
    glScissor(0, 0, Menu->Size.Width, Menu->Size.Height);
    glClearColor(Lab.x, Lab.y, Lab.z, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    DrawButtonNew(OpenGL, Menu->New);
    DrawButtonOpen(OpenGL, Menu->Open);
    DrawButtonSave(OpenGL, Menu->Save);
    DrawButtonRotateR(OpenGL, Menu->RotateR);
    DrawButtonRotateL(OpenGL, Menu->RotateL);
    DrawButtonOneToOne(OpenGL, Menu->OneToOne);
    DrawButtonReset(OpenGL, Menu->Reset);
    DrawButtonMinus(OpenGL, Menu->Minus);
    DrawButtonPlus(OpenGL, Menu->Plus);
    DrawButtonSmall(OpenGL, Menu->Small);
    DrawButtonBig(OpenGL, Menu->Big);
    DrawButtonMirror(OpenGL, Menu->Mirror);
    DrawPallet(Pallet);
    DrawTimerBorder(OpenGL, TimeBorder);
}

internal v2
MapTabletInput(DWORD TabletPointX, DWORD TabletPointY, window_mapping Map)
{
    v2 Result = {};
    Result.x = (r32)TabletPointX * Map.Scalar.x - Map.Offset.x;
    Result.y = (r32)TabletPointY * Map.Scalar.y - Map.Offset.y;
    return(Result);
}

internal void
ComputeTabletMapping(window_mapping *Map)
{
    // TODO(Zyonji): Should this be the lcSys coordinates instead of the monitor coordinates?
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
ToggleFullscreen(HWND Window, window_data *WindowData)
{
    if(WindowData->Fullscreen)
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
            // TODO(Zyonji): Find a way to prevent exclusive full screen mode.
            s32 ScreenWidth = MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left + 1;
            
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

BOOL CALLBACK
CycleStreamMonitor(HMONITOR Monitor, HDC DevideContext, LPRECT MonitorRect, LPARAM Data)
{
    local_persist LONG Left = 0;
    local_persist LONG Top = 0;
    local_persist LONG LastEquals = 1;
    
    if(MonitorRect->left == Left && MonitorRect->top == Top)
    {
        LastEquals = 1;
    }
    else if(LastEquals)
    {
        LastEquals = 0;
        Left = MonitorRect->left;
        Top = MonitorRect->top;
        
        SetWindowPos(OrcaState->StreamWindow, HWND_NOTOPMOST, Left, Top, 1920, 1080, SWP_FRAMECHANGED);
        return false;
    }
    return true;
    // TODO(Zyonji): It doesn't work if the last one is the current window position, got to press F12 again in that case.
}

internal void
CycleReferenceImage(HWND Window)
{
    if(*OrcaState->RefPath)
    {
        char SearchPath[MAX_PATH];
        char OldFileName[MAX_PATH];
        WIN32_FIND_DATA SearchResult;
        HANDLE FindHandle = INVALID_HANDLE_VALUE;
        
        char *At = SearchPath;
        char *From = OrcaState->RefPath;
        char *Slash = From;
        while(*Slash)
        {
            Slash++;
        }
        while(Slash >= From && *Slash != '/' && *Slash != '\\')
        {
            --Slash;
        }
        while(From <= Slash)
        {
            *At++ = *From++;
        }
        *At++ = '*';
        *At++ = 0;
        At = OldFileName;
        while(*From)
        {
            *At++ = *From++;
        }
        *At++ = 0;
        
        FindHandle = FindFirstFile(SearchPath, &SearchResult);
        if(FindHandle != INVALID_HANDLE_VALUE) 
        {
            b32 FileFound = false;
            char NewFileName[MAX_PATH];
            *NewFileName = 0;
            
            do
            {
                if(!(SearchResult.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                {
                    b32 Equal = true;
                    At = OldFileName;
                    From = SearchResult.cFileName;
                    while(*At || *From)
                    {
                        if(*At++ != *From++)
                        {
                            Equal = false;
                            break;
                        }
                    }
                    
                    if(*NewFileName == 0)
                    {
                        At = NewFileName;
                        From = SearchResult.cFileName;
                        while(*From)
                        {
                            *At++ = *From++;
                        }
                        *At++ = 0;
                    }
                    
                    if(FileFound)
                    {
                        At = NewFileName;
                        From = SearchResult.cFileName;
                        while(*From)
                        {
                            *At++ = *From++;
                        }
                        *At = 0;
                        break;
                    }
                    
                    if(Equal)
                    {
                        FileFound = true;
                    }
                }
            }
            while(FindNextFile(FindHandle, &SearchResult) != 0);
            
            At = Slash;
            From = NewFileName;
            At++;
            while(*From)
            {
                *At++ = *From++;
            }
            *At = 0;
            
            image_data Reference = LoadImageFile(OrcaState->RefPath, OrcaState->MaxId);
            
            if(Reference.Bitmap)
            {
                OrcaState->Canvas.ReferenceSize = {Reference.Width, Reference.Height};
                
                if(OrcaState->OpenGL.ReferenceFramebuffer.FramebufferHandle)
                {
                    FreeFramebuffer(&OrcaState->OpenGL.ReferenceFramebuffer);
                }
                OrcaState->OpenGL.ReferenceFramebuffer = CreateConvertedFramebuffer(&OrcaState->OpenGL, OrcaState->Canvas.ReferenceSize.x, OrcaState->Canvas.ReferenceSize.y, Reference.Memory);
                
                FreeImage_Unload(Reference.Bitmap);
            }
            
            InvalidateRect(Window, 0, true);
        }
        FindClose(FindHandle);
    }
}

PICK_COLOR(PickColor)
{
    return(PickColor(&OrcaState->OpenGL, P));
}
UPDATE_MENU(UpdateMenu)
{
    UpdateMenu(&OrcaState->OpenGL, Menu, Color, ColorMode);
}
RENDER_BRUSH_STROKE(RenderBrushStroke)
{
    RenderBrushStroke(&OrcaState->OpenGL, Canvas, OldPen, NewPen);
}
RENDER_LINE(RenderLine)
{
    RenderLine(&OrcaState->OpenGL, Canvas, OldPen, NextPen);
}

LRESULT CALLBACK
MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;
    
    if(Window != OrcaState->Map.Window)
    {
        if(Message == WM_DESTROY)
            OrcaState->StreamWindow = 0;
        return(DefWindowProc(Window, Message, WParam, LParam));
    }
    
    switch(Message)
    {
        case WM_DESTROY:
        case WM_CLOSE:
        {
            if(OfferToSaveHotCanvas())
            {
                PostQuitMessage(0);
            }
        } break;
        
        case WM_SIZE:
        {
            if(OrcaState->OpenGL.Initialized)
            {
                UpdateDisplayFrameData(&OrcaState->OpenGL, Window, &OrcaState->DisplaySize, &OrcaState->PaintingRegion, &OrcaState->Menu);
                OrcaState->UpdateMenu(&OrcaState->Menu, OrcaState->Pen.Color, OrcaState->Pen.Mode);
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
        
#if 0
        case WM_TIMER:
        {
            InvalidateRect(Window, 0, true);
        } break;
#endif
        
        case WM_PAINT:
        {
            if(OrcaState->OpenGL.Initialized)
            {
                LARGE_INTEGER Counter;
                QueryPerformanceCounter(&Counter);
                s64 CurrentTime = Counter.QuadPart;
                u32 TimeDifference = (u32)(1000 * (real32)(CurrentTime - OrcaState->LastFrameTime)
                                           / OrcaState->PerformanceFrequency);
                if(TimeDifference > 16)
                {
                    OrcaState->LastFrameTime = CurrentTime;
                    
                    u64 CurrentFileTime = GetFileTime();
                    u64 LastTime = OrcaState->Canvas.StartingTime;
                    u32 SecondsPassed = (u32)((CurrentFileTime - LastTime) / 10000000);
                    r32 TimerProgression = (((r32)(SecondsPassed - OrcaState->Canvas.SecondsIdle)) / 60.0f - ((r32)OrcaState->Image.Minutes)) / 30.0f;
                    
                    HDC WindowDC = GetDC(Window);
                    wglMakeCurrent(WindowDC, OrcaState->OpenGL.RenderingContext);
                    DisplayBuffer(&OrcaState->OpenGL, OrcaState->PaintingRegion, OrcaState->Pen, OrcaState->Canvas, OrcaState->Menu, TimerProgression);
                    ReleaseDC(Window, WindowDC);
                    if(OrcaState->StreamWindow)
                    {
                        u32 PenHistoryWindow = Minimum(1000, OrcaState->Replay.Count);
                        pen_target *PenHistory = OrcaState->Replay.Next - PenHistoryWindow - 1;
                        
                        HDC StreamDC = GetDC(OrcaState->StreamWindow);
                        wglMakeCurrent(StreamDC, OrcaState->OpenGL.RenderingContext);
                        DisplayStreamFrame(&OrcaState->OpenGL, OrcaState->Pen, PenHistory, PenHistoryWindow, PenHistoryWindow, OrcaState->Canvas);
                        ReleaseDC(OrcaState->StreamWindow, StreamDC);
                    }
                }
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
                
                case VK_F1:
                {
                    OrcaState->OpenGL.TESTReferenceOverlay = !(OrcaState->OpenGL.TESTReferenceOverlay);
                    InvalidateRect(Window, 0, true);
                } break;
                
                case VK_F3:
                {
                    RequestFileChoice(OrcaState->RefPath, sizeof(OrcaState->RefPath));
                    if(*OrcaState->RefPath)
                    {
                        image_data Reference = LoadImageFile(OrcaState->RefPath, OrcaState->MaxId);
                        
                        if(Reference.Bitmap)
                        {
                            OrcaState->Canvas.ReferenceSize = {Reference.Width, Reference.Height};
                            
                            if(OrcaState->OpenGL.ReferenceFramebuffer.FramebufferHandle)
                            {
                                FreeFramebuffer(&OrcaState->OpenGL.ReferenceFramebuffer);
                            }
                            OrcaState->OpenGL.ReferenceFramebuffer = CreateConvertedFramebuffer(&OrcaState->OpenGL, OrcaState->Canvas.ReferenceSize.x, OrcaState->Canvas.ReferenceSize.y, Reference.Memory);
                            
                            FreeImage_Unload(Reference.Bitmap);
                        }
                        
                        InvalidateRect(Window, 0, true);
                    }
                } break;
                
                case VK_F4:
                {
                    CycleReferenceImage(Window);
                } break;
                
                case VK_F5:
                {
                    r32 Scale = 1;
                    if(OrcaState->Image.Width > OrcaState->Image.Height)
                    {
                        Scale = 3600 / (r32)OrcaState->Image.Width;
                        OrcaState->Image.Height = (u32)(Scale * OrcaState->Image.Height);
                        OrcaState->Image.Width = 3600;
                    }
                    else
                    {
                        Scale = 3600 / (r32)OrcaState->Image.Height;
                        OrcaState->Image.Width = (u32)(Scale * OrcaState->Image.Width);
                        OrcaState->Image.Height = 3600;
                    }
                    
                    pen_target *Replay = (pen_target *)OrcaState->Replay.Buffer;
                    while(Replay < OrcaState->Replay.Next)
                    {
                        Replay->P *= Scale;
                        Replay->Width *= Scale;
                        Replay++;
                    }
                    
                    b32 Alive = true;
                    AnimateReplay(Window, &Alive, OrcaState->SleepIsGranular, 61);
                } break;
                
                case VK_F6:
                {
                    RequestEmptyFile(OrcaState->MaxId, OrcaState->PaintingRegion, 200, 300);
                    InvalidateRect(Window, 0, true);
                } break;
                
                case VK_F7:
                {
                    char FileName[MAX_PATH];
                    RequestFileChoice(FileName, sizeof(FileName));
                    if(*FileName)
                    {
                        image_data ImageReference = LoadImageFile(FileName, OrcaState->MaxId);
                        image_data ImageData = {};
                        if(ImageReference.Width > ImageReference.Height)
                        {
                            ImageData.Width = 3600;
                            ImageData.Height = (u32)(3600.0f * (r32)ImageReference.Height / (r32)ImageReference.Width);
                        }
                        else
                        {
                            ImageData.Height = 3600;
                            ImageData.Width = (u32)(3600.0f * (r32)ImageReference.Width / (r32)ImageReference.Height);
                        }
                        
                        ClearReplay(&OrcaState->Replay);
                        
                        ChangeCanvas(&OrcaState->OpenGL, ImageData, &OrcaState->Canvas, OrcaState->PaintingRegion);
                        FreeBitmap(&ImageData);
                        OrcaState->Image = ImageData;
                        OrcaState->Image.Id = OrcaState->MaxId;
                        
                        InvalidateRect(Window, 0, true);
                    }
                } break;
                
                case VK_F8:
                {
                    GlobalReplayModeChange = true;
                } break;
                case VK_F11:
                {
                    ToggleFullscreen(Window, &OrcaState->WindowData);
                } break;
                case VK_F12:
                {
                    if(!OrcaState->StreamWindow)
                    {
                        OrcaState->StreamWindow = CreateWindowExA(
                            0,
                            "OrcaHexClass",
                            "OrcaHexStream",
                            WS_VISIBLE | WS_POPUP,
                            0,
                            0,
                            1920,
                            1080,
                            0,
                            0,
                            0,
                            0);
                        SetPixelFormat(GetDC(OrcaState->StreamWindow));
                    }
                    else 
                    {
                        EnumDisplayMonitors(0, 0, CycleStreamMonitor, 0);
                    }
                    InvalidateRect(Window, 0, true);
                } break;
            }
        } break;
        
        case WT_PACKET:
        {
            PACKET Packet;
            if(gpWTPacket((HCTX)LParam, (UINT)WParam, &Packet)) 
            {
                v2 Point = MapTabletInput(Packet.pkX, Packet.pkY, OrcaState->Map);
                r32 Width = OrcaState->Pen.NextWidth;
                r32 Radians = (Pi32 / 1800.0f) * (r32)Packet.pkOrientation.orAzimuth;
                r32 RightOfMenu = Point.x + OrcaState->Menu.Origin.x - OrcaState->Menu.Size.Width;
                r32 TopOfMenu   = Point.y + OrcaState->Menu.Origin.y - OrcaState->Menu.Size.Height;
                r32 Pressure = 0;
                // NOTE(Zyonji): Only when pressure was set last time or pen is outside the menu.
                if(OrcaState->Pen.Pressure > 0 || RightOfMenu >= 0 || TopOfMenu >= 0)
                {
                    Pressure = (r32)Packet.pkNormalPressure / OrcaState->Map.PressureMax;
                }
                v4 Color = OrcaState->Pen.Color;
                u32 Mode = OrcaState->Pen.Mode;
                
                u32 Seconds = SecondsPassed(&OrcaState->LastPenTime);
                if(Seconds > 5)
                {
                    OrcaState->Canvas.SecondsIdle += Seconds;
                }
#if 0
                if(Seconds != 0)
                {
                    char Buffer[256];
                    wsprintf(Buffer, "Time: %u\n", Seconds);
                    OutputDebugStringA(Buffer);
                }
#endif
                // TODO(Zyonji): Considder making it possible to change the width. Maybe with Packet.pkOrientation.orAltitude
                pen_target PenTarget = {};
                PenTarget.P          = MapFrameToCanvas(Point, OrcaState->DisplaySize, OrcaState->PaintingRegion, OrcaState->Canvas);
                PenTarget.Pressure   = Pressure;
                PenTarget.Radians    = MapFrameToCanvas(Radians, OrcaState->Canvas);
                PenTarget.Width      = Width;
                PenTarget.Color      = Color;
                PenTarget.Mode       = Mode;
                
                u32 Buttons = Packet.pkButtons;
                b32 IsPenClose = (Packet.pkZ < 2 * OrcaState->Map.AxisMax.z / 3);
                b32 FreshClick = (!OrcaState->Pen.IsDown && Packet.pkNormalPressure);
                OrcaState->Pen.IsDown = Packet.pkNormalPressure;
                
                if(ProcessBrushMove(OrcaState, PenTarget, Point, Buttons, IsPenClose, FreshClick, true))
                {
                    InvalidateRect(Window, 0, true);
                }
                OrcaState->Pen.Buttons = Buttons;
                OrcaState->Pen.Point = Point;
                OrcaState->Pen.PointRadians = Radians;
            }
        } break;
        
        case WT_PACKETEXT:
        {
            PACKETEXT Packet = {};
            if(gpWTPacket((HCTX)LParam, (UINT)WParam, &Packet))
            {
                // TODO(Zyonji): Figure out how to detect the 0 position package when lifting the finger.
                if(Packet.pkTouchRing.nPosition)
                {
                    SetRelativeBrushSize(&OrcaState->Pen, (r32)Packet.pkTouchRing.nPosition / OrcaState->Map.TouchRingMax);
                }
            }
        }
        break;
        
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
    UINT DesiredSchedulerMS = 1;
    b32 SleepIsGranular = (timeBeginPeriod(DesiredSchedulerMS) == TIMERR_NOERROR);
    GlobalReplayModeChange = false;
    
    WNDCLASS WindowClass = {};
    
    WindowClass.lpfnWndProc = MainWindowCallback;
    WindowClass.hInstance = Instance;
    WindowClass.hCursor = LoadCursor(0, IDC_ARROW);
    //WindowClass.hIcon;
    WindowClass.lpszClassName = "OrcaHexClass";
    
    if(RegisterClassA(&WindowClass))
    {
        memory_index MemorySize = sizeof(win32_orca_state);
        umm GeneralMemory = (umm)VirtualAlloc(0, MemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
        if(GeneralMemory)
        {
            OrcaState = (win32_orca_state *)GeneralMemory;
            OrcaState->PickColor = CPUPickColor;
            OrcaState->UpdateMenu = CPUUpdateMenu;
            OrcaState->RenderBrushStroke = CPURenderBrushStroke;
            OrcaState->RenderLine = CPURenderLine;
            
            OrcaState->SleepIsGranular = SleepIsGranular;
            
            InitReplay(&OrcaState->Replay);
            GenerateIniPath(OrcaState->IniPath, sizeof(OrcaState->IniPath));
            DWORD LastWindowStyle = LoadInitialState(OrcaState, OrcaState->IniPath);
            
            HWND Window = CreateWindowExA(
                0,
                WindowClass.lpszClassName,
                "OrcaHex",
                WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                OrcaState->WindowData.Pos.x,
                OrcaState->WindowData.Pos.y,
                OrcaState->WindowData.Size.Width,
                OrcaState->WindowData.Size.Height,
                0,
                0,
                Instance,
                0);
            
            if(Window)
            {
                if(InitOpenGL(&OrcaState->OpenGL, GetDC(Window)))
                {
                    OrcaState->TabletContext = InitWinTab(Window, &OrcaState->Map);
                    if(OrcaState->TabletContext)
                    {
                        OrcaState->PickColor = PickColor;
                        OrcaState->UpdateMenu = UpdateMenu;
                        OrcaState->RenderBrushStroke = RenderBrushStroke;
                        OrcaState->RenderLine = RenderLine;
                        
                        OrcaState->Map.Window = Window;
                        ComputeTabletMapping(&OrcaState->Map);
                        UpdateDisplayFrameData(&OrcaState->OpenGL, Window, &OrcaState->DisplaySize, &OrcaState->PaintingRegion, &OrcaState->Menu);
                        UpdateMenu(&OrcaState->Menu, OrcaState->Pen.Color, OrcaState->Pen.Mode);
                        
                        LARGE_INTEGER PerfCountFrequencyResult;
                        QueryPerformanceFrequency(&PerfCountFrequencyResult);
                        OrcaState->PerformanceFrequency = (r32)PerfCountFrequencyResult.QuadPart;
                        
                        if(LastWindowStyle & WS_POPUP)
                        {
                            ToggleFullscreen(Window, &OrcaState->WindowData);
                        }
                        
                        // TODO(Zyonji): This has some awkward load times at the start.  Look if I want to change that.
                        if(CommandLine && *CommandLine != '\0')
                        {
                            LoadCanvas(&OrcaState->OpenGL, &OrcaState->Image, &OrcaState->Canvas, CommandLine, OrcaState->PaintingRegion, OrcaState->MaxId);
                        }
                        
                        // NOTE(Zyonji): Initialize the LastPenTime.
                        SecondsPassed(&OrcaState->LastPenTime);
                        
#if 0
                        SetTimer(Window, 1, 60000, (TIMERPROC) 0);
                        OrcaState->CreateNewFile(OrcaState->MaxId, OrcaState->PaintingRegion, 2400, 3600);
#endif
                        
                        // NOTE(Zyonji): The main Message loop.
                        b32 Alive = true;
                        for(;;)
                        {
                            MSG Message;
                            BOOL MessageResult = GetMessageA(&Message, 0, 0, 0);
                            if(MessageResult > 0 && Alive)
                            {
                                TranslateMessage(&Message);
                                DispatchMessageA(&Message);
                                
                                if(GlobalReplayModeChange)
                                {
                                    AnimateReplay(Window, &Alive, SleepIsGranular, (60 * 60 * 15) + 1);
                                }
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