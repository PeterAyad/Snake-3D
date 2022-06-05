#version 330

uniform sampler2D tex;
in vec2 tex_coord;
out vec4 frag_color;
uniform float time;
uniform float enable;

void main() {
    float x = sin(time * 1000) / 100;
    if(enable == 1.0) {
        frag_color = texture(tex, vec2(x + tex_coord.x, tex_coord.y));
    } else {
        frag_color = texture(tex, tex_coord);
    }
}