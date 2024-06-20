uniform sampler2D texture;

void main()
{
    vec2 pos = gl_TexCoord[0].xy;
    vec4 result = texture2D(texture, pos);

    float gray = result.r * 0.2126 + result.g * 0.7152 + result.b * 0.0722;

    gl_FragColor = vec4(gray, gray, gray, 1.0);
}