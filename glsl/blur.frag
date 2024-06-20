uniform sampler2D texture;
uniform vec2 resolution;

void main ()
{
    vec2 uv = gl_TexCoord[0].xy;
    vec4 result = texture2D(texture, uv);
    vec2 pixel_size = 1.0 / resolution;
    vec4 color = vec4(0.0);

    int count = 0;

    for (int i = -5; i <= 5; i++)
    {
        for (int j = -5; j <= 5; j++)
        {
            vec2 pos = uv + (vec2(i, j) * pixel_size);
            color = color + texture2D(texture, pos);
            count++;
        }
    }

    color = color / float(count);
    result = color;

    gl_FragColor = result;
}