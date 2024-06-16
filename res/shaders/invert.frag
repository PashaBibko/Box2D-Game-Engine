uniform sampler2D texture;

void main ()
{
	vec2 pos = gl_TexCoord[0].xy;
	vec4 invert_result = texture2D (texture, pos);

	invert_result.r = pos.x;
	invert_result.g = pos.y;
	invert_result.b = 1.0 - invert_result.b;

	gl_FragColor = invert_result;
}