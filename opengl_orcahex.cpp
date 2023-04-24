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
    GLuint BrushStyleID;
    GLuint LightLimitID;
    GLuint DarkLimitID;
};
struct line_mode_program
{
    render_program_base Common;
    
    GLuint OriginID;
    GLuint TargetID;
    GLuint Color1ID;
    GLuint Color2ID;
    GLuint Radius1ID;
    GLuint Radius2ID;
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
    v2u Size;
};
struct open_gl
{
    b32 Initialized;
    b32 TESTReferenceOverlay;
    HGLRC RenderingContext;
    
    render_program_base        VisualTransparency;
    render_program_base        TransferPixelsProgram;
    render_program_base        ConvertToLabProgram;
    render_program_base        ConvertToRGBProgram;
    render_program_base        StaticTransferPixelsProgram;
    render_program_base        BasicDrawProgram;
    render_program_base        LabDrawProgram;
    render_program_base        LchDrawProgram;
    brush_mode_program         BrushModeProgram;
    brush_mode_program         BrushAlphaModeProgram;
    line_mode_program          LineModeProgram;
    display_program            DisplayProgram;
    
    frame_buffer DisplayFramebuffer;
    frame_buffer CanvasFramebuffer;
    frame_buffer SwapFramebuffer;
    frame_buffer MenuFramebuffer;
    frame_buffer TilesetFramebuffer;
    frame_buffer ReferenceFramebuffer;
    
    GLuint ScreenFillVertexBuffer;
    GLuint VertexBuffer;
};

internal GLuint
OpenGLCreateProgram(char *HeaderCode, char *VertexCode, char *FragmentCode, render_program_base *Result)
{
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLchar *VertexShaderCode[] =
    {
        HeaderCode,
        VertexCode,
    };
    glShaderSource(VertexShaderID, ArrayCount(VertexShaderCode), VertexShaderCode, 0);
    glCompileShader(VertexShaderID);
    
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    GLchar *FragmentShaderCode[] =
    {
        HeaderCode,
        FragmentCode,
    };
    glShaderSource(FragmentShaderID, ArrayCount(FragmentShaderCode), FragmentShaderCode, 0);
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

global char *GlobalBasicShaderHeaderCode = R"glsl(
#version 330

)glsl";

global char *GlobalLabShaderHeaderCode = R"glsl(
#version 330
vec3 ConvertRGBToLab(vec3 RGB)
{
    float r = RGB.r;
    float g = RGB.g;
    float b = RGB.b;
    
    r = (r > 0.04045) ? pow((r + 0.055) / 1.055, 2.4) : (r / 12.92);
    g = (g > 0.04045) ? pow((g + 0.055) / 1.055, 2.4) : (g / 12.92);
    b = (b > 0.04045) ? pow((b + 0.055) / 1.055, 2.4) : (b / 12.92);
    
    float x = r * 0.4124564 + g * 0.3575761 + b * 0.1804375;
    float y = r * 0.2126729 + g * 0.7151522 + b * 0.0721750;
    float z = r * 0.0193339 + g * 0.1191920 + b * 0.9503041;
    
    x = x / 0.95047;
    y = y / 1.0000;
    z = z / 1.08883;
    
    x = (x > 0.008856) ? pow(x, 1.0 / 3.0) : (7.787 * x + 0.160 / 1.160);
    y = (y > 0.008856) ? pow(y, 1.0 / 3.0) : (7.787 * y + 0.160 / 1.160);
    z = (z > 0.008856) ? pow(z, 1.0 / 3.0) : (7.787 * z + 0.160 / 1.160);
    
    vec3 Lab = vec3((1.160 * y) - 0.160, 1.25 * (x - y) + 0.5, 0.5 * (y - z) + 0.5);
    
    return(Lab);
}

vec3 ConvertLabToRGB(vec3 Lab)
{
    float y = (Lab.x + 0.160) / 1.160;
    float x = (Lab.y - 0.5) / 1.25 + y;
    float z = y - (Lab.z - 0.5) / 0.5;
    
    float x3 = x * x * x;
    float y3 = y * y * y;
    float z3 = z * z * z;
    
    x = ((x3 > 0.008856) ? x3 : ((x - 0.160 / 1.160) / 7.787)) * 0.95047;
    y = ((y3 > 0.008856) ? y3 : ((y - 0.160 / 1.160) / 7.787)) * 1.000;
    z = ((z3 > 0.008856) ? z3 : ((z - 0.160 / 1.160) / 7.787)) * 1.08883;
    
    float r = x *  3.2404542 + y * -1.5371385 + z * -0.4985314;
    float g = x * -0.9692660 + y *  1.8760108 + z *  0.0415560;
    float b = x *  0.0556434 + y * -0.2040259 + z *  1.0572252;
    
    r = (r > 0.0031308) ? (1.055 * pow(r, 1 / 2.4) - 0.055) : (12.92 * r);
    g = (g > 0.0031308) ? (1.055 * pow(g, 1 / 2.4) - 0.055) : (12.92 * g);
    b = (b > 0.0031308) ? (1.055 * pow(b, 1 / 2.4) - 0.055) : (12.92 * b);
    
    vec3 RGB = vec3(r, g, b);
    
    return(RGB);
}
)glsl";

internal GLuint
CompileVisualTransparency(open_gl *OpenGL, render_program_base *Result)
{
    char *VertexCode = R"glsl(
// Vertex code
uniform mat4 Transform;

in vec4 VertP;
in vec4 VertColor;

smooth out vec4 Color;

void main(void)
{
gl_Position = Transform * VertP;
Color = VertColor;
}
)glsl";
    
    char *FragmentCode = R"glsl(
// Fragment code

smooth in vec4 Color;
in vec4 gl_FragCoord;

out vec4 FragmentColor;

void main(void)
{
FragmentColor = Color;
if(mod(floor(gl_FragCoord.x / 8) + floor(gl_FragCoord.y / 8), 2) > 0.5)
{
FragmentColor = vec4(1 - FragmentColor.x, 1 - FragmentColor.y, 1 - FragmentColor.z, FragmentColor.w);
}
}
)glsl";
    
    GLuint Program = OpenGLCreateProgram(GlobalBasicShaderHeaderCode, VertexCode, FragmentCode, Result);
    
    return(Program);
}

internal GLuint
CompileTransferPixels(open_gl *OpenGL, render_program_base *Result)
{
    char *VertexCode = R"glsl(
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
// Fragment code
uniform sampler2D Image;

smooth in vec2 FragUV;

out vec4 FragmentColor;

void main(void)
{
FragmentColor = texture(Image, FragUV);
}
)glsl";
    
    GLuint Program = OpenGLCreateProgram(GlobalBasicShaderHeaderCode, VertexCode, FragmentCode, Result);
    
    return(Program);
}
internal GLuint
CompileConvertToLab(open_gl *OpenGL, render_program_base *Result)
{
    char *VertexCode = R"glsl(
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
// Fragment code
uniform sampler2D Image;

smooth in vec2 FragUV;

out vec4 FragmentColor;

void main(void)
{
FragmentColor = vec4(ConvertRGBToLab(texture(Image, FragUV).xyz), texture(Image, FragUV).w);
}
)glsl";
    
    GLuint Program = OpenGLCreateProgram(GlobalLabShaderHeaderCode, VertexCode, FragmentCode, Result);
    
    return(Program);
}
internal GLuint
CompileConvertToRGB(open_gl *OpenGL, render_program_base *Result)
{
    char *VertexCode = R"glsl(
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
// Fragment code
uniform sampler2D Image;

smooth in vec2 FragUV;

out vec4 FragmentColor;

void main(void)
{
FragmentColor = vec4(ConvertLabToRGB(texture(Image, FragUV).xyz), 1.0);
}
)glsl";
    
    GLuint Program = OpenGLCreateProgram(GlobalLabShaderHeaderCode, VertexCode, FragmentCode, Result);
    
    return(Program);
}
internal GLuint
CompileStaticTransferPixels(open_gl *OpenGL, render_program_base *Result)
{
    char *VertexCode = R"glsl(
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
// Fragment code
uniform sampler2D Image;

smooth in vec2 FragUV;

out vec4 FragmentColor;

void main(void)
{
FragmentColor = texture(Image, FragUV);
}
)glsl";
    
    GLuint Program = OpenGLCreateProgram(GlobalBasicShaderHeaderCode, VertexCode, FragmentCode, Result);
    
    return(Program);
}

internal GLuint
CompileBasicDraw(open_gl *OpenGL, render_program_base *Result)
{
    char *VertexCode = R"glsl(
// Vertex code
uniform mat4 Transform;

in vec4 VertP;
in vec4 VertColor;

smooth out vec4 Color;

void main(void)
{
vec4 Position = Transform * VertP;
gl_Position = Position;
Color = VertColor;
}
)glsl";
    
    char *FragmentCode = R"glsl(
// Fragment code

smooth in vec4 Color;

out vec4 FragmentColor;

void main(void)
{
FragmentColor = Color;
     }
)glsl";
    
    GLuint Program = OpenGLCreateProgram(GlobalBasicShaderHeaderCode, VertexCode, FragmentCode, Result);
    
    return(Program);
}

