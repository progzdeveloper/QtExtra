#version 330
in highp vec4 vColor;
out highp vec4 fColor;

uniform sampler2D texture;
uniform vec2 iResolution;
uniform vec2 offset;
uniform vec2 halfpixel;

void main()
{
    vec2 uv = vec2(gl_FragCoord.xy / iResolution);

    vec4 sum = texture2D(texture, uv + vec2(-halfpixel.x * 2.0, 0.0) * offset);
    sum += texture2D(texture, uv + vec2(-halfpixel.x, halfpixel.y) * offset) * 2.0;
    sum += texture2D(texture, uv + vec2(0.0, halfpixel.y * 2.0) * offset);
    sum += texture2D(texture, uv + vec2(halfpixel.x, halfpixel.y) * offset) * 2.0;
    sum += texture2D(texture, uv + vec2(halfpixel.x * 2.0, 0.0) * offset);
    sum += texture2D(texture, uv + vec2(halfpixel.x, -halfpixel.y) * offset) * 2.0;
    sum += texture2D(texture, uv + vec2(0.0, -halfpixel.y * 2.0) * offset);
    sum += texture2D(texture, uv + vec2(-halfpixel.x, -halfpixel.y) * offset) * 2.0;

    fColor = sum / 12.0;
}
