struct common_vertex
{
    v4 P;
    v2 UV;
    v4 Color;
};
struct render_program_base
{
    GLuint Handle;
    
    GLuint PID;
    GLuint UVID;
    GLuint ColorID;
    GLuint TransformID;
};
struct brush_mode_program
{
    render_program_base Common;
    
    GLuint BrushModeID;
};
struct display_program
{
    render_program_base Common;
    
    GLuint CursorID;
    GLuint HalfWidthID;
    GLuint NormalID;
};
struct frame_buffer
{
    GLuint FramebufferHandle;
    GLuint ColorHandle;
};
struct open_gl
{
    b32 Initialized;
    HGLRC RenderingContext;
    
    render_program_base VisualTransparency;
    render_program_base TransferPixelsProgram;
    render_program_base StaticTransferPixelsProgram;
    render_program_base BasicDrawProgram;
    render_program_base LabDrawProgram;
    brush_mode_program  BrushModeProgram;
    display_program     DisplayProgram;
    
    frame_buffer DisplayFramebuffer;
    frame_buffer CanvasFramebuffer;
    frame_buffer SwapFramebuffer;
    frame_buffer MenuFramebuffer;
    
    GLuint ScreenFillVertexBuffer;
    GLuint VertexBuffer;
};

internal GLuint
OpenGLCreateProgram(char *VertexCode, char *FragmentCode,
                    render_program_base *Result)
{
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VertexShaderID, 1, &VertexCode, 0);
    glCompileShader(VertexShaderID);
    
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FragmentShaderID, 1, &FragmentCode, 0);
    glCompileShader(FragmentShaderID);
    
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);
    
    // TODO(Zyonji): remove this once the shaders are done.
#if 1
    glValidateProgram(ProgramID);
    GLint Linked = false;
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Linked);
    if(!Linked)
    {
        GLsizei Ignored;
        char VertexErrors[4096];
        char FragmentErrors[4096];
        char ProgramErrors[4096];
        glGetShaderInfoLog(VertexShaderID, sizeof(VertexErrors), &Ignored, VertexErrors);
        glGetShaderInfoLog(FragmentShaderID, sizeof(FragmentErrors), &Ignored, FragmentErrors);
        glGetProgramInfoLog(ProgramID, sizeof(ProgramErrors), &Ignored, ProgramErrors);
        
        LogError(ProgramErrors, "OpenGL");
        LogError(VertexErrors, "OpenGL");
        LogError(FragmentErrors, "OpenGL");
        return(0);
        //Assert(!"Shader validation failed");
    }
#endif
    
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
    
    Result->Handle = ProgramID;
    Result->PID = glGetAttribLocation(ProgramID, "VertP");
    Result->UVID = glGetAttribLocation(ProgramID, "VertUV");
    Result->ColorID = glGetAttribLocation(ProgramID, "VertColor");
    Result->TransformID = glGetUniformLocation(ProgramID, "Transform");
    
    return(ProgramID);
}

internal GLuint
CompileVisualTransparency(open_gl *OpenGL, render_program_base *Result)
{
    char *VertexCode = R"glsl(
#version 330
// Vertex code
uniform mat4 Transform;

in vec4 VertP;
in vec4 VertColor;

smooth out vec4 Color;

void main(void)
{
gl_Position = VertP;
Color = VertColor;
}
)glsl";
    
    char *FragmentCode = R"glsl(
#version 330
// Fragment code

smooth in vec4 Color;
in vec4 gl_FragCoord;

out vec4 FragmentColor;

void main(void)
{
FragmentColor = Color;
if(mod(floor(gl_FragCoord.x / 4) + floor(gl_FragCoord.y / 4), 2) > 0.5)
{
FragmentColor = vec4(1 - FragmentColor.x, 1 - FragmentColor.y, 1 - FragmentColor.z, FragmentColor.w);
}
}
)glsl";
    
    GLuint Program = OpenGLCreateProgram(VertexCode, FragmentCode, Result);
    
    return(Program);
}

internal GLuint
CompileTransferPixels(open_gl *OpenGL, render_program_base *Result)
{
    char *VertexCode = R"glsl(
#version 330
// Vertex code
uniform mat4 Transform;

in vec4 VertP;
in vec2 VertUV;

smooth out vec2 FragUV;

void main(void)
{
vec4 Position = Transform * VertP;
gl_Position = Position;
FragUV = VertUV;
}
)glsl";
    
    char *FragmentCode = R"glsl(
#version 330
// Fragment code
uniform sampler2D Image;

smooth in vec2 FragUV;

out vec4 FragmentColor;

void main(void)
{
FragmentColor = texture(Image, FragUV);
}
)glsl";
    
    GLuint Program = OpenGLCreateProgram(VertexCode, FragmentCode, Result);
    
    return(Program);
}
internal GLuint
CompileStaticTransferPixels(open_gl *OpenGL, render_program_base *Result)
{
    char *VertexCode = R"glsl(
#version 330
// Vertex code
uniform mat4 Transform;

in vec4 VertP;

smooth out vec2 FragUV;

void main(void)
{
vec4 Position = Transform * VertP;
gl_Position = Position;
FragUV = (0.5 * Position.xy) + vec2(0.5);
}
)glsl";
    
    char *FragmentCode = R"glsl(
#version 330
// Fragment code
uniform sampler2D Image;

smooth in vec2 FragUV;

out vec4 FragmentColor;

void main(void)
{
FragmentColor = texture(Image, FragUV);
}
)glsl";
    
    GLuint Program = OpenGLCreateProgram(VertexCode, FragmentCode, Result);
    
    return(Program);
}


internal GLuint
CompileBasicDraw(open_gl *OpenGL, render_program_base *Result)
{
    char *VertexCode = R"glsl(
#version 330
// Vertex code
in vec4 VertP;
in vec4 VertColor;

smooth out vec4 Color;

void main(void)
{
gl_Position = VertP;
Color = VertColor;
}
)glsl";
    
    char *FragmentCode = R"glsl(
#version 330
// Fragment code

smooth in vec4 Color;

out vec4 FragmentColor;

void main(void)
{
FragmentColor = Color;
     }
)glsl";
    
    GLuint Program = OpenGLCreateProgram(VertexCode, FragmentCode, Result);
    
    return(Program);
}

internal GLuint
CompileLabDraw(open_gl *OpenGL, render_program_base *Result)
{
    char *VertexCode = R"glsl(
#version 330
// Vertex code
in vec4 VertP;
in vec4 VertColor;

smooth out vec4 Color;

void main(void)
{
gl_Position = VertP;
Color = VertColor;
}
)glsl";
    
    char *FragmentCode = R"glsl(
#version 330
// Fragment code

smooth in vec4 Color;

out vec4 FragmentColor;

void main(void)
{
mat3 ToRGB = mat3(78.0 / 29.0, -2589.0 / 2533.0, 3.0 / 49.0, -37.0 / 29.0, 5011.0 / 2533.0, -11.0 / 49.0, -12.0 / 29.0, 111.0 / 2533.0, 57.0 / 49.0);

vec3 Cubic = vec3(Color.y + Color.x, Color.x, Color.x - Color.z);
    vec3 XYZ = vec3(pow(Cubic.x, 2.4), pow(Cubic.y, 2.4), pow(Cubic.z, 2.4));
    vec3 Linear = ToRGB * XYZ;
    vec3 RGB = vec3(pow(Linear.x, 1.0 / 2.2), pow(Linear.y, 1.0 / 2.2), pow(Linear.z, 1.0 / 2.2));
    
    float Cmax = max(max(RGB.x, RGB.y), RGB.z);
    float Cmin = min(min(RGB.x, RGB.y), RGB.z);
    if(Cmax > 1.0 || Cmin < 0.0)
    {
         RGB = vec3(0.9);
    }
    
     FragmentColor = vec4(RGB, 1);
     }
)glsl";
    
    GLuint Program = OpenGLCreateProgram(VertexCode, FragmentCode, Result);
    
    return(Program);
}