internal GLuint
CompileLabDraw(open_gl *OpenGL, render_program_base *Result)
{
    char *VertexCode = R"glsl(
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
// Fragment code

smooth in vec4 Color;

out vec4 FragmentColor;

void main(void)
{
vec3 RGB = ConvertLabToRGB(Color.xyz);
vec4 Lab = Color;

    float Cmax = max(max(RGB.x, RGB.y), RGB.z);
    float Cmin = min(min(RGB.x, RGB.y), RGB.z);
    if(Cmax > 1.0 || Cmin < 0.0)
    {
           Lab = vec4(0.91117078, 0.5, 0.5, 1);
    }
    
     FragmentColor = Lab;
     }
)glsl";
    
    GLuint Program = OpenGLCreateProgram(GlobalLabShaderHeaderCode, VertexCode, FragmentCode, Result);
    
    return(Program);
}
internal GLuint
CompileLchDraw(open_gl *OpenGL, render_program_base *Result)
{
    char *VertexCode = R"glsl(
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
// Fragment code

smooth in vec4 Color;

out vec4 FragmentColor;

void main(void)
{
vec3 Lab = vec3(Color.x, Color.y * cos(Color.z) + 0.5, Color.y * sin(Color.z) + 0.5);
vec3 RGB = ConvertLabToRGB(Lab);

    float Cmax = max(max(RGB.x, RGB.y), RGB.z);
    float Cmin = min(min(RGB.x, RGB.y), RGB.z);
    if(Cmax > 1.0 || Cmin < 0.0)
    {
          Lab= vec3(0.91117078, 0.5, 0.5);
    }
    
     FragmentColor = vec4(Lab, 1);
     }
)glsl";
    
    GLuint Program = OpenGLCreateProgram(GlobalLabShaderHeaderCode, VertexCode, FragmentCode, Result);
    
    return(Program);
}

// TODO(Zyonji): Should I change the code to reuse the common parts of the BrushModes?
// TODO(Zyonji): Schould there be an exception for Alpha 1?
internal GLuint
CompileBrushMode(open_gl *OpenGL, brush_mode_program *Result)
{
    char *VertexCode = R"glsl(
// Vertex code
uniform mat4 Transform;

in vec4 VertP;
in vec2 VertUV;
in vec4 VertColor;

smooth out vec2 FragUV;
smooth out vec4 Color;

void main(void)
{
vec4 Position = Transform * VertP;
gl_Position = Position;
FragUV = VertUV;
Color = VertColor;
}
)glsl";
    
    char *FragmentCode = R"glsl(
// Fragment code
uniform vec4 BrushMode;
uniform float BrushStyle;
uniform float LightLimit;
uniform float DarkLimit;
uniform sampler2D Image;

smooth in vec2 FragUV;
smooth in vec4 Color;

out vec4 FragmentColor;

void main(void)
{
vec4 BaseColor = texelFetch(Image, ivec2(gl_FragCoord.xy), 0);
float Alpha = gl_FragCoord.z;

if(BrushStyle > 1.5)
{
Alpha *= clamp((1.0 - abs(FragUV.x)) * FragUV.y, 0.0, 1.0);
} 
else
{
if(FragUV.x + BrushStyle > 0)
{
Alpha *= 0.5 * (1 + cos(FragUV.x * 3.1415926535897932384626433832795));
}
else
{
 Alpha *= clamp(1.0 + FragUV.x * FragUV.y, 0.0, 1.0);
}
}

float Blend = 1 - ((1 - Alpha) * BaseColor.w);

vec3 Lab0 = Color.xyz;
     vec3 Lab1 = BaseColor.xyz;
     vec3 BlendMode = vec3(Blend * BrushMode.x, Blend * BrushMode.y, Blend * BrushMode.y);
     
     if(Lab0.x * LightLimit > Lab1.x)
     {
     BlendMode.x = 0;
     }
     if(Lab0.x < Lab1.x * DarkLimit)
     {
     BlendMode.x = 0;
     }
     
     vec3 Lab = mix(Lab1, Lab0, BlendMode);
     
     float AlphaOut = clamp(Alpha * BrushMode.z + BaseColor.w, 0.0, 1.0);
     
     FragmentColor = vec4(Lab, AlphaOut);
     }
)glsl";
    
    GLuint Program = OpenGLCreateProgram(GlobalLabShaderHeaderCode, VertexCode, FragmentCode, &Result->Common);
    
    Result->BrushModeID = glGetUniformLocation(Program, "BrushMode");
    Result->BrushStyleID = glGetUniformLocation(Program, "BrushStyle");
    Result->LightLimitID = glGetUniformLocation(Program, "LightLimit");
    Result->DarkLimitID = glGetUniformLocation(Program, "DarkLimit");
    
    return(Program);
}
internal GLuint
CompileBrushAlphaMode(open_gl *OpenGL, brush_mode_program *Result)
{
    char *VertexCode = R"glsl(
// Vertex code
uniform mat4 Transform;

in vec4 VertP;
in vec2 VertUV;
in vec4 VertColor;

smooth out vec2 FragUV;
smooth out vec4 Color;

void main(void)
{
vec4 Position = Transform * VertP;
gl_Position = Position;
FragUV = VertUV;
Color = VertColor;
}
)glsl";
    
    char *FragmentCode = R"glsl(
// Fragment code
uniform vec4 BrushMode;
uniform float BrushStyle;
uniform float LightLimit;
uniform float DarkLimit;
uniform sampler2D Image;

smooth in vec2 FragUV;
smooth in vec4 Color;

out vec4 FragmentColor;

void main(void)
{
vec4 BaseColor = texelFetch(Image, ivec2(gl_FragCoord.xy), 0);
float Alpha = gl_FragCoord.z;

if(BrushStyle > 1.5)
{
Alpha *= clamp((1.0 - abs(FragUV.x)) * FragUV.y, 0.0, 1.0);
} 
else
{
if(FragUV.x + BrushStyle > 0)
{
Alpha *= 0.5 * (1 + cos(FragUV.x * 3.1415926535897932384626433832795));
}
else
{
 Alpha *= clamp(1.0 + FragUV.x * FragUV.y, 0.0, 1.0);
}
}

vec4 Lab0 = Color;
     vec4 Lab1 = BaseColor;
     vec4 BlendMode = vec4(Alpha * BrushMode.x, Alpha * BrushMode.y, Alpha * BrushMode.y, Alpha * BrushMode.z);
     
     if(Lab0.x * LightLimit > Lab1.x)
     {
     BlendMode.x = 0;
     }
     if(Lab0.x < Lab1.x * DarkLimit)
     {
     BlendMode.x = 0;
     }
     
     vec4 Lab = mix(Lab1, Lab0, BlendMode);
     
     FragmentColor = Lab;
     }
)glsl";
    
    GLuint Program = OpenGLCreateProgram(GlobalLabShaderHeaderCode, VertexCode, FragmentCode, &Result->Common);
    
    Result->BrushModeID = glGetUniformLocation(Program, "BrushMode");
    Result->BrushStyleID = glGetUniformLocation(Program, "BrushStyle");
    Result->LightLimitID = glGetUniformLocation(Program, "LightLimit");
    Result->DarkLimitID = glGetUniformLocation(Program, "DarkLimit");
    
    return(Program);
}
internal GLuint
CompileLineMode(open_gl *OpenGL, line_mode_program *Result)
{
    char *VertexCode = R"glsl(
// Vertex code
uniform mat4 Transform;

in vec4 VertP;
in vec2 VertUV;
in vec4 VertColor;

void main(void)
{
vec4 Position = Transform * VertP;
gl_Position = Position;
}
)glsl";
    
    char *FragmentCode = R"glsl(
// Fragment code
uniform vec2 Origin;
uniform vec2 Target;
uniform vec4 Color1;
uniform vec4 Color2;
uniform float Radius1;
uniform float Radius2;

in vec4 gl_FragCoord;

out vec4 FragmentColor;

 void main(void)
{
 float Alpha;
float OriginAlpha;
float TargetAlpha;
vec2 FragP = gl_FragCoord.xy;
vec2 Point = FragP - Origin;
vec2 Line = Target - Origin;
float Interpolator = clamp(dot(Point, Line) / dot(Line, Line), 0.0, 1.0);
 vec4 Color = mix(Color1, Color2, Interpolator);
float Radius = mix(Radius1, Radius2, Interpolator);
float Distance = length(Point - Interpolator * Line);
float NormalizedDistance = clamp(Distance / Radius, 0.0, 1.0);
float OriginDistance = length(Point);
float NormalizedOriginDistance = clamp(OriginDistance / Radius1, 0.0, 1.0);
float TargetDistance = length(Point - Line);
float NormalizedTargetDistance = clamp(TargetDistance / Radius2, 0.0, 1.0);

 Alpha = 0.5 * Color.w * (1 + cos(NormalizedDistance * 3.1415926535897932384626433832795));
OriginAlpha = 0.5 * Color1.w * (1 + cos(NormalizedOriginDistance * 3.1415926535897932384626433832795));
TargetAlpha = 0.5 * Color2.w * (1 + cos(NormalizedDistance * 3.1415926535897932384626433832795));
Alpha = max(Alpha, TargetAlpha);

if(OriginAlpha >= 1 || OriginAlpha >= Alpha)
{
 Color.w = 0;
}
else
{
 Color.w = clamp(1 - ((1 - Alpha) / (1 - OriginAlpha)), 0.0, 1.0);
}

     FragmentColor = Color;
     }
)glsl";
    
    GLuint Program = OpenGLCreateProgram(GlobalLabShaderHeaderCode, VertexCode, FragmentCode, &Result->Common);
    
    Result->OriginID = glGetUniformLocation(Program, "Origin");
    Result->TargetID = glGetUniformLocation(Program, "Target");
    Result->Color1ID = glGetUniformLocation(Program, "Color1");
    Result->Color2ID = glGetUniformLocation(Program, "Color2");
    Result->Radius1ID = glGetUniformLocation(Program, "Radius1");
    Result->Radius2ID = glGetUniformLocation(Program, "Radius2");
    
    return(Program);
}
#if 0
internal GLuint
CompileAreaFill(open_gl *OpenGL, area_fill_program *Result)
{
    char *VertexCode = R"glsl(
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
// Fragment code
uniform vec2 Center;
uniform vec2 Radius;
uniform vec4 BrushMode;
uniform sampler2D Image;
uniform sampler2D Mask;

 smooth in vec2 FragUV;
 
out vec4 FragmentColor;

void main(void)
{
vec4 Color = texture(Mask, FragUV);
float Alpha = Color.w * ((Radius.x == 0.0) ? 1.0 : clamp(length((FragUV - Center) * Radius), 0.0, 1.0));

vec3 Lab0 = texture(Image, FragUV).xyz;
     vec3 Lab1 = Color.xyz;
     
     vec3 BlendMode = vec3(Alpha * BrushMode.x, Alpha * BrushMode.y, Alpha * BrushMode.y);
     vec3 Lab = mix(Lab0, Lab1, BlendMode);
     
     FragmentColor = vec4(Lab, 1);
     }
)glsl";
    
    GLuint Program = OpenGLCreateProgram(GlobalLabShaderHeaderCode, VertexCode, FragmentCode, &Result->Common);
    
    Result->ImageID = glGetUniformLocation(Program, "Image");
    Result->MaskID = glGetUniformLocation(Program, "Mask");
    
    Result->RadiusID = glGetUniformLocation(Program, "Radius");
    Result->CenterID = glGetUniformLocation(Program, "Center");
    Result->BrushModeID = glGetUniformLocation(Program, "BrushMode");
    
    return(Program);
}
#endif

