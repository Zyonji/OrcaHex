// NOTE(Zyonji): Standard

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

// NOTE(Zyonji): In 0 to 1 range

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

// NOTE(Zyonji): Fast

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