// TODO(Zyonji): Should I change the code to reuse the common parts of the BrushModes?
// TODO(Zyonji): Schould there be an exception for Alpha 1?
internal GLuint
CompileBrushMode(open_gl *OpenGL, brush_mode_program *Result)
{
    char *VertexCode = R"glsl(
#version 330
// Vertex code
uniform mat4 Transform;

in vec4 VertP;
in vec2 VertUV;
in vec4 VertColor;

smooth out vec2 FragXY;
smooth out vec2 FragUV;
smooth out vec4 Color;

void main(void)
{
vec4 Position = Transform * VertP;
gl_Position = Position;
FragXY = (0.5 * Position.xy) + vec2(0.5);
FragUV = VertUV;
Color = VertColor;
}
)glsl";
    
    char *FragmentCode = R"glsl(
#version 330
// Fragment code
uniform vec4 BrushMode;
uniform sampler2D Image;

smooth in vec2 FragXY;
smooth in vec2 FragUV;
smooth in vec4 Color;

out vec4 FragmentColor;

void main(void)
{
float Alpha = Color.w * clamp((1.0 - abs(FragUV.x)) * FragUV.y, 0.0, 1.0);

mat3 ToXYZ = mat3(20416.0 / 41085.0, 319.0 / 1245.0, 957.0 / 41085.0, 2533.0 / 7470.0, 2533.0 / 3735.0, 2533.0 / 22410.0, 245.0 / 1494.0, 49.0 / 747.0, 3871.0 / 4482.0);
mat3 ToRGB = mat3(78.0 / 29.0, -2589.0 / 2533.0, 3.0 / 49.0, -37.0 / 29.0, 5011.0 / 2533.0, -11.0 / 49.0, -12.0 / 29.0, 111.0 / 2533.0, 57.0 / 49.0);

vec3 Linear0 = vec3(pow(Color.x, 2.2), pow(Color.y, 2.2), pow(Color.z, 2.2));
    vec3 XYZ0 = ToXYZ * Linear0;
    vec3 Cubic0 = vec3(pow(XYZ0.x, 1.0 / 2.4), pow(XYZ0.y, 1.0 / 2.4), pow(XYZ0.z, 1.0 / 2.4));
     vec3 Lab0 = vec3(Cubic0.y, Cubic0.x - Cubic0.y, Cubic0.y - Cubic0.z);
     
     vec4 Color1 = texture(Image, FragXY);
    vec3 Linear1 = vec3(pow(Color1.x, 2.2), pow(Color1.y, 2.2), pow(Color1.z, 2.2));
    vec3 XYZ1 = ToXYZ * Linear1;
    vec3 Cubic1 = vec3(pow(XYZ1.x, 1.0 / 2.4), pow(XYZ1.y, 1.0 / 2.4), pow(XYZ1.z, 1.0 / 2.4));
     vec3 Lab1 = vec3(Cubic1.y, Cubic1.x - Cubic1.y, Cubic1.y - Cubic1.z);
     
     vec3 BlendMode = vec3(Alpha * BrushMode.x, Alpha * BrushMode.y, Alpha * BrushMode.y);
     vec3 Lab = mix(Lab1, Lab0, BlendMode);
     
    vec3 Cubic = vec3(Lab.y + Lab.x, Lab.x, Lab.x - Lab.z);
    vec3 XYZ = vec3(pow(Cubic.x, 2.4), pow(Cubic.y, 2.4), pow(Cubic.z, 2.4));
    vec3 Linear = ToRGB * XYZ;
    
    vec3 Grey = ToRGB * vec3(pow(Cubic.y, 2.4));
    vec3 Vector = Grey - Linear;
    vec3 ScaleA = vec3(Vector.x / Grey.x, Vector.y / Grey.y, Vector.z / Grey.z);
    vec3 ScaleB = vec3(-Vector.x / (1 - Grey.x), -Vector.y / (1 - Grey.y), -Vector.z / (1 - Grey.z));
     float Distance = max(max(max(ScaleA.x, ScaleB.x), max(ScaleA.y, ScaleB.y)), max(ScaleA.z, ScaleB.z));
    if(Distance > 1.0)
    {
         Linear = Grey - (Vector / Distance);
    }
    
     FragmentColor = vec4(pow(Linear.x, 1.0 / 2.2), pow(Linear.y, 1.0 / 2.2), pow(Linear.z, 1.0 / 2.2), 1);
     }
)glsl";
    
    GLuint Program = OpenGLCreateProgram(VertexCode, FragmentCode, &Result->Common);
    
    Result->BrushModeID = glGetUniformLocation(Program, "BrushMode");
    
    return(Program);
}

internal GLuint
CompileDisplay(open_gl *OpenGL, display_program *Result)
{
    char *VertexCode = R"glsl(
#version 330
// Vertex code
uniform mat4 Transform;

in vec4 VertP;
in vec2 VertUV;

smooth out vec2 FragUV;

void main(void)
{
gl_Position = Transform * VertP;
FragUV = VertUV;
}
)glsl";
    
    char *FragmentCode = R"glsl(
#version 330
// Fragment code
precision highp float;

uniform sampler2D Image;
uniform ivec2 Cursor;
uniform float HalfWidth;
uniform vec2 Normal;

smooth in vec2 FragUV;

out vec4 FragmentColor;

void main(void)
{
FragmentColor = vec4(texture(Image, FragUV).xyz, 1.0);

vec2 RelPos = gl_FragCoord.xy - Cursor;
float U = dot(RelPos, Normal);
vec2 Normal2 = vec2(-Normal.y, Normal.x);
float V = dot(RelPos, Normal2);

vec4 CursorColor = vec4(1, 1, 1, 2) - step(0.5, FragmentColor);
float Strength = max((1.5 - abs(V)) * abs(U) / HalfWidth, (1.5 - abs(V)) * (1.5 - abs(U)));
if(Strength > 1) Strength = 1;
if((floor(gl_FragCoord.xy) == Cursor) || (abs(U) <= max(HalfWidth, 1.5) && abs(V) <= 1.5)) FragmentColor = mix(FragmentColor, CursorColor, Strength);
}
)glsl";
    
    GLuint Program = OpenGLCreateProgram(VertexCode, FragmentCode, &Result->Common);
    
    Result->CursorID = glGetUniformLocation(Program, "Cursor");
    Result->HalfWidthID = glGetUniformLocation(Program, "HalfWidth");
    Result->NormalID = glGetUniformLocation(Program, "Normal");
    
    return(Program);
}

internal b32
IsValidArray(GLuint Index)
{
    b32 Result = (Index != -1);
    return(Result);
}
internal void
OpenGLProgramBegin(render_program_base  *Program)
{
    glUseProgram(Program->Handle);
    
    GLuint PArray = Program->PID;
    GLuint UVArray = Program->UVID;
    GLuint CArray = Program->ColorID;
    
    if(IsValidArray(PArray))
    {
        glEnableVertexAttribArray(PArray);
        glVertexAttribPointer(PArray, 4, GL_FLOAT, false, sizeof(common_vertex), (void *)OffsetOf(common_vertex, P));
    }
    if(IsValidArray(UVArray))
    {
        glEnableVertexAttribArray(UVArray);
        glVertexAttribPointer(UVArray, 2, GL_FLOAT, false, sizeof(common_vertex), (void *)OffsetOf(common_vertex, UV));
    }
    if(IsValidArray(CArray))
    {
        glEnableVertexAttribArray(CArray);
        glVertexAttribPointer(CArray, 4, GL_FLOAT, false, sizeof(common_vertex), (void *)OffsetOf(common_vertex, Color));
    }
}

internal void
OpenGLProgramEnd(render_program_base *Program)
{
    glUseProgram(0);
    
    GLuint PArray = Program->PID;
    GLuint UVArray = Program->UVID;
    GLuint CArray = Program->ColorID;
    
    if(IsValidArray(PArray))
    {
        glDisableVertexAttribArray(PArray);
    }
    if(IsValidArray(UVArray))
    {
        glDisableVertexAttribArray(UVArray);
    }
    if(IsValidArray(CArray))
    {
        glDisableVertexAttribArray(CArray);
    }
}