internal GLuint
CompileDisplay(open_gl *OpenGL, display_program *Result)
{
    char *VertexCode = R"glsl(
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
vec4 DisplayLab = texture(Image, FragUV);
vec4 DisplayColor = vec4(ConvertLabToRGB(DisplayLab.xyz), 1.0);

vec4 AlphaColor = vec4(0.4, 0.4, 0.4, 1);
if(mod(floor(gl_FragCoord.x / 8) + floor(gl_FragCoord.y / 8), 2) > 0.5)
{
AlphaColor = vec4(0.6, 0.6, 0.6, 1);
}

FragmentColor = mix(AlphaColor, DisplayColor, DisplayLab.w);

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
    
    GLuint Program = OpenGLCreateProgram(GlobalLabShaderHeaderCode, VertexCode, FragmentCode, &Result->Common);
    
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
OpenGLProgramBegin(render_program_base *Program, m4x4 Transform)
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
    
    glUniformMatrix4fv(Program->TransformID, 1, GL_TRUE, Transform.E[0]);
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
OpenGLProgramBegin(render_program_base *Program)
{
    m4x4 Transform = {
        1, 0, 0, 0, 
        0, 1, 0, 0, 
        0, 0, 1, 0, 
        0, 0, 0, 1,};
    OpenGLProgramBegin(Program, Transform);
}

internal void
OpenGLProgramBegin(brush_mode_program *Program, m4x4 Transform, v4 BrushMode, r32 BrushStyle, r32 LightLimit, r32 DarkLimit)
{
    OpenGLProgramBegin(&Program->Common);
    
    glUniformMatrix4fv(Program->Common.TransformID, 1, GL_TRUE, Transform.E[0]);
    glUniform4fv(Program->BrushModeID, 1, BrushMode.E);
    glUniform1f(Program->BrushStyleID, BrushStyle);
    glUniform1f(Program->LightLimitID, LightLimit);
    glUniform1f(Program->DarkLimitID, DarkLimit);
}
internal void
OpenGLProgramEnd(brush_mode_program *Program)
{
    OpenGLProgramEnd(&Program->Common);
}

internal void
OpenGLProgramBegin(line_mode_program *Program, m4x4 Transform, v2 Origin, v2 Target, v4 Color1, v4 Color2, r32 Radius1, r32 Radius2)
{
    OpenGLProgramBegin(&Program->Common);
    
    glUniformMatrix4fv(Program->Common.TransformID, 1, GL_TRUE, Transform.E[0]);
    glUniform2fv(Program->OriginID, 1, Origin.E);
    glUniform2fv(Program->TargetID, 1, Target.E);
    glUniform4fv(Program->Color1ID, 1, Color1.E);
    glUniform4fv(Program->Color2ID, 1, Color2.E);
    glUniform1f(Program->Radius1ID, Radius1);
    glUniform1f(Program->Radius2ID, Radius2);
}
internal void
OpenGLProgramEnd(line_mode_program *Program)
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
    GLuint DummyVertexArray;
    glGenVertexArrays(1, &DummyVertexArray);
    glBindVertexArray(DummyVertexArray);
    
    glGenBuffers(1, &OpenGL->ScreenFillVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->ScreenFillVertexBuffer);
    common_vertex Vertices[] =
    {
        // NOTE(Zyonji): The colors are for the transparency pattern.
        {{-1,  1, 0, 1}, {0, 1}, {0.3, 0.5, 0.5, 1.0}},
        {{-1, -1, 0, 1}, {0, 0}, {0.3, 0.5, 0.5, 1.0}},
        {{ 1,  1, 0, 1}, {1, 1}, {0.3, 0.5, 0.5, 1.0}},
        {{ 1, -1, 0, 1}, {1, 0}, {0.3, 0.5, 0.5, 1.0}},
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
    if(!CompileTransferPixels(OpenGL,       &OpenGL->TransferPixelsProgram))
    {
        LogError("Unable to compile the pixel transfer program.", "OpenGL");
        Return = false;
    }
    if(!CompileConvertToLab(OpenGL,         &OpenGL->ConvertToLabProgram))
    {
        LogError("Unable to compile the Lab convresion program.", "OpenGL");
        Return = false;
    }
    if(!CompileConvertToRGB(OpenGL,         &OpenGL->ConvertToRGBProgram))
    {
        LogError("Unable to compile the RGB convresion program.", "OpenGL");
        Return = false;
    }
    if(!CompileStaticTransferPixels(OpenGL, &OpenGL->StaticTransferPixelsProgram))
    {
        LogError("Unable to compile the pixel transfer program.", "OpenGL");
        Return = false;
    }
    if(!CompileBasicDraw(OpenGL,            &OpenGL->BasicDrawProgram))
    {
        LogError("Unable to compile the basic draw program.", "OpenGL");
        Return = false;
    }
    if(!CompileLabDraw(OpenGL,              &OpenGL->LabDrawProgram))
    {
        LogError("Unable to compile the Lab draw program.", "OpenGL");
        Return = false;
    }
    if(!CompileLchDraw(OpenGL,              &OpenGL->LchDrawProgram))
    {
        LogError("Unable to compile the Lch draw program.", "OpenGL");
        Return = false;
    }
    if(!CompileBrushMode(OpenGL,            &OpenGL->BrushModeProgram))
    {
        LogError("Unable to compile the brush stroke renderer.", "OpenGL");
        Return = false;
    }
    if(!CompileBrushAlphaMode(OpenGL,       &OpenGL->BrushAlphaModeProgram))
    {
        LogError("Unable to compile the alpha sensitive brush stroke renderer.", "OpenGL");
        Return = false;
    }
    if(!CompileLineMode(OpenGL,             &OpenGL->LineModeProgram))
    {
        LogError("Unable to compile the line stroke renderer.", "OpenGL");
        Return = false;
    }
    if(!CompileDisplay(OpenGL,              &OpenGL->DisplayProgram))
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
    
    Result.Size = {Width, Height};
    
    glGenFramebuffers(1, &Result.FramebufferHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, Result.FramebufferHandle);
    
    Result.ColorHandle  = FramebufferTextureImage(OpenGL, GL_TEXTURE_2D, Width, Height, GL_RGBA12, Memory);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Result.ColorHandle, 0);
    
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

internal frame_buffer
CreateConvertedFramebuffer(open_gl *OpenGL, u32 Width, u32 Height, void *Memory)
{
    frame_buffer Result = CreateFramebuffer(OpenGL, Width, Height, 0);
    frame_buffer TempBuffer = CreateFramebuffer(OpenGL, Width, Height, Memory);
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Result.FramebufferHandle);
    glViewport(0, 0, Width, Height);
    glScissor(0, 0, Width, Height);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->ScreenFillVertexBuffer);
    m4x4 Transform = {
        1, 0, 0, 0, 
        0, 1, 0, 0, 
        0, 0, 1, 0, 
        0, 0, 0, 1,};
    OpenGLProgramBegin(&OpenGL->ConvertToLabProgram, Transform);
    glBindTexture(GL_TEXTURE_2D, TempBuffer.ColorHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    OpenGLProgramEnd(&OpenGL->ConvertToLabProgram);
    
    FreeFramebuffer(&TempBuffer);
    
    glBindTexture(GL_TEXTURE_2D, Result.ColorHandle);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    return(Result);
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
    *TargetBuffer = CreateFramebuffer(OpenGL, Width, Height, 0);
    *SwapBuffer = CreateFramebuffer(OpenGL, Width, Height, Memory);
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, TargetBuffer->FramebufferHandle);
    glViewport(0, 0, Width, Height);
    glScissor(0, 0, Width, Height);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->ScreenFillVertexBuffer);
    if(Memory)
    {
        m4x4 Transform = {
            1, 0, 0, 0, 
            0, 1, 0, 0, 
            0, 0, 1, 0, 
            0, 0, 0, 1,};
        OpenGLProgramBegin(&OpenGL->ConvertToLabProgram, Transform);
        glBindTexture(GL_TEXTURE_2D, SwapBuffer->ColorHandle);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        OpenGLProgramEnd(&OpenGL->ConvertToLabProgram);
    }
    else
    {
        // NOTE(Zyonji): The color for a new empty canvas.
        glClearColor(0.7, 0.5, 0.5, 1);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}

internal v4
PickColor(open_gl *OpenGL, v2 P)
{
    v4 Result;
    s32 PX = (s32)P.x + OpenGL->DisplayFramebuffer.Size.Width / 2;
    s32 PY = (s32)P.y + OpenGL->DisplayFramebuffer.Size.Height / 2;
    glBindFramebuffer(GL_FRAMEBUFFER, OpenGL->DisplayFramebuffer.FramebufferHandle);
    glReadPixels(PX, PY, 1, 1, GL_RGBA, GL_FLOAT, &Result);
    return(Result);
}

