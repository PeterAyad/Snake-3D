#version 330 core

out vec4 frag_color;

//TODO: Define uniforms for the center and the side-length
uniform float side_length;
uniform vec2 center;

uniform vec4 inside_color = vec4(1.0, 0.0, 0.0, 1.0);
uniform vec4 outside_color = vec4(0.0, 0.0, 0.0, 1.0);

void main() {
    //TODO: Write code that will draw the square
    float inf_n_dist = max(abs(gl_FragCoord.x - center.x), abs(gl_FragCoord.y - center.y));
    if(inf_n_dist <= side_length / 2.0) {
        frag_color = inside_color;
    } else {
        frag_color = outside_color;
    }
}