internal void
OpenGLProgramBegin(render_program_base *Program, m4x4 Transform)
{
    OpenGLProgramBegin(Program);
    
    glUniformMatrix4fv(Program->TransformID, 1, GL_TRUE, Transform.E[0]);
}

internal void
OpenGLProgramBegin(brush_mode_program *Program, m4x4 Transform, v4 BrushMode)
{
    OpenGLProgramBegin(&Program->Common);
    
    glUniformMatrix4fv(Program->Common.TransformID, 1, GL_TRUE, Transform.E[0]);
    glUniform4fv(Program->BrushModeID, 1, BrushMode.E);
}
internal void
OpenGLProgramEnd(brush_mode_program *Program)
{
    OpenGLProgramEnd(&Program->Common);
}

internal void
OpenGLProgramBegin(display_program *Program, m4x4 Transform, v2s Cursor, r32 HalfWidth, v2 Normal)
{
    OpenGLProgramBegin(&Program->Common);
    
    glUniformMatrix4fv(Program->Common.TransformID, 1, GL_TRUE, Transform.E[0]);
    glUniform2iv(Program->CursorID, 1, Cursor.E);
    glUniform1f(Program->HalfWidthID, HalfWidth);
    glUniform2fv(Program->NormalID, 1, Normal.E);
}
internal void
OpenGLProgramEnd(display_program *Program)
{
    OpenGLProgramEnd(&Program->Common);
}

internal b32
OpenGLInitPrograms(open_gl *OpenGL)
{
    b32 Return = true;
    
    glEnable(GL_SCISSOR_TEST);
    glBlendFunc(GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR);
    GLuint DummyVertexArray;
    glGenVertexArrays(1, &DummyVertexArray);
    glBindVertexArray(DummyVertexArray);
    
    glGenBuffers(1, &OpenGL->ScreenFillVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->ScreenFillVertexBuffer);
    common_vertex Vertices[] =
    {
        // NOTE(Zyonji): The colors are for the transparency pattern.
        {{-1,  1, 0, 1}, {0, 1}, {0.3, 0.3, 0.3, 1.0}},
        {{-1, -1, 0, 1}, {0, 0}, {0.3, 0.3, 0.3, 1.0}},
        {{ 1,  1, 0, 1}, {1, 1}, {0.3, 0.3, 0.3, 1.0}},
        {{ 1, -1, 0, 1}, {1, 0}, {0.3, 0.3, 0.3, 1.0}},
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    
    glGenBuffers(1, &OpenGL->VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    // NOTE(Zyonji): Max number of Vertex in Brush
    glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(common_vertex), NULL, GL_DYNAMIC_DRAW);
    
    if(!CompileVisualTransparency(OpenGL, &OpenGL->VisualTransparency))
    {
        LogError("Unable to compile the transparency visualizer.", "OpenGL");
        Return = false;
    }
    if(!CompileTransferPixels(OpenGL,     &OpenGL->TransferPixelsProgram))
    {
        LogError("Unable to compile the pixel transfer program.", "OpenGL");
        Return = false;
    }
    if(!CompileStaticTransferPixels(OpenGL,     &OpenGL->StaticTransferPixelsProgram))
    {
        LogError("Unable to compile the pixel transfer program.", "OpenGL");
        Return = false;
    }
    if(!CompileBasicDraw(OpenGL,          &OpenGL->BasicDrawProgram))
    {
        LogError("Unable to compile the basic draw program.", "OpenGL");
        Return = false;
    }
    if(!CompileLabDraw(OpenGL,            &OpenGL->LabDrawProgram))
    {
        LogError("Unable to compile the Lab draw program.", "OpenGL");
        Return = false;
    }
    if(!CompileBrushMode(OpenGL,          &OpenGL->BrushModeProgram))
    {
        LogError("Unable to compile the brush stroke renderer.", "OpenGL");
        Return = false;
    }
    if(!CompileDisplay(OpenGL,            &OpenGL->DisplayProgram))
    {
        LogError("Unable to compile the buffer display program.", "OpenGL");
        Return = false;
    }
    
    return(Return);
}

internal GLuint
FramebufferTextureImage(open_gl *OpenGL, GLuint Slot, u32 Width, u32 Height, GLuint Format, void *Memory)
{
    GLuint Result = 0;
    
    glGenTextures(1, &Result);
    glBindTexture(Slot, Result);
    glTexImage2D(Slot, 0, Format, Width, Height, 0, GL_BGRA, GL_UNSIGNED_BYTE, Memory);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    return(Result);
}

internal frame_buffer
CreateFramebuffer(open_gl *OpenGL, u32 Width, u32 Height, void *Memory)
{
    frame_buffer Result = {};
    
    glGenFramebuffers(1, &Result.FramebufferHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, Result.FramebufferHandle);
    
    Result.ColorHandle  = FramebufferTextureImage(OpenGL, GL_TEXTURE_2D, Width, Height, GL_RGBA8, Memory);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           Result.ColorHandle, 0);
    
    //GLenum FrameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    //Assert(FrameBufferStatus == GL_FRAMEBUFFER_COMPLETE);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    return(Result);
}

internal void
FreeFramebuffer(frame_buffer *Framebuffer)
{
    if(Framebuffer->FramebufferHandle)
    {
        glDeleteFramebuffers(1, &Framebuffer->FramebufferHandle);
        Framebuffer->FramebufferHandle = 0;
    }
    if(Framebuffer->ColorHandle)
    {
        glDeleteTextures(1, &Framebuffer->ColorHandle);
        Framebuffer->ColorHandle = 0;
    }
}

internal void
ConvertImageToBuffer(open_gl *OpenGL, void *Memory, u32 Width, u32 Height, frame_buffer *TargetBuffer, frame_buffer *SwapBuffer)
{
    if(TargetBuffer)
    {
        FreeFramebuffer(TargetBuffer);
    }
    if(SwapBuffer)
    {
        FreeFramebuffer(SwapBuffer);
    }
    *TargetBuffer = CreateFramebuffer(OpenGL, Width, Height, Memory);
    *SwapBuffer = CreateFramebuffer(OpenGL, Width, Height, 0);
    
    if(!Memory)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, TargetBuffer->FramebufferHandle);
        glViewport(0, 0, Width, Height);
        glScissor(0, 0, Width, Height);
        glBindBuffer(GL_ARRAY_BUFFER, OpenGL->ScreenFillVertexBuffer);
        // NOTE(Zyonji): The color for a new empty canvas.
        glClearColor(0.7, 0.7, 0.7, 1);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}

