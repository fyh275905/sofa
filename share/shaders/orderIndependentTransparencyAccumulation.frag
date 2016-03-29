#version 120

varying vec2 TexCoords;
varying vec3 LightPosition;
varying vec3 LightHalfVector;
varying vec3 Normal;

uniform float DepthScale;
uniform bool HasTexture;
uniform sampler2D ColorSampler;

vec4 Shading()
{
    float diffuseIntensity = max(abs(dot(Normal, LightPosition)), 0.0);

    vec3 ambientColor = vec3(gl_FrontMaterial.ambient * gl_LightSource[0].ambient + gl_LightModel.ambient * gl_FrontMaterial.ambient);
    vec3 diffuseColor = vec3(gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse);
    vec3 specularColor = vec3(gl_FrontMaterial.specular * gl_LightSource[0].specular * pow(max(abs(dot(Normal, LightHalfVector)), 0.0), gl_FrontMaterial.shininess));

    float alpha = gl_FrontMaterial.diffuse.a;

    if(HasTexture)
    {
        vec4 colorTexture = texture2D(ColorSampler, TexCoords);

        diffuseColor *= colorTexture.rgb;
        alpha *= colorTexture.a;
    }

    return vec4(ambientColor + diffuseColor * diffuseIntensity + specularColor, alpha);
}

void main()
{
    vec4 finalColor = Shading();
    finalColor.rgb *= finalColor.a; // premultiplied alpha

    float viewDepth = abs(1.0 / gl_FragCoord.w);
    float linearDepth = viewDepth * DepthScale;
    float weight = clamp(0.03 / (1e-5 + pow(linearDepth, 4.0)), 1e-2, 3e3);

    gl_FragData[0] = vec4(finalColor.rgb * weight, finalColor.a);
    gl_FragData[1].r = finalColor.a * weight;
}
