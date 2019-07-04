#version 330 core 
in vec3 fragNor;
in vec3 LightDir;
out vec4 color;
uniform vec3 matDif;
uniform vec3 matAmb;
uniform vec3 matSpec;
uniform float shine;
in vec3 view;

void main()
{
	vec3 normal = normalize(fragNor);
   vec3 light = normalize(LightDir);
   vec3 norview = normalize(view);

	// Map normal in the range [-1, 1] to color in range [0, 1];
	//vec3 Ncolor = 0.5*normal + 0.5;
	//color = vec4(Ncolor, 1.0);
   //color = vec4(matDif*matAmb + matDif*(max(0,dot(normal,light))),1.0);
   
   vec3 h = normalize(light + norview);
   vec3 bf = matSpec*pow(max(0,dot(normal, h)),shine);
   //vec3 bf = vec3(0,0,0);
   color = vec4(matAmb + bf + matDif*(max(0,dot(normal,light))),1.0);
   //color = vec4(LightDir,1.0);
}