internal void
UpdateMenu(open_gl *OpenGL, menu_state *Menu, v4 Color, u32 ColorMode)
{
    glEnable(GL_BLEND);
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGL->MenuFramebuffer.FramebufferHandle);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->ScreenFillVertexBuffer);
    glViewport(0, 0, Menu->Size.Width, Menu->Size.Height);
    
    r32 PressureMode = 0;
    if(ColorMode & COLOR_MODE_PRESSURE)
    {
        PressureMode = 1;
    }
    glClearColor(1 - PressureMode, 1 - PressureMode, 1 - PressureMode, 1);
    glScissor(Menu->ColorA.x, Menu->ColorA.y, Menu->ColorA.Height, Menu->ColorA.Width);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(PressureMode, PressureMode, PressureMode, 1);
    glScissor(Menu->ColorB.x, Menu->ColorB.y, Menu->ColorB.Height, Menu->ColorB.Width);
    glClear(GL_COLOR_BUFFER_BIT);
    
    r32 AlphaMode = 0;
    if(ColorMode & COLOR_MODE_ALPHA)
    {
        AlphaMode = 1;
    }
    glClearColor(1 - AlphaMode, 1 - AlphaMode, 1 - AlphaMode, 1);
    glScissor(Menu->AlphaButton.x, Menu->AlphaButton.y, Menu->AlphaButton.Height, Menu->AlphaButton.Width);
    glClear(GL_COLOR_BUFFER_BIT);
    r32 LuminaceMode = 0;
    if(ColorMode & COLOR_MODE_LUMINANCE)
    {
        LuminaceMode = 1;
    }
    glClearColor(1 - LuminaceMode, 1 - LuminaceMode, 1 - LuminaceMode, 1);
    glScissor(Menu->LButton.x, Menu->LButton.y, Menu->LButton.Height, Menu->LButton.Width);
    glClear(GL_COLOR_BUFFER_BIT);
    r32 ChromaMode = 0;
    if(ColorMode & COLOR_MODE_CHROMA)
    {
        ChromaMode = 1;
    }
    glClearColor(1 - ChromaMode, 1 - ChromaMode, 1 - ChromaMode, 1);
    glScissor(Menu->abButton.x, Menu->abButton.y, Menu->abButton.Height, Menu->abButton.Width);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glClearColor(Color.x, Color.y, Color.z, 1);
    glScissor(Menu->ColorB.x + Menu->Gap / 2, Menu->ColorB.y + Menu->Gap / 2, Menu->ColorB.Height - Menu->Gap, Menu->ColorB.Width - Menu->Gap);
    glClear(GL_COLOR_BUFFER_BIT);
    glScissor(Menu->ColorA.x + Menu->Gap / 2, Menu->ColorA.y + Menu->Gap / 2, Menu->ColorA.Height - Menu->Gap, Menu->ColorA.Width - Menu->Gap);
    glClear(GL_COLOR_BUFFER_BIT);
    OpenGLProgramBegin(&OpenGL->VisualTransparency);
    glBlendColor(1 - Color.a, 1 - Color.a, 1 - Color.a, 1);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    OpenGLProgramEnd(&OpenGL->VisualTransparency);
    
    for(u32 I = 0; I < Menu->Steps; ++I)
    {
        r32 Alpha = (r32)I / (r32)(Menu->Steps - 1);
        glBlendColor(Alpha, Alpha, Alpha, 0);
        glScissor(Menu->Alpha.x + I * Menu->Offset.x, Menu->Alpha.y + I * Menu->Offset.y, Menu->Alpha.Width, Menu->Alpha.Height);
        glClear(GL_COLOR_BUFFER_BIT);
        OpenGLProgramBegin(&OpenGL->VisualTransparency);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        OpenGLProgramEnd(&OpenGL->VisualTransparency);
    }
    
    glDisable(GL_BLEND);
    
    v3 Lab = ConvertRGBToLab(Color.xyz);
    for(u32 I = 0; I < Menu->Steps; ++I)
    {
        r32 L = (r32)(I + 1) / (r32)(Menu->Steps + 1);
        v3 RGB = ConvertLabToRGB({L, Lab.y, Lab.z});
        v3 Grey = ConvertLabToRGB({L, 0, 0});
        RGB = ValidateRGB(RGB, Grey);
        glClearColor(RGB.x, RGB.y, RGB.z, 1);
        glScissor(Menu->L.x + I * Menu->Offset.x, Menu->L.y + I * Menu->Offset.y, Menu->L.Width, Menu->L.Height);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    
    v4 a00, a01, a10, a11, b00, b01, b10, b11;
    v4 Limits = GetABLimits(Color.xyz);
    if(Menu->Size.Width > Menu->Size.Height)
    {
        a00 = {Lab.x, Limits.x, Lab.z + 0.02f, 1};
        a01 = {Lab.x, Limits.x, Lab.z - 0.02f, 1};
        a10 = {Lab.x, Limits.y, Lab.z + 0.02f, 1};
        a11 = {Lab.x, Limits.y, Lab.z - 0.02f, 1};
        b00 = {Lab.x, Lab.y + 0.02f, Limits.z, 1};
        b01 = {Lab.x, Lab.y - 0.02f, Limits.z, 1};
        b10 = {Lab.x, Lab.y + 0.02f, Limits.w, 1};
        b11 = {Lab.x, Lab.y - 0.02f, Limits.w, 1};
    }
    else
    {
        a00 = {Lab.x, Limits.y, Lab.z + 0.02f, 1};
        a01 = {Lab.x, Limits.x, Lab.z + 0.02f, 1};
        a10 = {Lab.x, Limits.y, Lab.z - 0.02f, 1};
        a11 = {Lab.x, Limits.x, Lab.z - 0.02f, 1};
        b00 = {Lab.x, Lab.y + 0.02f, Limits.w, 1};
        b01 = {Lab.x, Lab.y + 0.02f, Limits.z, 1};
        b10 = {Lab.x, Lab.y - 0.02f, Limits.w, 1};
        b11 = {Lab.x, Lab.y - 0.02f, Limits.z, 1};
    }
    
    glViewport(Menu->a.x, Menu->a.y, Menu->a.Width, Menu->a.Height);
    glScissor(Menu->a.x, Menu->a.y, Menu->a.Width, Menu->a.Height);
    common_vertex Vertices[] =
    {
        {{ 1,  1, 0, 1}, { 0, 0}, a00},
        {{ 1, -1, 0, 1}, { 0, 0}, a01},
        {{-1,  1, 0, 1}, { 0, 0}, a10},
        {{-1, -1, 0, 1}, { 0, 0}, a11},
    };
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    OpenGLProgramBegin(&OpenGL->LabDrawProgram);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    OpenGLProgramEnd(&OpenGL->LabDrawProgram);
    
    glViewport(Menu->b.x, Menu->b.y, Menu->b.Width, Menu->b.Height);
    glScissor(Menu->b.x, Menu->b.y, Menu->b.Width, Menu->b.Height);
    common_vertex Vertices2[] =
    {
        {{ 1,  1, 0, 1}, { 0, 0}, b00},
        {{ 1, -1, 0, 1}, { 0, 0}, b01},
        {{-1,  1, 0, 1}, { 0, 0}, b10},
        {{-1, -1, 0, 1}, { 0, 0}, b11},
    };
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices2), Vertices2);
    OpenGLProgramBegin(&OpenGL->LabDrawProgram);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices2)/sizeof(*Vertices2)));
    OpenGLProgramEnd(&OpenGL->LabDrawProgram);
}

// TODO(Zyonji): Should I source from a mipmap instead?
internal v4
PickColor(open_gl *OpenGL, v2 P, v2u Size)
{
    v4 Result;
    s32 PX = (s32)P.x + Size.Width / 2;
    s32 PY = (s32)P.y + Size.Height / 2;
    glBindFramebuffer(GL_FRAMEBUFFER, OpenGL->DisplayFramebuffer.FramebufferHandle);
    glReadPixels(PX, PY, 1, 1, GL_RGBA, GL_FLOAT, &Result);
    return(Result);
}

