#version 330 core
uniform sampler2D Texture0;

in vec2 vTexCoord;
in float dCo;
out vec4 Outcolor;

void main() {
  vec4 texColor0 = texture(Texture0, vTexCoord);

      Outcolor = texColor0 * dCo * 0.5;
  
	//Outcolor = vec4(vTexCoord.s, vTexCoord.t, 0, 1);
}

