#version 120

///////////////////////////////////////////////////////////////////////////////
// Math functions.
///////////////////////////////////////////////////////////////////////////////

vec3 VectorProject(vec3 V1, vec3 V2)
{
    return V1 * dot(V1, V2);
}

vec3 VectorOrthogonalize(vec3 V1, vec3 V2)
{
    return V2 - VectorProject(V1, V2);
}

// Pow(x,y) = x^y, which works even with negative base (x) numbers

#define POW return pow(abs(x), y) * sign(x);

float Pow(float x, float y) { POW }
vec2 Pow(vec2 x, vec2 y) { POW }
vec3 Pow(vec3 x, vec3 y) { POW }
vec4 Pow(vec4 x, vec4 y) { POW }

///////////////////////////////////////////////////////////////////////////////
// The brushed-metal normal computation.
///////////////////////////////////////////////////////////////////////////////

vec3 NoisyNormal(sampler3D NoiseMap, vec3 Coord)
{
    return (texture3D(NoiseMap, Coord).xyz       * 2.0 - 1.0) +
           (texture3D(NoiseMap, Coord * 0.5).xyz * 2.0 - 1.0);
}

vec3 BluredNormal(vec3 Position, vec3 Tangent, sampler3D NoiseMap)
{
    vec3 Final = vec3(0.0);
    int Precision = 8;

    for (int i = -Precision; i <= Precision; i++)
        Final += NoisyNormal(NoiseMap, Position + Tangent * (float(i) / float(Precision)));

    return (Final / float(Precision*2+1));
}

vec3 BrushedMetalNormal(vec3 Normal, vec3 Position, vec3 Direction, float Roughness, float Distance, float Strength, sampler3D NoiseMap)
{
    vec3 Tangent = normalize(VectorOrthogonalize(Normal, Direction));
    vec3 Blured = BluredNormal(Position * Roughness, Tangent * Distance, NoiseMap);

    return normalize(Normal + Blured * Strength);
}

///////////////////////////////////////////////////////////////////////////////
// The Schlick-Fresnel specular lighting model
// Good for: Skin rendering
///////////////////////////////////////////////////////////////////////////////

/*float SchlickFresnel(float ViewDotHalf, float Reflectance)
{
    float Exponential = pow(1.0 - ViewDotHalf, 5.0);
    return Exponential + (1.0 - Exponential) * Reflectance;
}*/

///////////////////////////////////////////////////////////////////////////////
// The Cook-Torrance specular lighting model with Lambert's diffuse lighting.
// Good for: Pretty much anything :D
///////////////////////////////////////////////////////////////////////////////

vec3 CookTorrance(vec3 Normal, vec3 LightDir, vec3 ViewDir,
                  float Roughness, float Reflectance,
                  vec3 Diffuse, vec3 Specular)
{
    vec3 Half            = normalize(LightDir + ViewDir);
    float NormalDotHalf  = dot(Normal, Half);
    float ViewDotHalf    = dot(Half,   ViewDir);
    float NormalDotView  = dot(Normal, ViewDir);
    float NormalDotLight = dot(Normal, LightDir);

    // Compute the geometric term
    float G1 = (2.0 * NormalDotHalf * NormalDotView) / ViewDotHalf;
    float G2 = (2.0 * NormalDotHalf * NormalDotLight) / ViewDotHalf;
    float G  = min(1.0, max(0.0, min(G1, G2)));

    // Compute the fresnel term
    float F  = Reflectance + (1.0 - Reflectance) * pow(1.0 - NormalDotView, 5.0);

    // Compute the roughness term
    float R_2     = Roughness * Roughness;
    float NDotH_2 = NormalDotHalf * NormalDotHalf;
    float A       = 1.0 / (4.0 * R_2 * NDotH_2 * NDotH_2);
    float B       = exp(-(1.0 - NDotH_2) / (R_2 * NDotH_2));
    float R       = A * B;

    // Compute the final term
    float D = max(0.0, NormalDotLight);
    float S = max(0.0, (G * F * R) / (NormalDotLight * NormalDotView));
    return D * (Diffuse + Specular * S);
}

///////////////////////////////////////////////////////////////////////////////
// The computation of the tangent-space basis for flat shading
///////////////////////////////////////////////////////////////////////////////

mat3 ComputeFlatTangentSpaceBasis(vec3 Position, vec2 Texcoord)
{
    vec3 dxPosition = dFdx(Position);
    vec3 dyPosition = dFdy(Position);
    vec2 dxTexcoord = dFdx(Texcoord);
    vec2 dyTexcoord = dFdy(Texcoord);

    vec3 Tangent   = normalize(dxPosition * dyTexcoord.y + dyPosition * dxTexcoord.y);
    vec3 Bitangent = normalize(dxPosition * dyTexcoord.x + dyPosition * dxTexcoord.x);
    vec3 Normal    = normalize(cross(dxPosition, dyPosition));

    return mat3(Tangent, Bitangent, Normal);
}

