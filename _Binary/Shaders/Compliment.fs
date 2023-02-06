#version 330

in vec3 Color;

uniform vec3 SolidColor = vec3(-1,-1,-1);
uniform bool CompBool = false;

out vec4 Fragment;

void main()
{
	if (CompBool == true){
		vec3 Compliment = vec3(1,1,1) - Color;
		Fragment = vec4(Compliment,1);
	}
	else {
		Fragment = vec4(Color, 1);
	}
	
	if(SolidColor.r != -1.0 && SolidColor.g != -1.0 && SolidColor.b != -1.0)
		Fragment = vec4(SolidColor, 1);

	return;
}