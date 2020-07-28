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
struct area_fill_program
{
    render_program_base Common;
    
    GLuint ImageID;
    GLuint MaskID;
    
    GLuint RadiusID;
    GLuint CenterID;
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
    b32 TESTReferenceOverlay;
    HGLRC RenderingContext;
    
    render_program_base VisualTransparency;
    render_program_base TransferPixelsProgram;
    render_program_base StaticTransferPixelsProgram;
    render_program_base BasicDrawProgram;
    render_program_base LabDrawProgram;
    render_program_base LchDrawProgram;
    brush_mode_program  BrushModeProgram;
    area_fill_program   AreaFillProgram;
    display_program     DisplayProgram;
    
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

#if 0
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
    
    float L = (1.160 * y) - 0.160;
    float a = 5.000 * (x - y);
    float b = 2.000 * (y - z);
    
    vec3 Lab = vec3(L, a, b);
    
    return(Lab);
}
vec3 ConvertLabToRGB(vec3 Lab)
{
    float y = (Lab.x + 0.160) / 1.160;
    float x = Lab.y / 5.000 + y;
    float z = y - Lab.z / 2.000;
    
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

vec3 ConvertRGBToLab(vec3 RGB)
{
    mat3 ToXYZ = mat3(20416.0 / 41085.0, 319.0 / 1245.0, 957.0 / 41085.0, 2533.0 / 7470.0, 2533.0 / 3735.0, 2533.0 / 22410.0, 245.0 / 1494.0, 49.0 / 747.0, 3871.0 / 4482.0);
    
    vec3 Linear = vec3(pow(RGB.x, 2.2), pow(RGB.y, 2.2), pow(RGB.z, 2.2));
    vec3 XYZ = ToXYZ * Linear;
    vec3 Cubic = vec3(pow(XYZ.x, 1.0 / 2.4), pow(XYZ.y, 1.0 / 2.4), pow(XYZ.z, 1.0 / 2.4));
    vec3 Lab = vec3(Cubic.y, Cubic.x - Cubic.y, Cubic.y - Cubic.z);
    
    return(Lab);
}
vec3 ConvertLabToRGB(vec3 Lab)
{
    mat3 ToRGB = mat3(78.0 / 29.0, -2589.0 / 2533.0, 3.0 / 49.0, -37.0 / 29.0, 5011.0 / 2533.0, -11.0 / 49.0, -12.0 / 29.0, 111.0 / 2533.0, 57.0 / 49.0);
    
    vec3 Cubic = vec3(Lab.y + Lab.x, Lab.x, Lab.x - Lab.z);
    vec3 XYZ = vec3(pow(Cubic.x, 2.4), pow(Cubic.y, 2.4), pow(Cubic.z, 2.4));
    vec3 Linear = ToRGB * XYZ;
    vec3 RGB = vec3(pow(Linear.x, 1.0 / 2.2), pow(Linear.y, 1.0 / 2.2), pow(Linear.z, 1.0 / 2.2));
    
#if 0
    // NOTE(Zyonji): For the fast algorithm, it should be enough to just clamp values.
    vec3 Grey = ToRGB * vec3(pow(Cubic.y, 2.4));
    vec3 Vector = Grey - Linear;
    vec3 ScaleA = vec3(Vector.x / Grey.x, Vector.y / Grey.y, Vector.z / Grey.z);
    vec3 ScaleB = vec3(-Vector.x / (1 - Grey.x), -Vector.y / (1 - Grey.y), -Vector.z / (1 - Grey.z));
    float Distance = max(max(max(ScaleA.x, ScaleB.x), max(ScaleA.y, ScaleB.y)), max(ScaleA.z, ScaleB.z));
    if(Distance > 1.0)
    {
        Linear = Grey - (Vector / Distance);
    }
    
    vec3 RGB = vec3(pow(Linear.x, 1.0 / 2.2), pow(Linear.y, 1.0 / 2.2), pow(Linear.z, 1.0 / 2.2));
#endif
    
    return(RGB);
}

#endif

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
    
    vec3 Lab = vec3((1.160 * y) - 0.160, 5.000 * (x - y), 2.000 * (y - z));
    
    return(Lab);
}
vec3 ConvertLabToRGB(vec3 Lab)
{
    float y = (Lab.x + 0.160) / 1.160;
    float x = Lab.y / 5.000 + y;
    float z = y - Lab.z / 2.000;
    
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

global char *GlobalFastLabShaderHeaderCode = R"glsl(
#version 330
vec3 ConvertRGBToLab(vec3 RGB)
{
    mat3 ToXYZ = mat3(20416.0 / 41085.0, 319.0 / 1245.0, 957.0 / 41085.0, 2533.0 / 7470.0, 2533.0 / 3735.0, 2533.0 / 22410.0, 245.0 / 1494.0, 49.0 / 747.0, 3871.0 / 4482.0);
    
    vec3 Linear = vec3(pow(RGB.x, 2.2), pow(RGB.y, 2.2), pow(RGB.z, 2.2));
    vec3 XYZ = ToXYZ * Linear;
    vec3 Cubic = vec3(pow(XYZ.x, 1.0 / 2.4), pow(XYZ.y, 1.0 / 2.4), pow(XYZ.z, 1.0 / 2.4));
    vec3 Lab = vec3(Cubic.y, Cubic.x - Cubic.y, Cubic.y - Cubic.z);
    
    return(Lab);
}
vec3 ConvertLabToRGB(vec3 Lab)
{
    mat3 ToRGB = mat3(78.0 / 29.0, -2589.0 / 2533.0, 3.0 / 49.0, -37.0 / 29.0, 5011.0 / 2533.0, -11.0 / 49.0, -12.0 / 29.0, 111.0 / 2533.0, 57.0 / 49.0);
    
    vec3 Cubic = vec3(Lab.y + Lab.x, Lab.x, Lab.x - Lab.z);
    vec3 XYZ = vec3(pow(Cubic.x, 2.4), pow(Cubic.y, 2.4), pow(Cubic.z, 2.4));
    vec3 Linear = ToRGB * XYZ;
    vec3 RGB = vec3(pow(Linear.x, 1.0 / 2.2), pow(Linear.y, 1.0 / 2.2), pow(Linear.z, 1.0 / 2.2));
    
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
gl_Position = VertP;
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
if(mod(floor(gl_FragCoord.x / 4) + floor(gl_FragCoord.y / 4), 2) > 0.5)
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

    float Cmax = max(max(RGB.x, RGB.y), RGB.z);
    float Cmin = min(min(RGB.x, RGB.y), RGB.z);
    if(Cmax > 1.0 || Cmin < 0.0)
    {
         RGB = vec3(0.9);
    }
    
     FragmentColor = vec4(RGB, 1);
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
vec3 Lab = vec3(Color.x, Color.y * cos(Color.z), Color.y * sin(Color.z));
vec3 RGB = ConvertLabToRGB(Lab);

    float Cmax = max(max(RGB.x, RGB.y), RGB.z);
    float Cmin = min(min(RGB.x, RGB.y), RGB.z);
    if(Cmax > 1.0 || Cmin < 0.0)
    {
         RGB = vec3(0.9);
    }
    
     FragmentColor = vec4(RGB, 1);
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
uniform sampler2D Image;

smooth in vec2 FragUV;
smooth in vec4 Color;

out vec4 FragmentColor;

void main(void)
{
float Alpha = Color.w * clamp((1.0 - abs(FragUV.x)) * FragUV.y, 0.0, 1.0);

vec3 Lab0 = ConvertRGBToLab(Color.xyz);
     vec3 Lab1 = ConvertRGBToLab(texelFetch(Image, ivec2(gl_FragCoord.xy), 0).xyz);
     
     vec3 BlendMode = vec3(Alpha * BrushMode.x, Alpha * BrushMode.y, Alpha * BrushMode.y);
     vec3 Lab = mix(Lab1, Lab0, BlendMode);
     
    vec3 RGB = ConvertLabToRGB(Lab);
    
     FragmentColor = vec4(RGB, 1);
     }
)glsl";
    
    GLuint Program = OpenGLCreateProgram(GlobalLabShaderHeaderCode, VertexCode, FragmentCode, &Result->Common);
    
    Result->BrushModeID = glGetUniformLocation(Program, "BrushMode");
    
    return(Program);
}
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

