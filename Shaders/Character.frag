#version 430 core
in vec3 ourColor;
in vec2 TexCoord;

out vec4 color;

// Texture sampler
uniform sampler2D texture;

void main()
{
    color = texture(texture, TexCoord); // * vec4(ourColor, 1.0);
}
