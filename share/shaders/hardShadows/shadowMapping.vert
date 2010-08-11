#version 120
varying vec3 normal;
varying vec4 ambientGlobal;

//0 -> disabled, 1 -> only lighting, 2 -> lighting & shadow
uniform int lightFlag[MAX_NUMBER_OF_LIGHTS];

varying vec4 shadowTexCoord[MAX_NUMBER_OF_LIGHTS];
varying vec3 lightDir[MAX_NUMBER_OF_LIGHTS];
varying float dist[MAX_NUMBER_OF_LIGHTS];

uniform mat4x4 shadowMatrix[MAX_NUMBER_OF_LIGHTS];

void main()
{
	vec4 ecPos;
	vec3 aux;

	// first transform the normal into eye space and normalize the result
	normal = normalize(gl_NormalMatrix * gl_Normal);

	// now normalize the light's direction. Note that according to the
	//OpenGL specification, the light is stored in eye space.
	ecPos = gl_ModelViewMatrix * gl_Vertex;

	ambientGlobal = gl_LightModel.ambient * gl_FrontMaterial.ambient;

	for (int i=0 ; i<MAX_NUMBER_OF_LIGHTS ;i++)
	{
		if (lightFlag[i] > 0)
		{
			aux = vec3(gl_LightSource[i].position-ecPos);
			lightDir[i] = (aux);

			// compute the distance to the light source to a varying variable
			dist[i] = length(aux);

			// Normalize the halfVector to pass it to the fragment shader
			//halfVector[i] = normalize(gl_LightSource[i].halfVector.xyz);

			// Compute the diffuse, ambient and globalAmbient terms
			//diffuse[i] = gl_FrontMaterial.diffuse * gl_LightSource[i].diffuse;
			//ambientGlobal += gl_FrontMaterial.ambient * gl_LightSource[i].ambient;

			if (lightFlag[i] == 2)
				//shadowTexCoord[i] = shadowMatrix[i] * gl_ModelViewMatrix * gl_Vertex;
				shadowTexCoord[i] = gl_TextureMatrix[i] * gl_ModelViewMatrix * gl_Vertex;

		}

	}

#ifdef USE_TEXTURE
	gl_TexCoord[0] = gl_MultiTexCoord0;
#endif

	//////
	gl_Position = ftransform();
	gl_FrontColor = gl_FrontMaterial.diffuse;
	gl_BackColor = gl_BackMaterial.diffuse;
}