vec3 Lab0 = ConvertRGBToLab(texture(Image, FragUV).xyz);
     vec3 Lab1 = ConvertRGBToLab(Color.xyz);
     
     vec3 BlendMode = vec3(Alpha * BrushMode.x, Alpha * BrushMode.y, Alpha * BrushMode.y);
     vec3 Lab = mix(Lab0, Lab1, BlendMode);
     
    vec3 RGB = ConvertLabToRGB(Lab);
    
     FragmentColor = vec4(RGB, 1);
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
    
    GLuint Program = OpenGLCreateProgram(GlobalBasicShaderHeaderCode, VertexCode, FragmentCode, &Result->Common);
    
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
OpenGLProgramBegin(area_fill_program *Program, m4x4 Transform, v4 BrushMode, v2 Center, v2 Radius)
{
    OpenGLProgramBegin(&Program->Common);
    
    glUniform1i(Program->ImageID, 0);
    glUniform1i(Program->MaskID,  1);
    
    glUniformMatrix4fv(Program->Common.TransformID, 1, GL_TRUE, Transform.E[0]);
    glUniform4fv(Program->BrushModeID, 1, BrushMode.E);
    glUniform2fv(Program->CenterID, 1, Center.E);
    glUniform2fv(Program->RadiusID, 1, Radius.E);
}
internal void
OpenGLProgramEnd(area_fill_program *Program)
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
    if(!CompileTransferPixels(OpenGL,       &OpenGL->TransferPixelsProgram))
    {
        LogError("Unable to compile the pixel transfer program.", "OpenGL");
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
    if(!CompileAreaFill(OpenGL,             &OpenGL->AreaFillProgram))
    {
        LogError("Unable to compile the area fill renderer.", "OpenGL");
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
    
    glGenFramebuffers(1, &Result.FramebufferHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, Result.FramebufferHandle);
    
    Result.ColorHandle  = FramebufferTextureImage(OpenGL, GL_TEXTURE_2D, Width, Height, GL_RGBA8, Memory);
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
    glClearColor(1, 1, 1, 1);
    glScissor(X, Y + 2 * Third, Half, Third);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0, 0, 0, 1);
    glScissor(X + Half, Y + 2 * Third, Third * 3 - Half, Third);
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
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGL->MenuFramebuffer.FramebufferHandle);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->ScreenFillVertexBuffer);
    glViewport(0, 0, Menu->Size.Width, Menu->Size.Height);
    
    glClearColor(Color.x, Color.y, Color.z, 1);
    glScissor(Menu->ColorB.x, Menu->ColorB.y, Menu->ColorB.Height, Menu->ColorB.Width);
    glClear(GL_COLOR_BUFFER_BIT);
    glScissor(Menu->ColorA.x, Menu->ColorA.y, Menu->ColorA.Height, Menu->ColorA.Width);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR);
    
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
    
    DrawToggle(OpenGL, Menu->TiltToggle, Mode & TILT_MODE_DISABLE);
    DrawToggle(OpenGL, Menu->PenButtonsToggle, Mode & PEN_BUTTON_MODE_FLIPPED);
    DrawToggle(OpenGL, Menu->AreaFillToggle, Mode & PEN_MODE_AREA_FILL);
    DrawToggle(OpenGL, Menu->ResizeToggle, Mode & PEN_MODE_RESIZE);
    
    DrawColorToggle(OpenGL, Menu->ColorButtonA, true, Mode & COLOR_MODE_PRESSURE);
    DrawColorToggle(OpenGL, Menu->ColorButtonB, false, !(Mode & COLOR_MODE_PRESSURE));
    DrawShortToggle(OpenGL, Menu->AlphaButton, !(Mode & COLOR_MODE_ALPHA));
    DrawShortToggle(OpenGL, Menu->LButton, !(Mode & COLOR_MODE_LUMINANCE));
    DrawLongToggle(OpenGL, Menu->abButton, !(Mode & COLOR_MODE_CHROMA));
    DrawToggle(OpenGL, Menu->chButton, (Mode & MENU_MODE_AB));
    
    v3 Lab = ConvertRGBToLab(Color.xyz);
    for(u32 I = 0; I < Menu->Steps; ++I)
    {
        r32 L = (r32)(I + 1) / (r32)(Menu->Steps + 1);
        v3 RGB = ValidatedRGBfromLab({L, Lab.y, Lab.z});
        glClearColor(RGB.x, RGB.y, RGB.z, 1);
        glScissor(Menu->L.x + I * Menu->Offset.x, Menu->L.y + I * Menu->Offset.y, Menu->L.Width, Menu->L.Height);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    glViewport(Menu->LSmooth.x, Menu->LSmooth.y, Menu->LSmooth.Width, Menu->LSmooth.Height);
    glScissor(Menu->LSmooth.x, Menu->LSmooth.y, Menu->LSmooth.Width, Menu->LSmooth.Height);
    v4 L00, L01, L10, L11;
    if(Menu->Size.Width > Menu->Size.Height)
    {
        L00 = {0, Lab.y, Lab.z, 1};
        L01 = {0, Lab.y, Lab.z, 1};
        L10 = {1, Lab.y, Lab.z, 1};
        L11 = {1, Lab.y, Lab.z, 1};
    }
    else
    {
        L00 = {1, Lab.y, Lab.z, 1};
        L01 = {0, Lab.y, Lab.z, 1};
        L10 = {1, Lab.y, Lab.z, 1};
        L11 = {0, Lab.y, Lab.z, 1};
    }
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
    
    
    if(Mode & MENU_MODE_AB)
    {
        v4 a00, a01, a10, a11, b00, b01, b10, b11;
        r32 Limit = GetABLimit(Color.xyz);
        if(Menu->Size.Width > Menu->Size.Height)
        {
            r32 Width = Limit * Menu->a.Height / Menu->a.Width;
            a00 = {Lab.x, -Limit, Lab.z + Width, 1};
            a01 = {Lab.x, -Limit, Lab.z - Width, 1};
            a10 = {Lab.x,  Limit, Lab.z + Width, 1};
            a11 = {Lab.x,  Limit, Lab.z - Width, 1};
            b00 = {Lab.x, Lab.y + Width, -Limit, 1};
            b01 = {Lab.x, Lab.y - Width, -Limit, 1};
            b10 = {Lab.x, Lab.y + Width,  Limit, 1};
            b11 = {Lab.x, Lab.y - Width,  Limit, 1};
        }
        else
        {
            r32 Width = Limit * Menu->a.Width / Menu->a.Height;
            a00 = {Lab.x,  Limit, Lab.z + Width, 1};
            a01 = {Lab.x, -Limit, Lab.z + Width, 1};
            a10 = {Lab.x,  Limit, Lab.z - Width, 1};
            a11 = {Lab.x, -Limit, Lab.z - Width, 1};
            b00 = {Lab.x, Lab.y + Width,  Limit, 1};
            b01 = {Lab.x, Lab.y + Width, -Limit, 1};
            b10 = {Lab.x, Lab.y - Width,  Limit, 1};
            b11 = {Lab.x, Lab.y - Width, -Limit, 1};
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
        v3 Lch = {Lab.x, Length(Lab.yz), (r32)atan2(Lab.z, Lab.y)};
        if(Lch.z < 0)
        {
            Lch.z += Pi32;
        }
        r32 Limit = GetABLimit(Color.xyz);
        if(Menu->Size.Width > Menu->Size.Height)
        {
            r32 Width = Limit * Menu->a.Height / Menu->a.Width;
            a00 = {Lch.x, -Limit, Lch.z + Width, 1};
            a01 = {Lch.x, -Limit, Lch.z - Width, 1};
            a10 = {Lch.x,  Limit, Lch.z + Width, 1};
            a11 = {Lch.x,  Limit, Lch.z - Width, 1};
            b00 = {Lch.x, Lch.y + Width, 2 * Pi32, 1};
            b01 = {Lch.x, Lch.y - Width, 2 * Pi32, 1};
            b10 = {Lch.x, Lch.y + Width, 0, 1};
            b11 = {Lch.x, Lch.y - Width, 0, 1};
        }
        else
        {
            r32 Width = Limit * Menu->a.Width / Menu->a.Height;
            a00 = {Lch.x,  Limit, Lch.z + Width, 1};
            a01 = {Lch.x, -Limit, Lch.z + Width, 1};
            a10 = {Lch.x,  Limit, Lch.z - Width, 1};
            a11 = {Lch.x, -Limit, Lch.z - Width, 1};
            b00 = {Lch.x, Lch.y + Width, 0, 1};
            b01 = {Lch.x, Lch.y + Width, 2 * Pi32, 1};
            b10 = {Lch.x, Lch.y - Width, 0, 1};
            b11 = {Lch.x, Lch.y - Width, 2 * Pi32, 1};
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
    
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGL->SwapFramebuffer.FramebufferHandle);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGL->CanvasFramebuffer.FramebufferHandle);
    glViewport(0, 0, Canvas.Size.Width, Canvas.Size.Height);
    glScissor(0, 0, Canvas.Size.Width, Canvas.Size.Height);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    
    OpenGLProgramBegin(&OpenGL->BrushModeProgram, Transform, BrushMode);
    glBindTexture(GL_TEXTURE_2D, OpenGL->CanvasFramebuffer.ColorHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    OpenGLProgramEnd(&OpenGL->BrushModeProgram);
    
#if 0
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGL->CanvasFramebuffer.FramebufferHandle);
    OpenGLProgramBegin(&OpenGL->StaticTransferPixelsProgram, Transform);
    glBindTexture(GL_TEXTURE_2D, OpenGL->SwapFramebuffer.ColorHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    glBindTexture(GL_TEXTURE_2D, 0);
    OpenGLProgramEnd(&OpenGL->StaticTransferPixelsProgram);
#endif
}
internal void
RenderAreaFlip(open_gl *OpenGL, canvas_state Canvas, pen_target OldPen, pen_target NewPen, v2 Origin, v2 Spread, v4 Color, u32 Step)
{
    common_vertex Vertices[] =
    {
        {{  Origin.x,   Origin.y, 0, 1}, {}, Color},
        {{OldPen.P.x, OldPen.P.y, 0, 1}, {}, Color},
        {{NewPen.P.x, NewPen.P.y, 0, 1}, {}, Color},
    };
    
    m4x4 Transform = {
        (2.0f / (r32)Canvas.Size.Width), 0, 0, 0, 
        0, (2.0f / (r32)Canvas.Size.Height), 0, 0, 
        0, 0, 1, 0, 
        0, 0, 0, 1,};
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGL->SwapFramebuffer.FramebufferHandle);
    glViewport(0, 0, Canvas.Size.Width, Canvas.Size.Height);
    glScissor(0, 0, Canvas.Size.Width, Canvas.Size.Height);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->VertexBuffer);
    if(Step == 0)
    {
        glClearColor(Color.r, Color.g, Color.b, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ZERO);
    
    OpenGLProgramBegin(&OpenGL->BasicDrawProgram, Transform);
    glBindTexture(GL_TEXTURE_2D, OpenGL->CanvasFramebuffer.ColorHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sizeof(Vertices)/sizeof(*Vertices)));
    OpenGLProgramEnd(&OpenGL->BrushModeProgram);
    
    glDisable(GL_BLEND);
    
    if(Step == 2)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGL->CanvasFramebuffer.FramebufferHandle);
        glBindBuffer(GL_ARRAY_BUFFER, OpenGL->ScreenFillVertexBuffer);
        
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, OpenGL->SwapFramebuffer.ColorHandle);
        glGenerateMipmap(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0 + 0);
        
        v4 BrushMode = {};
        if(NewPen.Mode & COLOR_MODE_LUMINANCE)
        {
            BrushMode.x = 1;
        }
        if(NewPen.Mode & COLOR_MODE_CHROMA)
        {
            BrushMode.y = 1;
        }
        if(NewPen.Mode & COLOR_MODE_ALPHA)
        {
            BrushMode.z = 1;
        }
        
        v2 Center = {0.5f + Origin.x / (r32)Canvas.Size.Width, 0.5f + Origin.y / (r32)Canvas.Size.Height};
        Transform = {
            1, 0, 0, 0, 
            0, 1, 0, 0, 
            0, 0, 1, 0, 
            0, 0, 0, 1,};
        
        OpenGLProgramBegin(&OpenGL->AreaFillProgram, Transform, BrushMode, Center, Spread);
        glGenerateMipmap(GL_TEXTURE_2D);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        OpenGLProgramEnd(&OpenGL->AreaFillProgram);
        
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGL->SwapFramebuffer.FramebufferHandle);
        glClearColor(0, 0, 0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);
    }
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
    
    if(PenState.Mode & PEN_MODE_AREA_FILL)
    {
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, OpenGL->SwapFramebuffer.ColorHandle);
        glGenerateMipmap(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0 + 0);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        v4 BrushMode = {};
        if(PenState.Mode & COLOR_MODE_LUMINANCE)
        {
            BrushMode.x = 1;
        }
        if(PenState.Mode & COLOR_MODE_CHROMA)
        {
            BrushMode.y = 1;
        }
        if(PenState.Mode & COLOR_MODE_ALPHA)
        {
            BrushMode.z = 1;
        }
        
        v2 Center = {0.5f + PenState.Origin.x / (r32)Canvas.Size.Width, 0.5f + PenState.Origin.y / (r32)Canvas.Size.Height};
        
        OpenGLProgramBegin(&OpenGL->AreaFillProgram, Transform, BrushMode, Center, PenState.Spread);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        OpenGLProgramEnd(&OpenGL->AreaFillProgram);
    }
    else
    {
        glGenerateMipmap(GL_TEXTURE_2D);
        OpenGLProgramBegin(&OpenGL->TransferPixelsProgram, Transform);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        OpenGLProgramEnd(&OpenGL->TransferPixelsProgram);
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

internal void
DisplayBuffer(open_gl *OpenGL, display_state DisplayState, pen_state PenState, canvas_state Canvas, menu_state Menu, u32 Mintues)
{
    u32area CanvasRegion = {};
    CanvasRegion = {0, 0, DisplayState.Size.Width, DisplayState.Size.Height};
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGL->DisplayFramebuffer.FramebufferHandle);
    glViewport(CanvasRegion.x, CanvasRegion.y, CanvasRegion.Width, CanvasRegion.Height);
    glScissor(CanvasRegion.x, CanvasRegion.y, CanvasRegion.Width, CanvasRegion.Height);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->ScreenFillVertexBuffer);
    glClearColor(0.8, 0.8, 0.8, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    m4x4 Transform;
    
    // NOTE(Zyonji): Rendering the canvas.
    if(Canvas.Size.Height && Canvas.Size.Width)
    {
        r32 M00 = Canvas.Scale * Canvas.XMap.x * (r32)Canvas.Size.Width / (r32)CanvasRegion.Width;
        r32 M10 = Canvas.Scale * Canvas.XMap.y * (r32)Canvas.Size.Width / (r32)CanvasRegion.Height;
        r32 M01 = Canvas.Scale * Canvas.YMap.x * (r32)Canvas.Size.Height / (r32)CanvasRegion.Width;
        r32 M11 = Canvas.Scale * Canvas.YMap.y * (r32)Canvas.Size.Height / (r32)CanvasRegion.Height;
        
        r32 MatrixOffsetX = (2 * roundf(Canvas.Center.x)) / CanvasRegion.Width;
        r32 MatrixOffsetY = (2 * roundf(Canvas.Center.y)) / CanvasRegion.Height;
        
        if(OpenGL->ReferenceFramebuffer.FramebufferHandle)
        {
            v2 MenuOffset = {};
            if(Menu.Size.Width > Menu.Size.Height)
            {
                MenuOffset = {0, 2 * (r32)Menu.Size.Height};
            }
            else
            {
                MenuOffset = {2 * (r32)Menu.Size.Width, 0};
            }
            
            v2 ReferenceOffset = {};
            if(MatrixOffsetX > 0 && MatrixOffsetY > 0)
            {
                ReferenceOffset.x = -1 + (Canvas.Scale * (r32)Canvas.Size.Width + MenuOffset.x) / (r32)CanvasRegion.Width;
                ReferenceOffset.y = -1 + (Canvas.Scale * (r32)Canvas.Size.Height + MenuOffset.y) / (r32)CanvasRegion.Height;
            }
            else if(MatrixOffsetX > 0)
            {
                ReferenceOffset.x = -1 + (Canvas.Scale * (r32)Canvas.Size.Width + MenuOffset.x) / (r32)CanvasRegion.Width;
                ReferenceOffset.y =  1 - Canvas.Scale * (r32)Canvas.Size.Height / (r32)CanvasRegion.Height;
            }
            else if(MatrixOffsetY > 0)
            {
                ReferenceOffset.x =  1 - Canvas.Scale * (r32)Canvas.Size.Width / (r32)CanvasRegion.Width;
                ReferenceOffset.y = -1 + (Canvas.Scale * (r32)Canvas.Size.Height + MenuOffset.y) / (r32)CanvasRegion.Height;
            }
            else
            {
                ReferenceOffset.x =  1 - Canvas.Scale * (r32)Canvas.Size.Width / (r32)CanvasRegion.Width;
                ReferenceOffset.y =  1 - Canvas.Scale * (r32)Canvas.Size.Height / (r32)CanvasRegion.Height;
            }
            
            Transform = {
                M00, M01, 0, ReferenceOffset.x,
                M10, M11, 0, ReferenceOffset.y,
                0, 0, 1, 0, 
                0, 0, 0, 1,};
            
            OpenGLProgramBegin(&OpenGL->TransferPixelsProgram, Transform);
            glBindTexture(GL_TEXTURE_2D, OpenGL->ReferenceFramebuffer.ColorHandle);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindTexture(GL_TEXTURE_2D, 0);
            OpenGLProgramEnd(&OpenGL->TransferPixelsProgram);
        }
        
        Transform = {
            M00, M01, 0, MatrixOffsetX,
            M10, M11, 0, MatrixOffsetY,
            0, 0, 1, 0, 
            0, 0, 0, 1,};
        
        glGenerateMipmap(GL_TEXTURE_2D);
        
        RenderCanvas(OpenGL, Canvas, Transform, PenState);
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
        
        r32 MinX = PenState.P.x - PenState.Width;
        r32 MaxX = PenState.P.x + PenState.Width;
        r32 MinY = PenState.P.y - PenState.Width;
        r32 MaxY = PenState.P.y + PenState.Width;
        
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
        r32 Zoom =  1.0f / Maximum((MaxX - MinX) * 1080.0f / (r32)Width, MaxY - MinY);
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
        
        RenderCanvas(OpenGL, Canvas, Transform, PenState);
    }
    
    SwapBuffers(wglGetCurrentDC());
}