///////////////////////////////////////////////////////////////////////////////
// Vertex shader:
///////////////////////////////////////////////////////////////////////////////
#if defined(VertexShader)

// Light parameters
uniform vec3 LightPosition;

// Planar mapping
uniform vec4 PlaneS, PlaneT;

varying vec3 Position;
varying vec3 Normal;
varying vec2 Texcoord;
varying vec3 Tangent;
varying vec3 Bitangent;
varying vec3 ViewDirection;
varying vec3 LightDirection;

void main()
{
    gl_Position = ftransform();
    Position = gl_Vertex.xyz;
    Normal = gl_Normal;
#if defined(Mirrored)
    Normal = -Normal;
#endif

#if defined(PlanarMapping)
    Texcoord.x = dot(vec4(gl_Vertex.xyz, 1.0), PlaneS);
    Texcoord.y = dot(vec4(gl_Vertex.xyz, 1.0), PlaneT);
#else
    Texcoord  = gl_MultiTexCoord0.xy;
#endif
    Tangent   = gl_MultiTexCoord1.xyz;
    Bitangent = gl_MultiTexCoord2.xyz;

    ViewDirection  = gl_ModelViewMatrixInverse[3].xyz - Position;
#if 0
    LightDirection = (gl_ModelViewMatrixInverse * vec4(LightPosition, 1)).xyz - Position;
#else
    LightDirection = LightPosition - Position;
#endif
}

#endif
///////////////////////////////////////////////////////////////////////////////
// Fragment shader:
///////////////////////////////////////////////////////////////////////////////
#if defined(FragmentShader)

// Material parameters for lighting
uniform vec3 DiffuseColor;
uniform vec3 SpecularColor;
uniform float SpecularRoughness;
uniform float SpecularReflectance;

// Material parameters for brushing
uniform vec3 BrushDirection;
uniform float BrushDistance;
uniform float BrushRoughness;
uniform float BrushStrength;

// Custom per-pixel distance-based model clipping
uniform vec3 ClipOrigin;
uniform float ClipDistance;

// Exponential scale of texture coordinates
uniform vec2 ExpScale;

// DiffuseMap contrast adjustment
uniform vec2 DiffuseContrastScaleBias;

// Light parameters
uniform vec3 AmbientColor;
uniform vec3 LightColor;

// Textures
uniform sampler2D DiffuseMap;
uniform sampler2D NormalMap;
uniform sampler3D NoiseMap;

// Data from vertex shader
varying vec3 Position;
varying vec3 Normal;
varying vec2 Texcoord;
varying vec3 Tangent;
varying vec3 Bitangent;
varying vec3 ViewDirection;
varying vec3 LightDirection;

vec3 mainFS()
{
#if defined(DistanceBasedCutting)
    if (distance(Position, ClipOrigin) > ClipDistance)
        discard;
#endif

    vec2 Texcoord = Texcoord;
#if defined(ExponentialMapping)
    Texcoord = fract(Texcoord);
    Texcoord -= 0.5;
    Texcoord = Pow(Texcoord, ExpScale);
    Texcoord += 0.5;
#endif

    // Copy and correct the input
    vec3 Ambient = AmbientColor;
    vec3 Diffuse  = DiffuseColor;
    vec3 Specular = SpecularColor;

    vec3 Normal   = normalize(Normal);
    vec3 Tangent  = normalize(Tangent);
    vec3 ViewDir  = normalize(ViewDirection);
    vec3 LightDir = normalize(LightDirection);

    //Ambient = vec3(0.0);
    //Diffuse = vec3(0.0);
    //Specular = vec3(0.0);

#if defined(DiffuseMap_Present)
    vec3 DiffuseTexColor = texture2D(DiffuseMap, Texcoord).xyz;

#if defined(DiffuseMap_AdjustContrast)
    DiffuseTexColor = (DiffuseTexColor - DiffuseContrastScaleBias.y) * DiffuseContrastScaleBias.x + DiffuseContrastScaleBias.y;
#endif    

    // Apply the diffuse map
    Diffuse *= DiffuseTexColor;
#endif

#if defined(NormalMap_Present)
    mat3 TBN = mat3(Tangent, Bitangent, Normal);

    Normal = normalize(texture2D(NormalMap, Texcoord).xyz * 2.0 - 1.0);
    ViewDir = normalize(ViewDir * TBN);
    LightDir = normalize(LightDir * TBN);
#endif

#if defined(Brush)
    // Apply the brushing term
    Normal = BrushedMetalNormal(Normal, Position, BrushDirection, BrushRoughness, BrushDistance, BrushStrength, NoiseMap);
#endif

    // Compute lighting
    vec3 Lighting = CookTorrance(Normal, LightDir, ViewDir, SpecularRoughness, SpecularReflectance, Diffuse, Specular);

    // Combine colors
    vec3 Final = Ambient * Diffuse + LightColor * Lighting;

    return Final;
}

void main()
{
    gl_FragColor = vec4(mainFS(), 0);
}

#endif