internal v2
NormalizeTiles(v2 P)
{
    v2 Result = {P.x / TilesWidth, P.y / TilesHeight};
    return(Result);
}
internal void
DrawTile(open_gl *OpenGL, u32area Area, v2 P00, v2 P01, v2 P10, v2 P11)
{
    glViewport(Area.x, Area.y, Area.Width, Area.Height);
    glScissor(Area.x, Area.y, Area.Width, Area.Height);
    
    common_vertex Vertices[] =
    {
        {{-1, -1, 0, 1}, NormalizeTiles(P00), {0, 0, 0, 1}},
        {{-1,  1, 0, 1}, NormalizeTiles(P01), {0, 0, 0, 1}},
        {{ 1, -1, 0, 1}, NormalizeTiles(P10), {0, 0, 0, 1}},
        {{ 1,  1, 0, 1}, NormalizeTiles(P11), {0, 0, 0, 1}},
    };
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    
    m4x4 Transform = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0, 
        0, 0, 0, 1,};
    OpenGLProgramBegin(&OpenGL->TransferPixelsProgram, Transform);
    glBindTexture(GL_TEXTURE_2D, OpenGL->TilesetFramebuffer.ColorHandle);
    glGenerateMipmap(GL_TEXTURE_2D);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    OpenGLProgramEnd(&OpenGL->TransferPixelsProgram);
}
internal void
DrawButton(open_gl *OpenGL, u32area Area)
{
    DrawTile(OpenGL, Area, {  0.0f, 384.0f}, {  0.0f, 448.0f}, { 64.0f, 384.0f}, { 64.0f, 448.0f});
}
internal void
DrawButtonNew(open_gl *OpenGL, u32area Area)
{
    DrawTile(OpenGL, Area, {  0.0f, 192.0f}, {  0.0f, 256.0f}, { 64.0f, 192.0f}, { 64.0f, 256.0f});
}
internal void
DrawButtonOpen(open_gl *OpenGL, u32area Area)
{
    DrawTile(OpenGL, Area, {  0.0f, 256.0f}, {  0.0f, 320.0f}, { 64.0f, 256.0f}, { 64.0f, 320.0f});
}
internal void
DrawButtonSave(open_gl *OpenGL, u32area Area)
{
    DrawTile(OpenGL, Area, {  0.0f, 320.0f}, {  0.0f, 384.0f}, { 64.0f, 320.0f}, { 64.0f, 384.0f});
}
internal void
DrawButtonRotateR(open_gl *OpenGL, u32area Area)
{
    DrawTile(OpenGL, Area, {128.0f, 128.0f}, {128.0f, 192.0f}, {192.0f, 128.0f}, {192.0f, 192.0f});
}
internal void
DrawButtonRotateL(open_gl *OpenGL, u32area Area)
{
    DrawTile(OpenGL, Area, {128.0f,  64.0f}, {128.0f, 128.0f}, {192.0f,  64.0f}, {192.0f, 128.0f});
}
internal void
DrawButtonOneToOne(open_gl *OpenGL, u32area Area)
{
    DrawTile(OpenGL, Area, {  0.0f,  64.0f}, {  0.0f, 128.0f}, { 64.0f,  64.0f}, { 64.0f, 128.0f});
}
internal void
DrawButtonReset(open_gl *OpenGL, u32area Area)
{
    DrawTile(OpenGL, Area, {  0.0f, 128.0f}, {  0.0f, 192.0f}, { 64.0f, 128.0f}, { 64.0f, 192.0f});
}
internal void
DrawButtonMinus(open_gl *OpenGL, u32area Area)
{
    DrawTile(OpenGL, Area, {128.0f, 320.0f}, {128.0f, 384.0f}, {192.0f, 320.0f}, {192.0f, 384.0f});
}
internal void
DrawButtonPlus(open_gl *OpenGL, u32area Area)
{
    DrawTile(OpenGL, Area, {128.0f, 384.0f}, {128.0f, 448.0f}, {192.0f, 384.0f}, {192.0f, 448.0f});
}
internal void
DrawButtonSmall(open_gl *OpenGL, u32area Area)
{
    DrawTile(OpenGL, Area, {128.0f, 256.0f}, {128.0f, 320.0f}, {192.0f, 256.0f}, {192.0f, 320.0f});
}
internal void
DrawButtonBig(open_gl *OpenGL, u32area Area)
{
    DrawTile(OpenGL, Area, {128.0f, 192.0f}, {128.0f, 256.0f}, {192.0f, 192.0f}, {192.0f, 256.0f});
}
internal void
DrawButtonMirror(open_gl *OpenGL, u32area Area)
{
    DrawTile(OpenGL, Area, {  0.0f,   0.0f}, {  0.0f,  64.0f}, { 64.0f,   0.0f}, { 64.0f,  64.0f});
}
internal void
DrawTimerBorder(open_gl *OpenGL, u32area Area)
{
    if(Area.Height < Area.Width)
    {
        DrawTile(OpenGL, Area, { 52.0f, 464.0f}, { 52.0f, 512.0f}, { 60.0f, 464.0f}, { 60.0f, 512.0f});
    }
    else
    {
        DrawTile(OpenGL, Area, { 52.0f, 464.0f}, { 60.0f, 464.0f}, { 52.0f, 512.0f}, { 60.0f, 512.0f});
    }
}
internal void
DrawPallet(u32area Area)
{
    u32 X = Area.x;
    u32 Y = Area.y;
    u32 Half = Area.Width / 2;
    u32 Third = Area.Width / 3;
    v3 Lab;
    glClearColor(1, 0.5, 0.5, 1);
    glScissor(X, Y + 2 * Third, Half, Third);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0, 0.5, 0.5, 1);
    glScissor(X + Half, Y + 2 * Third, Third * 3 - Half, Third);
    glClear(GL_COLOR_BUFFER_BIT);
    Lab = ConvertRGBToLab({1, 0, 0});
    glClearColor(Lab.x, Lab.y, Lab.z, 1);
    glScissor(X, Y + Third, Third, Third);
    glClear(GL_COLOR_BUFFER_BIT);
    Lab = ConvertRGBToLab({0, 1, 0});
    glClearColor(Lab.x, Lab.y, Lab.z, 1);
    glScissor(X + Third, Y + Third, Third, Third);
    glClear(GL_COLOR_BUFFER_BIT);
    Lab = ConvertRGBToLab({0, 0, 1});
    glClearColor(Lab.x, Lab.y, Lab.z, 1);
    glScissor(X + 2 * Third, Y + Third, Third, Third);
    glClear(GL_COLOR_BUFFER_BIT);
    Lab = ConvertRGBToLab({1, 0, 1});
    glClearColor(Lab.x, Lab.y, Lab.z, 1);
    glScissor(X, Y, Third, Third);
    glClear(GL_COLOR_BUFFER_BIT);
    Lab = ConvertRGBToLab({1, 1, 0});
    glClearColor(Lab.x, Lab.y, Lab.z, 1);
    glScissor(X + Third, Y, Third, Third);
    glClear(GL_COLOR_BUFFER_BIT);
    Lab = ConvertRGBToLab({0, 1, 1});
    glClearColor(Lab.x, Lab.y, Lab.z, 1);
    glScissor(X + 2 * Third, Y, Third, Third);
    glClear(GL_COLOR_BUFFER_BIT);
}
internal void
DrawToggle(open_gl *OpenGL, u32area Area, b32 Toggle)
{
    if(Toggle)
    {
        DrawTile(OpenGL, Area, {256.0f, 448.0f}, {256.0f, 480.0f}, {288.0f, 448.0f}, {288.0f, 480.0f});
    }
    else
    {
        DrawTile(OpenGL, Area, {256.0f, 480.0f}, {256.0f, 512.0f}, {288.0f, 480.0f}, {288.0f, 512.0f});
    }
}
internal void
DrawSwitch(open_gl *OpenGL, u32area Area, u32 Toggle)
{
    if(Toggle == 2)
    {
        DrawTile(OpenGL, Area, {256.0f, 192.0f}, {256.0f, 224.0f}, {288.0f, 192.0f}, {288.0f, 224.0f});
    }
    else if(Toggle == 1)
    {
        DrawTile(OpenGL, Area, {256.0f, 256.0f}, {256.0f, 288.0f}, {288.0f, 256.0f}, {288.0f, 288.0f});
    }
    else
    {
        DrawTile(OpenGL, Area, {256.0f, 288.0f}, {256.0f, 320.0f}, {288.0f, 288.0f}, {288.0f, 320.0f});
    }
}
internal void
DrawRangeSwitch(open_gl *OpenGL, u32area Area, b32 Truned, u32 Toggle)
{
    r32 X1, X2, Y1, Y2;
    X1 = 256.0f;
    X2 = 288.0f;
    Y1 =  96.0f + Toggle * 32.0f;
    Y2 = 128.0f + Toggle * 32.0f;
    
    if(Truned)
    {
        DrawTile(OpenGL, Area, {X1, Y1}, {X1, Y2}, {X2, Y1}, {X2, Y2});
    }
    else
    {
        DrawTile(OpenGL, Area, {X2, Y1}, {X1, Y1}, {X2, Y2}, {X1, Y2});
    }
}
internal void
DrawColorToggle(open_gl *OpenGL, u32area Area, b32 Mirrored, b32 Toggle)
{
    r32 X1, X2, Y1, Y2;
    if(Mirrored)
    {
        X1 = 256.0f;
        X2 = 288.0f;
    }
    else
    {
        X1 = 288.0f;
        X2 = 256.0f;
    }
    if(Toggle)
    {
        Y1 = 320.0f;
        Y2 = 384.0f;
    }
    else
    {
        Y1 = 384.0f;
        Y2 = 448.0f;
    }
    
    if(Area.Height > Area.Width)
    {
        DrawTile(OpenGL, Area, {X1, Y1}, {X1, Y2}, {X2, Y1}, {X2, Y2});
    }
    else
    {
        DrawTile(OpenGL, Area, {X1, Y1}, {X2, Y1}, {X1, Y2}, {X2, Y2});
    }
}
internal void
DrawShortToggle(open_gl *OpenGL, u32area Area, b32 Toggle)
{
    if(Toggle)
    {
        if(Area.Height < Area.Width)
        {
            DrawTile(OpenGL, Area, { 64.0f, 480.0f}, { 64.0f, 512.0f}, {128.0f, 480.0f}, {128.0f, 512.0f});
        }
        else
        {
            DrawTile(OpenGL, Area, { 64.0f, 512.0f}, {128.0f, 512.0f}, { 64.0f, 480.0f}, {128.0f, 480.0f});
        }
    }
    else
    {
        if(Area.Height < Area.Width)
        {
            DrawTile(OpenGL, Area, { 64.0f, 448.0f}, { 64.0f, 480.0f}, {128.0f, 448.0f}, {128.0f, 480.0f});
        }
        else
        {
            DrawTile(OpenGL, Area, { 64.0f, 480.0f}, {128.0f, 480.0f}, { 64.0f, 448.0f}, {128.0f, 448.0f});
        }
    }
}
internal void
DrawLongToggle(open_gl *OpenGL, u32area Area, b32 Toggle)
{
    if(Toggle)
    {
        if(Area.Height < Area.Width)
        {
            DrawTile(OpenGL, Area, {128.0f, 480.0f}, {128.0f, 512.0f}, {256.0f, 480.0f}, {256.0f, 512.0f});
        }
        else
        {
            DrawTile(OpenGL, Area, {128.0f, 512.0f}, {256.0f, 512.0f}, {128.0f, 480.0f}, {256.0f, 480.0f});
        }
    }
    else
    {
        if(Area.Height < Area.Width)
        {
            DrawTile(OpenGL, Area, {128.0f, 448.0f}, {128.0f, 480.0f}, {256.0f, 448.0f}, {256.0f, 480.0f});
        }
        else
        {
            DrawTile(OpenGL, Area, {128.0f, 480.0f}, {256.0f, 480.0f}, {128.0f, 448.0f}, {256.0f, 448.0f});
        }
    }
}

