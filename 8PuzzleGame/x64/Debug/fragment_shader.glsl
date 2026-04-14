#version 430

in vec2 localPos;
out vec4 FragColor;

uniform vec4 squareColor;
uniform int renderMode; // 0 = striped, 1 = solid

void main()
{
    vec3 bgColor = vec3(0.2, 0.3, 0.3);

    // -------------------------
    // MODE 1: SOLID COLOR
    // -------------------------
    if (renderMode == 1)
    {
        FragColor = squareColor;
        return;
    }

    // -------------------------
    // MODE 0: DIAGONAL STRIPES
    // -------------------------
    float spacing = 0.08;
    float lineWidth = 0.015;

    vec2 p = localPos + 0.25;

    float d = mod(p.x + p.y, spacing);

    float aa = fwidth(p.x + p.y);

    float line = 1.0 - smoothstep(lineWidth - aa, lineWidth + aa, d);

    vec3 color = mix(squareColor.rgb, bgColor, line);

    FragColor = vec4(color, 1.0);
}