internal void
DrawButton(u32area Area)
{
    glViewport(Area.x, Area.y, Area.Width, Area.Height);
    glScissor(Area.x, Area.y, Area.Width, Area.Height);
    glClearColor(0.6, 0.6, 0.6, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}
internal void
DrawButtonNew(open_gl *OpenGL, u32area Area)
{
    DrawButton(Area);
    common_vertex Vertices[] =
    {
        {{ 1/8.0,  6/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 1/8.0,  5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-5/8.0,  6/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-4/8.0,  5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-5/8.0, -6/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-4/8.0, -5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 5/8.0, -6/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 4/8.0, -5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 5/8.0,  2/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 4/8.0,  2/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 1/8.0,  2/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 1/8.0,  6/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 5/8.0,  2/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 2/8.0,  6/8.0, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
        {{ 5/8.0,  3/8.0, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
    };
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    
    OpenGLProgramBegin(&OpenGL->BasicDrawProgram);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    OpenGLProgramEnd(&OpenGL->BasicDrawProgram);
}
internal void
DrawButtonOpen(open_gl *OpenGL, u32area Area)
{
    DrawButton(Area);
    common_vertex Vertices[] =
    {
        {{-5/8.0,  4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-5/8.0,  3/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 6/8.0,  4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 5/8.0,  3/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 6/8.0, -5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 5/8.0, -4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-6/8.0, -5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-5/8.0, -4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-6/8.0,  6/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-5/8.0,  5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-2/8.0,  6/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-3/8.0,  5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-1/8.0,  5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-2/8.0,  4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-1/8.0,  4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
    };
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    
    OpenGLProgramBegin(&OpenGL->BasicDrawProgram);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    OpenGLProgramEnd(&OpenGL->BasicDrawProgram);
}
internal void
DrawButtonSave(open_gl *OpenGL, u32area Area)
{
    DrawButton(Area);
    common_vertex Vertices[] =
    {
        {{-3/8.0, -4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-2/8.0, -4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-3/8.0,  0/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-2/8.0, -1/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 3/8.0,  0/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 2/8.0, -1/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 3/8.0, -5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 2/8.0, -4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-5/8.0, -5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-4/8.0, -4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-5/8.0,  5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-4/8.0,  4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 3/8.0,  5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 3/8.0,  4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 5/8.0,  3/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 4/8.0,  3/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 5/8.0, -5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 4/8.0, -4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 3/8.0, -5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 3/8.0, -4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
    };
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    
    OpenGLProgramBegin(&OpenGL->BasicDrawProgram);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    OpenGLProgramEnd(&OpenGL->BasicDrawProgram);
    
    common_vertex Vertices2[] =
    {
        {{-2/8.0,  4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 0/8.0,  4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-2/8.0,  2/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 0/8.0,  3/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 2/8.0,  2/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 1/8.0,  3/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 2/8.0,  4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 1/8.0,  4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
    };
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices2), Vertices2);
    
    OpenGLProgramBegin(&OpenGL->BasicDrawProgram);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices2)/sizeof(*Vertices2)));
    OpenGLProgramEnd(&OpenGL->BasicDrawProgram);
}
internal void
DrawButtonRotateR(open_gl *OpenGL, u32area Area)
{
    DrawButton(Area);
    common_vertex Vertices[] =
    {
        {{ 0/8.0, -5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 0/8.0, -4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-4/8.0, -4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-3/8.0, -3/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-5/8.0,  0/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-4/8.0,  0/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-4/8.0,  4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-3/8.0,  3/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 0/8.0,  5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 0/8.0,  4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 4/8.0,  4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 3/8.0,  3/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 5/8.0,  0/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 4/8.0,  0/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 7/8.0,  0/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 2/8.0,  0/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{4.5/8.0, -2.5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
    };
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    
    OpenGLProgramBegin(&OpenGL->BasicDrawProgram);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    OpenGLProgramEnd(&OpenGL->BasicDrawProgram);
}
internal void
DrawButtonRotateL(open_gl *OpenGL, u32area Area)
{
    DrawButton(Area);
    common_vertex Vertices[] =
    {
        {{ 0/8.0, -5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 0/8.0, -4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 4/8.0, -4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 3/8.0, -3/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 5/8.0,  0/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 4/8.0,  0/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 4/8.0,  4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 3/8.0,  3/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 0/8.0,  5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 0/8.0,  4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-4/8.0,  4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-3/8.0,  3/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-5/8.0,  0/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-4/8.0,  0/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-7/8.0,  0/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-2/8.0,  0/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-4.5/8.0, -2.5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
    };
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    
    OpenGLProgramBegin(&OpenGL->BasicDrawProgram);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    OpenGLProgramEnd(&OpenGL->BasicDrawProgram);
}
internal void
DrawButtonOneToOne(open_gl *OpenGL, u32area Area)
{
    DrawButton(Area);
    common_vertex Vertices[] =
    {
        {{ 6/8.0,  6/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 2/8.0,  2/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 6/8.0, -6/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 2/8.0, -2/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-6/8.0, -6/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-2/8.0, -2/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-6/8.0,  6/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-2/8.0,  2/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 6/8.0,  6/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 2/8.0,  2/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
    };
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    
    OpenGLProgramBegin(&OpenGL->BasicDrawProgram);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    OpenGLProgramEnd(&OpenGL->BasicDrawProgram);
}
internal void
DrawButtonReset(open_gl *OpenGL, u32area Area)
{
    DrawButton(Area);
    common_vertex Vertices[] =
    {
        {{ 6/8.0,  6/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 5/8.0,  5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 6/8.0, -6/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 5/8.0, -5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-6/8.0, -6/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-5/8.0, -5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-6/8.0,  6/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-5/8.0,  5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 6/8.0,  6/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 5/8.0,  5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
    };
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    
    OpenGLProgramBegin(&OpenGL->BasicDrawProgram);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    OpenGLProgramEnd(&OpenGL->BasicDrawProgram);
    
    common_vertex Vertices2[] =
    {
        {{ 1/8.0,  1/8.0, 0, 1}, { 0, 0}, {0.3, 0.3, 0.3, 1}},
        {{ 1/8.0, -1/8.0, 0, 1}, { 0, 0}, {0.3, 0.3, 0.3, 1}},
        {{-1/8.0,  1/8.0, 0, 1}, { 0, 0}, {0.3, 0.3, 0.3, 1}},
        {{-1/8.0, -1/8.0, 0, 1}, { 0, 0}, {0.3, 0.3, 0.3, 1}},
    };
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices2), Vertices2);
    
    OpenGLProgramBegin(&OpenGL->BasicDrawProgram);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices2)/sizeof(*Vertices2)));
    OpenGLProgramEnd(&OpenGL->BasicDrawProgram);
}
internal void
DrawButtonMinus(open_gl *OpenGL, u32area Area)
{
    DrawButton(Area);
    common_vertex Vertices[] =
    {
        {{ 6/8.0,  1/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 6/8.0, -1/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-6/8.0,  1/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-6/8.0, -1/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
    };
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    
    OpenGLProgramBegin(&OpenGL->BasicDrawProgram);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    OpenGLProgramEnd(&OpenGL->BasicDrawProgram);
}
internal void
DrawButtonPlus(open_gl *OpenGL, u32area Area)
{
    DrawButton(Area);
    common_vertex Vertices[] =
    {
        {{ 6/8.0,  1/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 6/8.0, -1/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-6/8.0,  1/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-6/8.0, -1/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 0, 0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 0, 0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 1/8.0,  6/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 1/8.0, -6/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-1/8.0,  6/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-1/8.0, -6/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
    };
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    
    OpenGLProgramBegin(&OpenGL->BasicDrawProgram);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    OpenGLProgramEnd(&OpenGL->BasicDrawProgram);
}
internal void
DrawButtonSmall(open_gl *OpenGL, u32area Area)
{
    DrawButton(Area);
    common_vertex Vertices[] =
    {
        {{-2/8.0,  5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 2/8.0,  5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-2/8.0,  2/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 2/8.0,  2/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-4/8.0, -2/8.0, 0, 1}, { 0, 0}, {0.3, 0.3, 0.3, 1}},
        {{ 4/8.0, -2/8.0, 0, 1}, { 0, 0}, {0.3, 0.3, 0.3, 1}},
        {{-4/8.0, -8/8.0, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
        {{ 4/8.0, -8/8.0, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
    };
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    
    OpenGLProgramBegin(&OpenGL->BasicDrawProgram);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    OpenGLProgramEnd(&OpenGL->BasicDrawProgram);
}
internal void
DrawButtonBig(open_gl *OpenGL, u32area Area)
{
    DrawButton(Area);
    common_vertex Vertices[] =
    {
        {{-7/8.0,  5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 7/8.0,  5/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-7/8.0,  2/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 7/8.0,  2/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-4/8.0, -2/8.0, 0, 1}, { 0, 0}, {0.3, 0.3, 0.3, 1}},
        {{ 4/8.0, -2/8.0, 0, 1}, { 0, 0}, {0.3, 0.3, 0.3, 1}},
        {{-4/8.0, -8/8.0, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
        {{ 4/8.0, -8/8.0, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
    };
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    
    OpenGLProgramBegin(&OpenGL->BasicDrawProgram);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    OpenGLProgramEnd(&OpenGL->BasicDrawProgram);
}
internal void
DrawButtonMirror(open_gl *OpenGL, u32area Area)
{
    DrawButton(Area);
    common_vertex Vertices[] =
    {
        {{-6/8.0,  6/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-6/8.0, -6/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-3/8.0,  4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-3/8.0, -4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{-1/8.0,  2/8.0, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
        {{-1/8.0, -2/8.0, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
        {{-1/8.0,  7/8.0, 0, 1}, { 0, 0}, {0.3, 0.3, 0.3, 1}},
        {{-1/8.0, -7/8.0, 0, 1}, { 0, 0}, {0.3, 0.3, 0.3, 1}},
        {{ 1/8.0,  7/8.0, 0, 1}, { 0, 0}, {0.3, 0.3, 0.3, 1}},
        {{ 1/8.0, -7/8.0, 0, 1}, { 0, 0}, {0.3, 0.3, 0.3, 1}},
        {{ 1/8.0,  2/8.0, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
        {{ 1/8.0, -2/8.0, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
        {{ 3/8.0,  4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 3/8.0, -4/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 6/8.0,  6/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
        {{ 6/8.0, -6/8.0, 0, 1}, { 0, 0}, {0, 0, 0, 1}},
    };
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    
    OpenGLProgramBegin(&OpenGL->BasicDrawProgram);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    OpenGLProgramEnd(&OpenGL->BasicDrawProgram);
}
internal void
DrawPallet(u32area Area, u32 Gap)
{
    DrawButton(Area);
    
    u32 X = Area.x + Gap;
    u32 Y = Area.y + Gap;
    u32 Half = (Area.Width - 2 * Gap) / 2;
    u32 Third = (Area.Width - 2 * Gap) / 3;
    glClearColor(1, 1, 1, 1);
    glScissor(X, Y + 2 * Third, Half, Third);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0, 0, 0, 1);
    glScissor(X + Half, Y + 2 * Third, Half, Third);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1, 0, 0, 1);
    glScissor(X, Y + Third, Third, Third);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0, 1, 0, 1);
    glScissor(X + Third, Y + Third, Third, Third);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0, 0, 1, 1);
    glScissor(X + 2 * Third, Y + Third, Third, Third);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1, 0, 1, 1);
    glScissor(X, Y, Third, Third);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1, 1, 0, 1);
    glScissor(X + Third, Y, Third, Third);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0, 1, 1, 1);
    glScissor(X + 2 * Third, Y, Third, Third);
    glClear(GL_COLOR_BUFFER_BIT);
}
internal void
DrawToggleTop(open_gl *OpenGL, u32area Area, u32 Gap)
{
    glViewport(Area.x - Gap/2, Area.y - Gap/2, Area.Width + Gap, Area.Height + 3 * Gap);
    glScissor(Area.x - Gap/2, Area.y - Gap/2, Area.Width + Gap, Area.Height + 3 * Gap);
    common_vertex Vertices[] =
    {
        {{ 0,  7/7.0, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
        {{-1,  3/7.0, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
        {{ 1,  3/7.0, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
        {{-1, -7/7.0, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
        {{ 1, -7/7.0, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
    };
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    
    OpenGLProgramBegin(&OpenGL->BasicDrawProgram);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    OpenGLProgramEnd(&OpenGL->BasicDrawProgram);
}
internal void
DrawToggleLeft(open_gl *OpenGL, u32area Area, u32 Gap)
{
    glViewport(Area.x - 5 * Gap/2, Area.y - Gap/2, Area.Width + 3 * Gap, Area.Height + Gap);
    glScissor(Area.x - 5 * Gap/2, Area.y - Gap/2, Area.Width + 3 * Gap, Area.Height + Gap);
    common_vertex Vertices[] =
    {
        {{-7/7.0,  0, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
        {{-3/7.0, -1, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
        {{-3/7.0,  1, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
        {{ 7/7.0, -1, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
        {{ 7/7.0,  1, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
    };
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    
    OpenGLProgramBegin(&OpenGL->BasicDrawProgram);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    OpenGLProgramEnd(&OpenGL->BasicDrawProgram);
}
internal void
DrawToggleBottom(open_gl *OpenGL, u32area Area, u32 Gap)
{
    glViewport(Area.x - Gap/2, Area.y - 5 * Gap/2, Area.Width + Gap, Area.Height + 3 * Gap);
    glScissor(Area.x - Gap/2, Area.y - 5 * Gap/2, Area.Width + Gap, Area.Height + 3 * Gap);
    common_vertex Vertices[] =
    {
        {{-1, -1, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
        {{-1,  1, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
        {{ 0, -3/7.0, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
        {{ 1,  1, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
        {{ 1, -1, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
    };
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    
    OpenGLProgramBegin(&OpenGL->BasicDrawProgram);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    OpenGLProgramEnd(&OpenGL->BasicDrawProgram);
}
internal void
DrawToggleRight(open_gl *OpenGL, u32area Area, u32 Gap)
{
    glViewport(Area.x - Gap/2, Area.y - Gap/2, Area.Width + 3 * Gap, Area.Height + Gap);
    glScissor(Area.x - Gap/2, Area.y - Gap/2, Area.Width + 3 * Gap, Area.Height + Gap);
    common_vertex Vertices[] =
    {
        {{ 1, -1, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
        {{-1, -1, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
        {{ 3/7.0, 0, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
        {{-1,  1, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
        {{ 1,  1, 0, 1}, { 0, 0}, {0.6, 0.6, 0.6, 1}},
    };
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    
    OpenGLProgramBegin(&OpenGL->BasicDrawProgram);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    OpenGLProgramEnd(&OpenGL->BasicDrawProgram);
}

internal void
RenderBrushStroke(open_gl *OpenGL, canvas_state Canvas, pen_target OldPen, pen_target NewPen)
{
    u32 ColorMode = NewPen.ColorMode;
    v2 OldP = OldPen.P;
    v2 NewP = NewPen.P;
    v2 OldV = {sinf(OldPen.Radians), cosf(OldPen.Radians)};
    OldV *= (0.5f * OldPen.Width);
    v2 NewV = {sinf(NewPen.Radians), cosf(NewPen.Radians)};
    NewV *= (0.5f * NewPen.Width);
    v4 OldColor = OldPen.Color;
    v4 NewColor = NewPen.Color;
    if(ColorMode & COLOR_MODE_PRESSURE)
    {
        OldColor.a *= OldPen.Pressure;
        NewColor.a *= NewPen.Pressure;
    }
    else
    {
        OldColor.a = OldPen.Pressure;
        NewColor.a = NewPen.Pressure;
    }
    v2 Normal = Normalize(Perp(NewP - OldP));
    r32 OldAliasDistance = fabsf(Inner(OldV, Normal));
    r32 NewAliasDistance = fabsf(Inner(NewV, Normal));
    
    common_vertex Vertices[] =
    {
        {{NewP.x+NewV.x, NewP.y+NewV.y, 0, 1}, { 1, NewAliasDistance}, NewColor},
        {{OldP.x+OldV.x, OldP.y+OldV.y, 0, 1}, { 1, OldAliasDistance}, OldColor},
        {{NewP.x-NewV.x, NewP.y-NewV.y, 0, 1}, {-1, NewAliasDistance}, NewColor},
        {{OldP.x-OldV.x, OldP.y-OldV.y, 0, 1}, {-1, OldAliasDistance}, OldColor},
    };
    
    m4x4 Transform = {
        (2.0f / (r32)Canvas.Size.Width), 0, 0, 0, 
        0, (2.0f / (r32)Canvas.Size.Height), 0, 0, 
        0, 0, 1, 0, 
        0, 0, 0, 1,};
    // TODO(Zyonji): Figure out how to implement the different alpha modes. Probably through the Transferprogram
    v4 BrushMode = {};
    if(ColorMode & COLOR_MODE_LUMINANCE)
    {
        BrushMode.x = 1;
    }
    if(ColorMode & COLOR_MODE_CHROMA)
    {
        BrushMode.y = 1;
    }
    if(ColorMode & COLOR_MODE_ALPHA)
    {
        BrushMode.z = 1;
    }
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGL->SwapFramebuffer.FramebufferHandle);
    glViewport(0, 0, Canvas.Size.Width, Canvas.Size.Height);
    glScissor(0, 0, Canvas.Size.Width, Canvas.Size.Height);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    
    OpenGLProgramBegin(&OpenGL->BrushModeProgram, Transform, BrushMode);
    glBindTexture(GL_TEXTURE_2D, OpenGL->CanvasFramebuffer.ColorHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    OpenGLProgramEnd(&OpenGL->BrushModeProgram);
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGL->CanvasFramebuffer.FramebufferHandle);
    OpenGLProgramBegin(&OpenGL->StaticTransferPixelsProgram, Transform);
    glBindTexture(GL_TEXTURE_2D, OpenGL->SwapFramebuffer.ColorHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    glBindTexture(GL_TEXTURE_2D, 0);
    OpenGLProgramEnd(&OpenGL->StaticTransferPixelsProgram);
}
#if 0
internal void
RenderBrushStroke(open_gl *OpenGL, canvas_state Canvas, v2 OldP, v2 NewP, v2 OldV, v2 NewV, v4 OldColor, v4 NewColor, r32 OldAliasDistance, r32 NewAliasDistance, u32 ColorMode)
{
    common_vertex Vertices[] =
    {
        {{NewP.x+NewV.x, NewP.y+NewV.y, 0, 1}, { 1, NewAliasDistance}, NewColor},
        {{OldP.x+OldV.x, OldP.y+OldV.y, 0, 1}, { 1, OldAliasDistance}, OldColor},
        {{NewP.x-NewV.x, NewP.y-NewV.y, 0, 1}, {-1, NewAliasDistance}, NewColor},
        {{OldP.x-OldV.x, OldP.y-OldV.y, 0, 1}, {-1, OldAliasDistance}, OldColor},
    };
    
    m4x4 Transform = {
        (2.0f / (r32)Canvas.Size.Width), 0, 0, 0, 
        0, (2.0f / (r32)Canvas.Size.Height), 0, 0, 
        0, 0, 1, 0, 
        0, 0, 0, 1,};
    // TODO(Zyonji): Figure out how to implement the different alpha modes. Probably through the Transferprogram
    v4 BrushMode = {};
    if(ColorMode & COLOR_MODE_LUMINANCE)
    {
        BrushMode.x = 1;
    }
    if(ColorMode & COLOR_MODE_CHROMA)
    {
        BrushMode.y = 1;
    }
    if(ColorMode & COLOR_MODE_ALPHA)
    {
        BrushMode.z = 1;
    }
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGL->SwapFramebuffer.FramebufferHandle);
    glViewport(0, 0, Canvas.Size.Width, Canvas.Size.Height);
    glScissor(0, 0, Canvas.Size.Width, Canvas.Size.Height);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    
    OpenGLProgramBegin(&OpenGL->BrushModeProgram, Transform, BrushMode);
    glBindTexture(GL_TEXTURE_2D, OpenGL->CanvasFramebuffer.ColorHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    OpenGLProgramEnd(&OpenGL->BrushModeProgram);
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGL->CanvasFramebuffer.FramebufferHandle);
    OpenGLProgramBegin(&OpenGL->StaticTransferPixelsProgram, Transform);
    glBindTexture(GL_TEXTURE_2D, OpenGL->SwapFramebuffer.ColorHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    glBindTexture(GL_TEXTURE_2D, 0);
    OpenGLProgramEnd(&OpenGL->StaticTransferPixelsProgram);
}
#endif

internal void
DrawTimer(u32area Area, v4 Color)
{
    glScissor(Area.x, Area.y, Area.Width, Area.Height);
    glClearColor(Color.r, Color.g, Color.b, Color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}
internal void
RenderTimer(open_gl *OpenGL, menu_state Menu, v4 Color)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->ScreenFillVertexBuffer);
    DrawTimer(Menu.Time, Color);
    // TODO(Zyonji): Check if it's a good idea to use get current DC.
    SwapBuffers(wglGetCurrentDC());
}

internal void
DisplayBuffer(open_gl *OpenGL, display_state DisplayState, pen_state PenState, canvas_state Canvas, menu_state Menu, u32 Mintues)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGL->DisplayFramebuffer.FramebufferHandle);
    glViewport(0, 0, DisplayState.Size.Width, DisplayState.Size.Height);
    glScissor(0, 0, DisplayState.Size.Width, DisplayState.Size.Height);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->ScreenFillVertexBuffer);
    glClearColor(0.8, 0.8, 0.8, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    m4x4 Transform;
    
    // NOTE(Zyonji): Rendering the canvas.
    if(Canvas.Size.Height && Canvas.Size.Width)
    {
        r32 M00 = Canvas.Scale * Canvas.XMap.x * (r32)Canvas.Size.Width / (r32)DisplayState.Size.Width;
        r32 M10 = Canvas.Scale * Canvas.XMap.y * (r32)Canvas.Size.Width / (r32)DisplayState.Size.Height;
        r32 M01 = Canvas.Scale * Canvas.YMap.x * (r32)Canvas.Size.Height / (r32)DisplayState.Size.Width;
        r32 M11 = Canvas.Scale * Canvas.YMap.y * (r32)Canvas.Size.Height / (r32)DisplayState.Size.Height;
        Transform = {
            M00, M01, 0, (2 * roundf(Canvas.Center.x)) / DisplayState.Size.Width,
            M10, M11, 0, (2 * roundf(Canvas.Center.y)) / DisplayState.Size.Height,
            0, 0, 1, 0, 
            0, 0, 0, 1,};
        
        OpenGLProgramBegin(&OpenGL->TransferPixelsProgram, Transform);
        glBindTexture(GL_TEXTURE_2D, OpenGL->CanvasFramebuffer.ColorHandle);
        glGenerateMipmap(GL_TEXTURE_2D);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        OpenGLProgramEnd(&OpenGL->TransferPixelsProgram);
    }
    
    // NOTE(Zyonji): Rendering the menu.
    v2u MenuOffset = {0, 0};
    glViewport(MenuOffset.x, MenuOffset.y, Menu.Size.Width, Menu.Size.Height);
    glScissor(MenuOffset.x, MenuOffset.y, Menu.Size.Width, Menu.Size.Height);
    Transform = {
        1, 0, 0, 0, 
        0, 1, 0, 0, 
        0, 0, 1, 0, 
        0, 0, 0, 1,};
    OpenGLProgramBegin(&OpenGL->TransferPixelsProgram, Transform);
    glBindTexture(GL_TEXTURE_2D, OpenGL->MenuFramebuffer.ColorHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    OpenGLProgramEnd(&OpenGL->TransferPixelsProgram);
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glViewport(0, 0, DisplayState.Size.Width, DisplayState.Size.Height);
    glScissor(0, 0, DisplayState.Size.Width, DisplayState.Size.Height);
    Transform = {
        1, 0, 0, 0, 
        0, 1, 0, 0, 
        0, 0, 1, 0, 
        0, 0, 0, 1,};
    v2s Cursor = {(s32)PenState.Point.x + (s32)DisplayState.Size.Width / 2, (s32)PenState.Point.y + (s32)DisplayState.Size.Height / 2};
    r32 HalfWidth = 0.5f * PenState.Width;
    v2 Normal = {sinf(PenState.Radians), cosf(PenState.Radians)};
    if(Canvas.Mirrored)
        Normal.x = -Normal.x;
    
    OpenGLProgramBegin(&OpenGL->DisplayProgram, Transform, Cursor, Canvas.Scale * HalfWidth, Normal);
    glBindTexture(GL_TEXTURE_2D, OpenGL->DisplayFramebuffer.ColorHandle);
    glGenerateMipmap(GL_TEXTURE_2D);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    OpenGLProgramEnd(&OpenGL->DisplayProgram);
    
#if 0
    glViewport(Cursor.x, Cursor.y, 10, 10);
    glScissor(Cursor.x, Cursor.y, 10, 10);
    glClearColor(0, 0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
#endif
    
    u32area Area = Menu.Time;
    v4 Color;
    u32 Width;
    if(Mintues > 120)
    {
        DrawTimer(Area, {0, 0, 0, 1});
        Width = 0;
        Color = {0, 0, 0, 1};
    }
    else if(Mintues > 90)
    {
        DrawTimer(Area, {0, 1, 0, 1});
        Width = Mintues * 2 - 180;
        Color = {0, 0, 0, 1};
    }
    else if(Mintues > 60)
    {
        DrawTimer(Area, {1, 1, 0, 1});
        Width = Mintues * 2 - 120;
        Color = {0, 1, 0, 1};
    }
    else if(Mintues > 30)
    {
        DrawTimer(Area, {1, 0, 0, 1});
        Width = Mintues * 2 - 60;
        Color = {1, 1, 0, 1};
    }
    else
    {
        DrawTimer(Area, {1, 1, 1, 1});
        Width = Mintues * 2;
        Color = {1, 0, 0, 1};
    }
    if(Area.Width > Area.Height)
    {
        Area.Width = Width;
    }
    else
    {
        Area.Height = Width;
    }
    DrawTimer(Area, Color);
    // TODO(Zyonji): Should I save the device context or should that be part of display buffer?
    SwapBuffers(wglGetCurrentDC());
}

internal r32
LimitStreamDeltaMin(r32 NewDelta, r32 OldDelta)
{
    r32 Result;
    if(OldDelta < 0)
    {
        if(NewDelta < OldDelta)
            Result = Maximum(1.1f * OldDelta - 2, 0.1f * NewDelta);
        else
            Result = Minimum(0.9f * OldDelta + 2, 0.1f * NewDelta);
    }
    else
    {
        if(NewDelta > OldDelta)
            Result = Minimum(1.01f * OldDelta + 2, 0.01f * NewDelta);
        else
            Result = Maximum(0.9f * OldDelta - 2, 0.1f * NewDelta);
    }
    return(Result);
}
internal r32
LimitStreamDeltaMax(r32 NewDelta, r32 OldDelta)
{
    r32 Result;
    if(OldDelta < 0)
    {
        if(NewDelta < OldDelta)
            Result = Maximum(1.01f * OldDelta - 2, 0.01f * NewDelta);
        else
            Result = Minimum(0.9f * OldDelta + 2, 0.1f * NewDelta);
    }
    else
    {
        if(NewDelta > OldDelta)
            Result = Minimum(1.1f * OldDelta + 2, 0.1f * NewDelta);
        else
            Result = Maximum(0.9f * OldDelta - 2, 0.1f * NewDelta);
    }
    return(Result);
}

internal void
DisplayStreamFrame(open_gl *OpenGL, pen_target PenHistory, pen_target PenState, pen_target PenTarget, canvas_state Canvas)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glViewport(0, 0, 1920, 1080);
    glScissor(0, 0, 1920, 1080);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->ScreenFillVertexBuffer);
    glClearColor(0.8, 0.8, 0.8, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    m4x4 Transform;
    
    // NOTE(Zyonji): Rendering the canvas.
    if(Canvas.Size.Height && Canvas.Size.Width)
    {
        u32 Width = 1080 * Canvas.Size.Width / Canvas.Size.Height;
        
        glViewport(0, 0, Width, 1080);
        glScissor(0, 0, Width, 1080);
        Transform = {
            1, 0, 0, 0, 
            0, 1, 0, 0, 
            0, 0, 1, 0, 
            0, 0, 0, 1,};
        // TODO(Zyonji): Fix this position.
        v2s Cursor = {
            (s32)((PenTarget.P.x / Canvas.Size.Width  + 0.5f) * Width), 
            (s32)((PenTarget.P.y / Canvas.Size.Height + 0.5f) * 1080)
        };
        r32 HalfWidth = 0.5f * PenState.Width;
        v2 Normal = {sinf(PenState.Radians), cosf(PenState.Radians)};
        
        OpenGLProgramBegin(&OpenGL->DisplayProgram, Transform, Cursor, Canvas.Scale * HalfWidth, Normal);
        glBindTexture(GL_TEXTURE_2D, OpenGL->CanvasFramebuffer.ColorHandle);
        glGenerateMipmap(GL_TEXTURE_2D);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        OpenGLProgramEnd(&OpenGL->DisplayProgram);
        
        local_persist r32 MinXt = -1;
        local_persist r32 MaxXt =  1;
        local_persist r32 MinYt = -1;
        local_persist r32 MaxYt =  1;
        
        local_persist r32 MinXp = -1;
        local_persist r32 MaxXp =  1;
        local_persist r32 MinYp = -1;
        local_persist r32 MaxYp =  1;
        
        local_persist r32 MinXd =  0;
        local_persist r32 MaxXd =  0;
        local_persist r32 MinYd =  0;
        local_persist r32 MaxYd =  0;
        
        Width = 1920 - Width;
        r32 MinX = 
            Maximum(Minimum(Minimum(
            PenHistory.P.x - PenHistory.Width, 
            PenState.P.x - PenState.Width), 
                            PenTarget.P.x - PenTarget.Width),  
                    -0.5f * Canvas.Size.Width);
        r32 MaxX = 
            Minimum(Maximum(Maximum(
            PenHistory.P.x + PenHistory.Width, 
            PenState.P.x + PenState.Width), 
                            PenTarget.P.x + PenTarget.Width),  
                    0.5f * Canvas.Size.Width); 
        r32 MinY =
            Maximum(Minimum(Minimum(
            PenHistory.P.y - PenHistory.Width, 
            PenState.P.y - PenState.Width), 
                            PenTarget.P.y - PenTarget.Width),  
                    -0.5f * Canvas.Size.Height);
        r32 MaxY = 
            Minimum(Maximum(Maximum(
            PenHistory.P.y + PenHistory.Width, 
            PenState.P.y + PenState.Width), 
                            PenTarget.P.y + PenTarget.Width),  
                    0.5f * Canvas.Size.Height);
        
        if(MinX < MinXt || MinX - 200 > 0.5f * MinXt + 0.5f * MaxXt)
            MinXt = MinX - 200;
        if(MaxX > MaxXt || MaxX + 200 < 0.5f * MaxXt + 0.5f * MinXt)
            MaxXt = MaxX + 200;
        if(MinY < MinYt || MinY - 200 > 0.5f * MinYt + 0.5f * MaxYt)
            MinYt = MinY - 200;
        if(MaxY > MaxYt || MaxY + 200 < 0.5f * MaxYt + 0.5f * MinYt)
            MaxYt = MaxY + 200;
        
        MinXd = LimitStreamDeltaMin(MinXt - MinXp, MinXd);
        MaxXd = LimitStreamDeltaMax(MaxXt - MaxXp, MaxXd);
        MinYd = LimitStreamDeltaMin(MinYt - MinYp, MinYd);
        MaxYd = LimitStreamDeltaMax(MaxYt - MaxYp, MaxYd);
        
        MinXp += MinXd;
        MaxXp += MaxXd;
        MinYp += MinYd;
        MaxYp += MaxYd;
        
        v2 Scale = {1080.0f * (r32)Canvas.Size.Width / (r32)Width, (r32)Canvas.Size.Height};
        r32 Zoom =  1.0f / Maximum(MaxXp - MinXp, MaxYp - MinYp);
        Scale *= Zoom;
        
        glViewport(1920 - Width, 0, Width, 1080);
        glScissor(1920 - Width, 0, Width, 1080);
        Transform = {
            Scale.x, 0, 0, -(MinXp + MaxXp) * Zoom * 1080.0f / (r32)Width,
            0, Scale.y, 0, -(MinYp + MaxYp) * Zoom,
            0, 0, 1, 0, 
            0, 0, 0, 1,};
        OpenGLProgramBegin(&OpenGL->TransferPixelsProgram, Transform);
        glBindTexture(GL_TEXTURE_2D, OpenGL->CanvasFramebuffer.ColorHandle);
        glGenerateMipmap(GL_TEXTURE_2D);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        OpenGLProgramEnd(&OpenGL->TransferPixelsProgram);
    }
    
    SwapBuffers(wglGetCurrentDC());
}