internal void
UpdateMenu(open_gl *OpenGL, menu_state *Menu, v4 Color, u32 Mode)
{
    r32 Light = 0.91117078;
    r32 Dark = 0.72759241;
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGL->MenuFramebuffer.FramebufferHandle);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->ScreenFillVertexBuffer);
    glViewport(0, 0, Menu->Size.Width, Menu->Size.Height);
    
    glClearColor(Color.x, Color.y, Color.z, 1);
    glScissor(Menu->ColorB.x, Menu->ColorB.y, Menu->ColorB.Height, Menu->ColorB.Width);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(Color.x, Color.y, Color.z, Color.w);
    glScissor(Menu->ColorA.x, Menu->ColorA.y, Menu->ColorA.Height, Menu->ColorA.Width);
    glClear(GL_COLOR_BUFFER_BIT);
    
    for(u32 I = 0; I < Menu->Steps; ++I)
    {
        r32 Alpha = 1.0f - (r32)I / (r32)(Menu->Steps - 1);
        glClearColor(Color.x, Color.y, Color.z, Alpha);
        glScissor(Menu->Alpha.x + I * Menu->Offset.x, Menu->Alpha.y + I * Menu->Offset.y, Menu->Alpha.Width, Menu->Alpha.Height);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    
    DrawToggle(OpenGL, Menu->TiltToggle, Mode & TILT_MODE_DISABLE);
    DrawToggle(OpenGL, Menu->LinePenToggle, Mode & LINE_PEN_MODE);
    DrawToggle(OpenGL, Menu->PenButtonsToggle, Mode & PEN_BUTTON_MODE_FLIPPED);
    DrawSwitch(OpenGL, Menu->BrushStyleSwitch, Mode & PEN_BRUSH_STYLE);
    
    DrawColorToggle(OpenGL, Menu->ColorButtonA, true, Mode & COLOR_MODE_PRESSURE);
    DrawColorToggle(OpenGL, Menu->ColorButtonB, false, !(Mode & COLOR_MODE_PRESSURE));
    DrawShortToggle(OpenGL, Menu->AlphaButton, !(Mode & COLOR_MODE_ALPHA));
    DrawShortToggle(OpenGL, Menu->LButton, !(Mode & COLOR_MODE_LUMINANCE));
    DrawRangeSwitch(OpenGL, Menu->LLimitToggle, Menu->Size.Width > Menu->Size.Height, 2 - ((Mode & COLOR_LIMIT_LUMINANCE) >> COLOR_LIMIT_OFFSET));
    DrawLongToggle(OpenGL, Menu->abButton, !(Mode & COLOR_MODE_CHROMA));
    DrawToggle(OpenGL, Menu->chButton, (Mode & MENU_MODE_AB));
    
    for(u32 I = 0; I < Menu->Steps; ++I)
    {
        r32 L = (r32)(I + 1) / (r32)(Menu->Steps + 1);
        v3 Step = ValidatedLabForRGB({L, Color.y, Color.z});
        glClearColor(Step.x, Step.y, Step.z, 1);
        glScissor(Menu->L.x + I * Menu->Offset.x, Menu->L.y + I * Menu->Offset.y, Menu->L.Width, Menu->L.Height);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    v4 L00, L01, L10, L11;
    if(Menu->Size.Width > Menu->Size.Height)
    {
        L00 = {0, Color.y, Color.z, 1};
        L01 = {0, Color.y, Color.z, 1};
        L10 = {1, Color.y, Color.z, 1};
        L11 = {1, Color.y, Color.z, 1};
        
        glViewport(Menu->LSmooth.x, Menu->LSmooth.y - Menu->LSmooth.Height, Menu->LSmooth.Width, Menu->LSmooth.Height);
        glScissor(Menu->LSmooth.x, Menu->LSmooth.y - Menu->LSmooth.Height, Menu->LSmooth.Width, Menu->LSmooth.Height);
        glClearColor(Light, 0.5, 0.5, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        u32 WidthPortion = Menu->LSmooth.Width - (u32)(Color.x * (r32)Menu->LSmooth.Width);
        glViewport(Menu->LSmooth.x, Menu->LSmooth.y - Menu->LSmooth.Height, WidthPortion, Menu->LSmooth.Height);
        glScissor(Menu->LSmooth.x, Menu->LSmooth.y - Menu->LSmooth.Height, WidthPortion, Menu->LSmooth.Height);
        glClearColor(Dark, 0.5, 0.5, 1);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    else
    {
        L00 = {1, Color.y, Color.z, 1};
        L01 = {0, Color.y, Color.z, 1};
        L10 = {1, Color.y, Color.z, 1};
        L11 = {0, Color.y, Color.z, 1};
        
        glViewport(Menu->LSmooth.x - Menu->LSmooth.Width, Menu->LSmooth.y, Menu->LSmooth.Width, Menu->LSmooth.Height);
        glScissor(Menu->LSmooth.x - Menu->LSmooth.Width, Menu->LSmooth.y, Menu->LSmooth.Width, Menu->LSmooth.Height);
        glClearColor(Dark, 0.5, 0.5, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        u32 HeightPortion = (u32)(Color.x * (r32)Menu->LSmooth.Height);
        glViewport(Menu->LSmooth.x - Menu->LSmooth.Width, Menu->LSmooth.y, Menu->LSmooth.Width, HeightPortion);
        glScissor(Menu->LSmooth.x - Menu->LSmooth.Width, Menu->LSmooth.y, Menu->LSmooth.Width, HeightPortion);
        glClearColor(Light, 0.5, 0.5, 1);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    glViewport(Menu->LSmooth.x, Menu->LSmooth.y, Menu->LSmooth.Width, Menu->LSmooth.Height);
    glScissor(Menu->LSmooth.x, Menu->LSmooth.y, Menu->LSmooth.Width, Menu->LSmooth.Height);
    common_vertex Vertices[] =
    {
        {{ 1,  1, 0, 1}, { 0, 0}, L00},
        {{ 1, -1, 0, 1}, { 0, 0}, L01},
        {{-1,  1, 0, 1}, { 0, 0}, L10},
        {{-1, -1, 0, 1}, { 0, 0}, L11},
    };
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    OpenGLProgramBegin(&OpenGL->LabDrawProgram);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    OpenGLProgramEnd(&OpenGL->LabDrawProgram);
    
    u32area Gap = Menu->a;
    if(Mode & MENU_MODE_AB)
    {
        v4 a00, a01, a10, a11, b00, b01, b10, b11;
        if(Menu->Size.Width > Menu->Size.Height)
        {
            r32 Width = 0.3375f * Menu->a.Height / Menu->a.Width;
            a00 = {Color.x, 0.1625, Color.z + Width, 1};
            a01 = {Color.x, 0.1625, Color.z - Width, 1};
            a10 = {Color.x, 0.8375, Color.z + Width, 1};
            a11 = {Color.x, 0.8375, Color.z - Width, 1};
            b00 = {Color.x, Color.y + Width, 0.1625, 1};
            b01 = {Color.x, Color.y - Width, 0.1625, 1};
            b10 = {Color.x, Color.y + Width, 0.8375, 1};
            b11 = {Color.x, Color.y - Width, 0.8375, 1};
            
            Gap.y = Menu->a.y + Menu->a.Height;
            Gap.Height = (Menu->b.y - Gap.y) / 2;
            glViewport(Gap.x, Gap.y, Gap.Width, Gap.Height * 2);
            glScissor(Gap.x, Gap.y, Gap.Width, Gap.Height * 2);
            glClearColor(Light, 0.5, 0.5, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            u32 APortion = (u32)((1 - Color.y) * Gap.Width);
            glViewport(Gap.x, Gap.y, APortion, Gap.Height);
            glScissor(Gap.x, Gap.y, APortion, Gap.Height);
            glClearColor(Dark, 0.5, 0.5, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            u32 BPortion = (u32)((1 - Color.z) * Gap.Width);
            glViewport(Gap.x, Gap.y + Gap.Height, BPortion, Gap.Height);
            glScissor(Gap.x, Gap.y + Gap.Height, BPortion, Gap.Height);
            glClearColor(Dark, 0.5, 0.5, 1);
            glClear(GL_COLOR_BUFFER_BIT);
        }
        else
        {
            r32 Width = 0.3375f * Menu->a.Width / Menu->a.Height;
            a00 = {Color.x, 0.8375, Color.z + Width, 1};
            a01 = {Color.x, 0.1625, Color.z + Width, 1};
            a10 = {Color.x, 0.8375, Color.z - Width, 1};
            a11 = {Color.x, 0.1625, Color.z - Width, 1};
            b00 = {Color.x, Color.y + Width, 0.8375, 1};
            b01 = {Color.x, Color.y + Width, 0.1625, 1};
            b10 = {Color.x, Color.y - Width, 0.8375, 1};
            b11 = {Color.x, Color.y - Width, 0.1625, 1};
            
            Gap.x = Menu->a.x + Menu->a.Width;
            Gap.Width = (Menu->b.x - Gap.x) / 2;
            glViewport(Gap.x, Gap.y, Gap.Width * 2, Gap.Height);
            glScissor(Gap.x, Gap.y, Gap.Width * 2, Gap.Height);
            glClearColor(Dark, 0.5, 0.5, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            u32 APortion = (u32)((1 + Color.y) * Gap.Height);
            glViewport(Gap.x, Gap.y, Gap.Width, APortion);
            glScissor(Gap.x, Gap.y, Gap.Width, APortion);
            glClearColor(Light, 0.5, 0.5, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            u32 BPortion = (u32)((1 + Color.z) * Gap.Height);
            glViewport(Gap.x + Gap.Width, Gap.y, Gap.Width, BPortion);
            glScissor(Gap.x + Gap.Width, Gap.y, Gap.Width, BPortion);
            glClearColor(Light, 0.5, 0.5, 1);
            glClear(GL_COLOR_BUFFER_BIT);
        }
        
        glViewport(Menu->a.x, Menu->a.y, Menu->a.Width, Menu->a.Height);
        glScissor(Menu->a.x, Menu->a.y, Menu->a.Width, Menu->a.Height);
        common_vertex Vertices1[] =
        {
            {{ 1,  1, 0, 1}, { 0, 0}, a00},
            {{ 1, -1, 0, 1}, { 0, 0}, a01},
            {{-1,  1, 0, 1}, { 0, 0}, a10},
            {{-1, -1, 0, 1}, { 0, 0}, a11},
        };
        glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices1), Vertices1);
        OpenGLProgramBegin(&OpenGL->LabDrawProgram);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices1)/sizeof(*Vertices1)));
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
    else
    {
        v4 a00, a01, a10, a11, b00, b01, b10, b11;
        v2 AB = {Color.y - 0.5f, Color.z - 0.5f};
        v3 Lch = {Color.x, Length(AB), (r32)atan2(Color.z - 0.5, Color.y - 0.5)};
        b32 Flipped = false;
        if(Lch.z < 0)
        {
            Lch.z += Pi32;
            Flipped = true;
        }
        if(Menu->Size.Width > Menu->Size.Height)
        {
            r32 Width = 0.3375f * Menu->a.Height / Menu->a.Width;
            a00 = {Lch.x, -0.3375, Lch.z + Width, 1};
            a01 = {Lch.x, -0.3375, Lch.z - Width, 1};
            a10 = {Lch.x,  0.3375, Lch.z + Width, 1};
            a11 = {Lch.x,  0.3375, Lch.z - Width, 1};
            b00 = {Lch.x, Lch.y + Width, 2 * Pi32, 1};
            b01 = {Lch.x, Lch.y - Width, 2 * Pi32, 1};
            b10 = {Lch.x, Lch.y + Width, 0, 1};
            b11 = {Lch.x, Lch.y - Width, 0, 1};
            
            Gap.y = Menu->a.y + Menu->a.Height;
            Gap.Height = (Menu->b.y - Gap.y) / 2;
            glViewport(Gap.x, Gap.y, Gap.Width, Gap.Height * 2);
            glScissor(Gap.x, Gap.y, Gap.Width, Gap.Height * 2);
            glClearColor(Light, 0.5, 0.5, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            u32 APortion = (u32)((0.5 - Lch.y / 0.675) * Gap.Width);
            if(Flipped)
            {
                APortion = Gap.Width - APortion;
            }
            glViewport(Gap.x, Gap.y, APortion, Gap.Height);
            glScissor(Gap.x, Gap.y, APortion, Gap.Height);
            glClearColor(Dark, 0.5, 0.5, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            u32 BPortion = (u32)(0.5f * (Lch.z / Pi32) * Gap.Width);
            if(Flipped)
            {
                BPortion += (u32)(0.5f * Gap.Width);
            }
            glViewport(Gap.x, Gap.y + Gap.Height, BPortion, Gap.Height);
            glScissor(Gap.x, Gap.y + Gap.Height, BPortion, Gap.Height);
            glClearColor(Dark, 0.5, 0.5, 1);
            glClear(GL_COLOR_BUFFER_BIT);
        }
        else
        {
            r32 Width = 0.3375f * Menu->a.Width / Menu->a.Height;
            a00 = {Lch.x,  0.3375, Lch.z + Width, 1};
            a01 = {Lch.x, -0.3375, Lch.z + Width, 1};
            a10 = {Lch.x,  0.3375, Lch.z - Width, 1};
            a11 = {Lch.x, -0.3375, Lch.z - Width, 1};
            b00 = {Lch.x, Lch.y + Width, 0, 1};
            b01 = {Lch.x, Lch.y + Width, 2 * Pi32, 1};
            b10 = {Lch.x, Lch.y - Width, 0, 1};
            b11 = {Lch.x, Lch.y - Width, 2 * Pi32, 1};
            
            Gap.x = Menu->a.x + Menu->a.Width;
            Gap.Width = (Menu->b.x - Gap.x) / 2;
            glViewport(Gap.x, Gap.y, Gap.Width * 2, Gap.Height);
            glScissor(Gap.x, Gap.y, Gap.Width * 2, Gap.Height);
            glClearColor(Dark, 0.5, 0.5, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            u32 APortion = (u32)((Lch.y / 0.675 + 0.5) * Gap.Height);
            if(Flipped)
            {
                APortion = Gap.Height - APortion;
            }
            glViewport(Gap.x, Gap.y, Gap.Width, APortion);
            glScissor(Gap.x, Gap.y, Gap.Width, APortion);
            glClearColor(Light, 0.5, 0.5, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            u32 BPortion = (u32)((1 - 0.5f * (Lch.z / Pi32)) * Gap.Height);
            if(Flipped)
            {
                BPortion -= (u32)(0.5f * Gap.Height);
            }
            glViewport(Gap.x + Gap.Width, Gap.y, Gap.Width, BPortion);
            glScissor(Gap.x + Gap.Width, Gap.y, Gap.Width, BPortion);
            glClearColor(Light, 0.5, 0.5, 1);
            glClear(GL_COLOR_BUFFER_BIT);
        }
        glViewport(Menu->a.x, Menu->a.y, Menu->a.Width, Menu->a.Height);
        glScissor(Menu->a.x, Menu->a.y, Menu->a.Width, Menu->a.Height);
        common_vertex Vertices1[] =
        {
            {{ 1,  1, 0, 1}, { 0, 0}, a00},
            {{ 1, -1, 0, 1}, { 0, 0}, a01},
            {{-1,  1, 0, 1}, { 0, 0}, a10},
            {{-1, -1, 0, 1}, { 0, 0}, a11},
        };
        glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices1), Vertices1);
        OpenGLProgramBegin(&OpenGL->LchDrawProgram);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices1)/sizeof(*Vertices1)));
        OpenGLProgramEnd(&OpenGL->LchDrawProgram);
        
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
        OpenGLProgramBegin(&OpenGL->LchDrawProgram);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices2)/sizeof(*Vertices2)));
        OpenGLProgramEnd(&OpenGL->LchDrawProgram);
    }
}

internal void
RenderBrushStroke(open_gl *OpenGL, canvas_state Canvas, pen_target OldPen, pen_target NewPen)
{
    u32 Mode = NewPen.Mode;
    v2 OldP = OldPen.P;
    v2 NewP = NewPen.P;
    v2 OldV = {sinf(OldPen.Radians), cosf(OldPen.Radians)};
    OldV *= (0.5f * OldPen.Width);
    v2 NewV = {sinf(NewPen.Radians), cosf(NewPen.Radians)};
    NewV *= (0.5f * NewPen.Width);
    v4 OldColor = OldPen.Color;
    v4 NewColor = NewPen.Color;
    r32 OldPress = OldPen.Pressure;
    r32 NewPress = NewPen.Pressure;
    v2 Normal = Normalize(Perp(NewP - OldP));
    r32 OldAliasDistance = fabsf(Inner(OldV, Normal));
    r32 NewAliasDistance = fabsf(Inner(NewV, Normal));
    
    common_vertex Vertices[] =
    {
        {{NewP.x+NewV.x, NewP.y+NewV.y, NewPress, 1}, { 1, NewAliasDistance}, NewColor},
        {{OldP.x+OldV.x, OldP.y+OldV.y, OldPress, 1}, { 1, OldAliasDistance}, OldColor},
        {{NewP.x-NewV.x, NewP.y-NewV.y, NewPress, 1}, {-1, NewAliasDistance}, NewColor},
        {{OldP.x-OldV.x, OldP.y-OldV.y, OldPress, 1}, {-1, OldAliasDistance}, OldColor},
    };
    
    m4x4 Transform = {
        (2.0f / (r32)Canvas.Size.Width), 0, 0, 0, 
        0, (2.0f / (r32)Canvas.Size.Height), 0, 0, 
        0, 0, 1, 0, 
        0, 0, 0, 1,};
    // TODO(Zyonji): Figure out how to implement the different alpha modes. Probably through the Transferprogram
    v4 BrushMode = {};
    if(Mode & COLOR_MODE_LUMINANCE)
    {
        BrushMode.x = 1;
    }
    if(Mode & COLOR_MODE_CHROMA)
    {
        BrushMode.y = 1;
    }
    if(Mode & COLOR_MODE_ALPHA)
    {
        BrushMode.z = 1;
    }
    r32 LightLimit = 0;
    if(Mode & COLOR_LIMIT_LIGHT)
    {
        LightLimit = 1.0f;
    }
    r32 DarkLimit = 0;
    if(Mode & COLOR_LIMIT_DARK)
    {
        DarkLimit = 1.0f;
    }
    
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGL->SwapFramebuffer.FramebufferHandle);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGL->CanvasFramebuffer.FramebufferHandle);
    glViewport(0, 0, Canvas.Size.Width, Canvas.Size.Height);
    glScissor(0, 0, Canvas.Size.Width, Canvas.Size.Height);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    
    if(Mode & COLOR_MODE_PRESSURE)
    {
        OpenGLProgramBegin(&OpenGL->BrushAlphaModeProgram, Transform, BrushMode, (r32)(Mode & PEN_BRUSH_STYLE), LightLimit, DarkLimit);
    }
    else
    {
        OpenGLProgramBegin(&OpenGL->BrushModeProgram, Transform, BrushMode, (r32)(Mode & PEN_BRUSH_STYLE), LightLimit, DarkLimit);
    }
    glBindTexture(GL_TEXTURE_2D, OpenGL->CanvasFramebuffer.ColorHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    OpenGLProgramEnd(&OpenGL->BrushModeProgram); //BrushModeProgram and BrushAlphaModeProgram are identical
    
#if 0
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGL->CanvasFramebuffer.FramebufferHandle);
    OpenGLProgramBegin(&OpenGL->StaticTransferPixelsProgram, Transform);
    glBindTexture(GL_TEXTURE_2D, OpenGL->SwapFramebuffer.ColorHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    glBindTexture(GL_TEXTURE_2D, 0);
    OpenGLProgramEnd(&OpenGL->StaticTransferPixelsProgram);
#endif
}

