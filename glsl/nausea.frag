uniform sampler2D texture;
uniform float time;

void main()
{
    vec2 pos = gl_TexCoord[0].xy;
    vec4 original_color = texture2D(texture, pos);

    float distortionFactor = 0.1;
    float waveSpeed = 5.0;

    float distortedX = pos.x + distortionFactor * sin(waveSpeed * time + pos.y);
    float distortedY = pos.y + distortionFactor * cos(waveSpeed * time + pos.x);

    vec4 color = texture2D (texture, vec2(distortedX, distortedY));

    gl_FragColor = color;
}