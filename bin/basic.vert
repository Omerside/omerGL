#version 330 core

/*
---- Space Definitions ----

These definitions are ordered in the way that they are processed by the program.
We start by calculating Local/Object space, and end by calculating the NDC.

-- Local/Object Space: 
Local coordinates are the coordinates of your object relative to its local origin; 
they're the coordinates your object begins in.

Most objects have their point of origin at (0, 0, 0).


-- World Coordinates:
These coordinates are relative to some global origin of the world, together with 
many other objects also placed relative to this world's origin. The coordinates of 
your object are transformed from local to world space; this is accomplished with the model matrix.

Related uniform: mat4 model;


-- View Coordinates:
Coordinates of an object as it is seen from the camera or viewer's point of view.

-- Clip Coordinates
Clip coordinates are processed to the -1.0 and 1.0 range and determine which vertices 
will end up on the screen. Projection to clip-space coordinates can add perspective if 
using perspective projection.


-- Normalized Device Coordinates (NDC): 
transform the clip coordinates to screen coordinates in a process we call viewport transform 
that transforms the coordinates from -1.0 and 1.0 to the coordinate range defined by glViewport.
The resulting coordinates are then sent to the rasterizer to turn them into fragments.


----- END Space Definitions -----

Calculating new vertex position in 3d space:
Vertex (clip space) = Projection matrix x View Matrix x Model Matrix x Vertex in local space

*/

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in ivec4 boneIds;
layout (location = 4) in vec4 weights;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

const int MAX_NUM_OF_BONES = 100;
uniform mat4 model; //The model matrix is a transformation matrix that translates, scales and/or rotates your object to place it in the world at a location/orientation they belong to.
uniform mat4 view; // The view space is the result of transforming your world-space coordinates to coordinates that are in front of the user's view.
uniform mat4 projection; //transforms coordinates within this specified range to normalized device coordinates (-1.0, 1.0). All coordinates outside this range will not be mapped between -1.0 and 1.0 and therefore be clipped.
uniform mat4 finalTransforms[MAX_NUM_OF_BONES]; //transformation matrices based on new position of model bones.

void main()
{

//This is a hardcoded of version of the transformation matrix generated in the program for turnstick.
mat4 tempBoneTransform = mat4(0.52f, 0.0f, 0.85f, 0.0f,
								-0.08f, -1.0f, 0.049f, 0.0f,
								0.85f, -0.094f, -0.52f, 0.0f,
								0.34f, 4.2f, -0.2f, 1.0f);

	mat4 BoneTransform;
	if (boneIds[0] > 0 && weights[0] > 0){
		
		BoneTransform = finalTransforms[boneIds[0]] * weights[0];
		//BoneTransform += finalTransforms[boneIds[1]] * 0;weights[1];
		//BoneTransform += finalTransforms[boneIds[2]] * 0;weights[2];
		//BoneTransform += finalTransforms[boneIds[3]] * 0;weights[3];
		//BoneTransform = tempBoneTransform * weights[0]; 
		//BoneTransform = mat4(1.0f);
		
	} else {
		BoneTransform = mat4(1.0f);
	}

	vec4 newPos = BoneTransform * (vec4(pos, 1.0f));
	vec4 newNormal = BoneTransform  * (vec4(normal, 1.0f));


	//To get world space, we multiply our view space by our model vector
	vec4 worldPos = vec4(model * newPos);
	
	FragPos = vec3(worldPos);


	//recalculate normals in case of scaling skewing incoming values
	Normal = mat3(transpose(inverse(model))) * newNormal.xyz;

	//Results are assigned to gl_Position in the vertex shader so that OpenGL will automatically perform perspective division and clipping.
	gl_Position = projection * view * worldPos;
	TexCoord = texCoord;
	

};