// TODO(Zyonji): To make it easier to test out, I gave this an internal state with previous points.  
internal void
RenderLine(open_gl *OpenGL, canvas_state Canvas, pen_target OldPen, pen_target NewPen)
{
    v2 Center = {(0.5f * (r32)Canvas.Size.Width), (0.5f * (r32)Canvas.Size.Height)};
    u32 Mode = NewPen.Mode;
    v2 OldP = OldPen.P;
    v2 NewP = NewPen.P;
    r32 OldRadius = 0.5f * OldPen.Width;
    r32 NewRadius = 0.5f * NewPen.Width;
    v4 OldColor = OldPen.Color;
    v4 NewColor = NewPen.Color;
    if(Mode & COLOR_MODE_PRESSURE)
    {
        OldColor.a *= OldPen.Pressure;
        NewColor.a *= NewPen.Pressure;
    }
    else
    {
        OldColor.a = OldPen.Pressure;
        NewColor.a = NewPen.Pressure;
    }
    
    v2 MinP = {Minimum(OldP.x - OldRadius, NewP.x - NewRadius), Minimum(OldP.y - OldRadius, NewP.y - NewRadius)};
    v2 MaxP = {Maximum(OldP.x + OldRadius, NewP.x + NewRadius), Maximum(OldP.y + OldRadius, NewP.y + NewRadius)};
    
    common_vertex Vertices[] =
    {
        {{MinP.x, MinP.y, 0, 1}, {}, {}},
        {{MaxP.x, MinP.y, 0, 1}, {}, {}},
        {{MinP.x, MaxP.y, 0, 1}, {}, {}},
        {{MaxP.x, MaxP.y, 0, 1}, {}, {}},
    };
    
    m4x4 Transform = {
        (2.0f / (r32)Canvas.Size.Width), 0, 0, 0, 
        0, (2.0f / (r32)Canvas.Size.Height), 0, 0, 
        0, 0, 1, 0, 
        0, 0, 0, 1,};
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGL->CanvasFramebuffer.FramebufferHandle);
    glViewport(0, 0, Canvas.Size.Width, Canvas.Size.Height);
    glScissor(0, 0, Canvas.Size.Width, Canvas.Size.Height);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    OpenGLProgramBegin(&OpenGL->LineModeProgram, Transform, OldP + Center, NewP + Center, OldColor, NewColor, OldRadius, NewRadius);
    glBindTexture(GL_TEXTURE_2D, OpenGL->CanvasFramebuffer.ColorHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    OpenGLProgramEnd(&OpenGL->LineModeProgram);
    
    glDisable(GL_BLEND);
}

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
RenderCanvas(open_gl *OpenGL, canvas_state Canvas, m4x4 Transform, pen_state PenState)
{
    // TODO(Zyonji): Make the Reference code more structured.
    if(OpenGL->TESTReferenceOverlay)
    {
        glBindTexture(GL_TEXTURE_2D, OpenGL->ReferenceFramebuffer.ColorHandle);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, OpenGL->CanvasFramebuffer.ColorHandle);
    }
    
    glGenerateMipmap(GL_TEXTURE_2D);
    
    OpenGLProgramBegin(&OpenGL->TransferPixelsProgram, Transform);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    OpenGLProgramEnd(&OpenGL->TransferPixelsProgram);
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

