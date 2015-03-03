#version 400 core
layout (location =0) in vec3 baseVert;
layout (location =1) in vec3 baseNormal;

// transform matrix values
uniform mat4 MVP;
uniform mat4 MV;
out vec3 position;
out vec3 normal;

void main()
{
	// then normalize and mult by normal matrix for shading
        normal = normalize(baseNormal);
        // calculate the position
        position = vec3(MV * vec4(baseVert,1.0));
	// Convert position to clip coordinates and pass along
        gl_Position = MVP*vec4(baseVert,1.0);
}