internal void
DisplayBuffer(open_gl *OpenGL, u32area PaintingRegion, pen_state PenState, canvas_state Canvas, menu_state Menu, r32 TimerProgression)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGL->DisplayFramebuffer.FramebufferHandle);
    glViewport(PaintingRegion.x, PaintingRegion.y, PaintingRegion.Width, PaintingRegion.Height);
    glScissor(PaintingRegion.x, PaintingRegion.y, PaintingRegion.Width, PaintingRegion.Height);
    glClearColor(0.8, 0.5, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    m4x4 Transform;
    
    // NOTE(Zyonji): Rendering the reference.
    if(OpenGL->ReferenceFramebuffer.FramebufferHandle)
    {
        v2 Region = {(r32)PaintingRegion.Width, (r32)PaintingRegion.Height};
        v2 Reference = {(r32)Canvas.ReferenceSize.Width, (r32)Canvas.ReferenceSize.Height};
        v2 ReferenceScale ={};
        
        if(Region.x / Reference.x < Region.y / Reference.y)
        {
            ReferenceScale = {Reference.x / Region.x * Region.y / Reference.y, 1};
        }
        else
        {
            ReferenceScale = {1, Reference.y / Region.y * Region.x / Reference.x};
        }
        
        v4 ReferenceLimits = {1, -1, 1, -1};
        if(Canvas.Center.x > 0 && Canvas.Center.y > 0)
        {
            ReferenceLimits = {ReferenceScale.x - 1, -1, ReferenceScale.y - 1, -1};
        }
        else if(Canvas.Center.x > 0)
        {
            ReferenceLimits = {ReferenceScale.x - 1, -1, 1, 1 - ReferenceScale.y};
        }
        else if(Canvas.Center.y > 0)
        {
            ReferenceLimits = {1, 1 - ReferenceScale.x, ReferenceScale.y - 1, -1};
        }
        else
        {
            ReferenceLimits = {1, 1 - ReferenceScale.x, 1, 1 - ReferenceScale.y};
        }
        
        common_vertex Vertices[] =
        {
            {{ReferenceLimits.Left,  ReferenceLimits.Top,    0, 1}, {0, 1}, {}},
            {{ReferenceLimits.Left,  ReferenceLimits.Bottom, 0, 1}, {0, 0}, {}},
            {{ReferenceLimits.Right, ReferenceLimits.Top,    0, 1}, {1, 1}, {}},
            {{ReferenceLimits.Right, ReferenceLimits.Bottom, 0, 1}, {1, 0}, {}},
        };
        glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
        
        Transform = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0, 
            0, 0, 0, 1,};
        
        OpenGLProgramBegin(&OpenGL->TransferPixelsProgram, Transform);
        glBindTexture(GL_TEXTURE_2D, OpenGL->ReferenceFramebuffer.ColorHandle);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        OpenGLProgramEnd(&OpenGL->TransferPixelsProgram);
        
        common_vertex Vertices2[] =
        {
            {{-ReferenceLimits.Right, -ReferenceLimits.Bottom, 0, 1}, {0, 1}, {}},
            {{-ReferenceLimits.Right, -ReferenceLimits.Top,    0, 1}, {0, 0}, {}},
            {{-ReferenceLimits.Left,  -ReferenceLimits.Bottom, 0, 1}, {1, 1}, {}},
            {{-ReferenceLimits.Left,  -ReferenceLimits.Top,    0, 1}, {1, 0}, {}},
        };
        glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices2), Vertices2);
        
        OpenGLProgramBegin(&OpenGL->TransferPixelsProgram, Transform);
        glBindTexture(GL_TEXTURE_2D, OpenGL->ReferenceFramebuffer.ColorHandle);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        OpenGLProgramEnd(&OpenGL->TransferPixelsProgram);
    }
    
    // NOTE(Zyonji): Rendering the canvas.
    if(Canvas.Size.Height && Canvas.Size.Width)
    {
        r32 M00 = Canvas.Scale * Canvas.XMap.x * (r32)Canvas.Size.Width / (r32)PaintingRegion.Width;
        r32 M10 = Canvas.Scale * Canvas.XMap.y * (r32)Canvas.Size.Width / (r32)PaintingRegion.Height;
        r32 M01 = Canvas.Scale * Canvas.YMap.x * (r32)Canvas.Size.Height / (r32)PaintingRegion.Width;
        r32 M11 = Canvas.Scale * Canvas.YMap.y * (r32)Canvas.Size.Height / (r32)PaintingRegion.Height;
        
        r32 MatrixOffsetX = (2 * roundf(Canvas.Center.x)) / PaintingRegion.Width;
        r32 MatrixOffsetY = (2 * roundf(Canvas.Center.y)) / PaintingRegion.Height;
        
        Transform = {
            M00, M01, 0, MatrixOffsetX,
            M10, M11, 0, MatrixOffsetY,
            0, 0, 1, 0, 
            0, 0, 0, 1,};
        
        glBindBuffer(GL_ARRAY_BUFFER, OpenGL->ScreenFillVertexBuffer);
        
        // TODO(Zyonji): Why do I generate those here and in render canvas?
        glGenerateMipmap(GL_TEXTURE_2D);
        
        RenderCanvas(OpenGL, Canvas, Transform, PenState);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->ScreenFillVertexBuffer);
    
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
    glViewport(0, 0, OpenGL->DisplayFramebuffer.Size.Width, OpenGL->DisplayFramebuffer.Size.Height);
    glScissor(0, 0, OpenGL->DisplayFramebuffer.Size.Width, OpenGL->DisplayFramebuffer.Size.Height);
    Transform = {
        1, 0, 0, 0, 
        0, 1, 0, 0, 
        0, 0, 1, 0, 
        0, 0, 0, 1,};
    v2s Cursor = {(s32)PenState.Point.x + (s32)OpenGL->DisplayFramebuffer.Size.Width / 2, (s32)PenState.Point.y + (s32)OpenGL->DisplayFramebuffer.Size.Height / 2};
    r32 HalfWidth = 0.5f * PenState.Width;
    v2 Normal = {sinf(PenState.PointRadians), cosf(PenState.PointRadians)};
    
    OpenGLProgramBegin(&OpenGL->DisplayProgram, Transform, Cursor, Canvas.Scale * HalfWidth, Normal);
    glBindTexture(GL_TEXTURE_2D, OpenGL->DisplayFramebuffer.ColorHandle);
    glGenerateMipmap(GL_TEXTURE_2D);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    OpenGLProgramEnd(&OpenGL->DisplayProgram);
    
    u32area Area = Menu.Time;
    v4 Color;
    r32 Width;
    if(TimerProgression > 4)
    {
        DrawTimer(Area, {0, 0, 0, 1});
        Width = 0;
        Color = {0, 0, 0, 1};
    }
    else if(TimerProgression > 3)
    {
        DrawTimer(Area, {0, 1, 0, 1});
        Width = TimerProgression - 3;
        Color = {0, 0, 0, 1};
    }
    else if(TimerProgression > 2)
    {
        DrawTimer(Area, {1, 1, 0, 1});
        Width = TimerProgression - 2;
        Color = {0, 1, 0, 1};
    }
    else if(TimerProgression > 1)
    {
        DrawTimer(Area, {1, 0, 0, 1});
        Width = TimerProgression - 1;
        Color = {1, 1, 0, 1};
    }
    else
    {
        DrawTimer(Area, {1, 1, 1, 1});
        Width = TimerProgression;
        Color = {1, 0, 0, 1};
    }
    if(Area.Width > Area.Height)
    {
        Area.Width = (u32)((r32)Area.Width * Width);
    }
    else
    {
        Area.Height = (u32)((r32)Area.Height * Width);
    }
    DrawTimer(Area, Color);
    // TODO(Zyonji): Should I save the device context or should that be part of display buffer?
    SwapBuffers(wglGetCurrentDC());
}

internal void
DisplayStreamFrame(open_gl *OpenGL, pen_state PenState, pen_target *PenHistory, u32 PenHistoryOffset,  u32 PenHistoryWindow, canvas_state Canvas)
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
        v2s Cursor = {
            (s32)((PenState.P.x / Canvas.Size.Width  + 0.5f) * Width), 
            (s32)((PenState.P.y / Canvas.Size.Height + 0.5f) * 1080)
        };
        r32 HalfWidth = 0.5f * PenState.Width;
        v2 Normal = {sinf(PenState.Radians), cosf(PenState.Radians)};
        
        OpenGLProgramBegin(&OpenGL->DisplayProgram, Transform, Cursor, 1080.0f / (r32)Canvas.Size.Height * HalfWidth, Normal);
        glBindTexture(GL_TEXTURE_2D, OpenGL->CanvasFramebuffer.ColorHandle);
        glGenerateMipmap(GL_TEXTURE_2D);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        OpenGLProgramEnd(&OpenGL->DisplayProgram);
        
        Width = 1920 - Width;
        
        r32 AverageX = 0;
        r32 AverageY = 0;
        r32 WeightTotal = 0;
        
        u32 WindowRadius = Maximum(PenHistoryOffset, PenHistoryWindow - PenHistoryOffset);
        r32 RealRadius = (r32)WindowRadius;
        for(u32 Offset = 0; Offset <= PenHistoryWindow; Offset++)
        {
            pen_target *Pen = PenHistory + Offset;
            r32 NormOffset = fabsf(((r32)Offset - (r32)PenHistoryOffset) / RealRadius);
            r32 Weight = 3*NormOffset*NormOffset - 2*NormOffset*NormOffset*NormOffset;
            AverageX += (1 - Weight) * Pen->P.x;
            AverageY += (1 - Weight) * Pen->P.y;
            WeightTotal += (1 - Weight);
        }
        
        AverageX = AverageX / WeightTotal;
        AverageY = AverageY / WeightTotal;
        
#if 0
        // NOTE(Zyonji): This includes the current pen position in the close view.
        r32 MinX = PenState.P.x - PenState.Width;
        r32 MaxX = PenState.P.x + PenState.Width;
        r32 MinY = PenState.P.y - PenState.Width;
        r32 MaxY = PenState.P.y + PenState.Width;
#else
        r32 MinX = AverageX - PenState.Width;
        r32 MaxX = AverageX + PenState.Width;
        r32 MinY = AverageY - PenState.Width;
        r32 MaxY = AverageY + PenState.Width;
#endif
        
        for(u32 Offset = 0; Offset <= PenHistoryWindow; Offset++)
        {
            pen_target *Pen = PenHistory + Offset;
            r32 NormOffset = fabsf(((r32)Offset - (r32)PenHistoryOffset) / RealRadius);
            r32 Weight = 3*NormOffset*NormOffset - 2*NormOffset*NormOffset*NormOffset;
            
            r32 Px = Weight * AverageX + (1 - Weight) * Pen->P.x;
            r32 Py = Weight * AverageY + (1 - Weight) * Pen->P.y;
            r32 LocalWidth = (1 - Weight) * Pen->Width;
            MinX = Minimum(MinX, Px - LocalWidth);
            MaxX = Maximum(MaxX, Px + LocalWidth);
            MinY = Minimum(MinY, Py - LocalWidth);
            MaxY = Maximum(MaxY, Py + LocalWidth);
        }
        
        MinX -= 100;
        MaxX += 100;
        MinY -= 100;
        MaxY += 100;
        
        r32 MarginX = Width - (MaxX - MinX);
        if(MarginX > 0)
        {
            MinX -= MarginX / 2;
            MaxX += MarginX / 2;
        }
        r32 MarginY = 1080.0f - (MaxY - MinY);
        if(MarginY > 0)
        {
            MinY -= MarginY / 2;
            MaxY += MarginY / 2;
        }
        
        r32 LimitX = 0.5f * Canvas.Size.Width;
        r32 LimitY = 0.5f * Canvas.Size.Height;
        if( MinX < -LimitX)
        {
            MinX = -LimitX;
        }
        if( MaxX >  LimitX)
        {
            MaxX =  LimitX;
        }
        if( MinY < -LimitY)
        {
            MinY = -LimitY;
        }
        if( MaxY >  LimitY)
        {
            MaxY =  LimitY;
        }
        
        v2 Scale = {1080.0f * (r32)Canvas.Size.Width / (r32)Width, (r32)Canvas.Size.Height};
        r32 NewZoom = 1.0f / Maximum((MaxX - MinX) * 1080.0f / (r32)Width, MaxY - MinY);
        static r32 Zoom = NewZoom;
        if(Zoom  > NewZoom)
        {
            Zoom = NewZoom;
        }
        else if(Zoom * 1.25f < NewZoom)
        {
            Zoom = NewZoom * 0.8f;
        }
        v2 Center = {(MinX + MaxX), (MinY + MaxY)};
        Scale *= Zoom;
        if(Scale.x <= 1)
        {
            Center.x = 0;
        }
        else if(Zoom * (Canvas.Size.Width + Center.x) * 1080.0f / (r32)Width <= 1)
        {
            Center.x = 1 / (Zoom * 1080.0f / (r32)Width) - Canvas.Size.Width;
        }
        else if(Zoom * (Canvas.Size.Width - Center.x) * 1080.0f / (r32)Width <= 1)
        {
            Center.x = -1 / (Zoom * 1080.0f / (r32)Width) + Canvas.Size.Width;
        }
        if(Scale.y <= 1)
        {
            Center.y = 0;
        }
        else if(Zoom * (Canvas.Size.Height + Center.y) <= 1)
        {
            Center.y = 1 / Zoom - Canvas.Size.Height;
        }
        else if(Zoom * (Canvas.Size.Height - Center.y) <= 1)
        {
            Center.y = -1 / Zoom + Canvas.Size.Height;
        }
        
        glViewport(1920 - Width, 0, Width, 1080);
        glScissor(1920 - Width, 0, Width, 1080);
        Transform = {
            Scale.x, 0, 0, -Center.x * Zoom * 1080.0f / (r32)Width,
            0, Scale.y, 0, -Center.y * Zoom,
            0, 0, 1, 0, 
            0, 0, 0, 1,};
        
        // TODO(Zyonji): Make the Reference code more structured.
        glBindTexture(GL_TEXTURE_2D, OpenGL->CanvasFramebuffer.ColorHandle);
        
        glGenerateMipmap(GL_TEXTURE_2D);
        OpenGLProgramBegin(&OpenGL->ConvertToRGBProgram, Transform);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        OpenGLProgramEnd(&OpenGL->ConvertToRGBProgram);
        
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    SwapBuffers(wglGetCurrentDC());
}
