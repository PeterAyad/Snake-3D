#version 330 core

out vec4 frag_color;

// //TODO: Define uniforms for the center and the radius
uniform float radius;
uniform vec2 center;

uniform vec4 inside_color = vec4(1.0, 0.0, 0.0, 1.0);
uniform vec4 outside_color = vec4(0.0, 0.0, 0.0, 1.0);

void main(){
    //TODO: Write code that will draw the circle
    float distance = sqrt(pow(gl_FragCoord.x - center.x, 2.0) + pow(gl_FragCoord.y - center.y, 2.0));
    if(distance <= radius){
        frag_color = inside_color;
    } else {
        frag_color = outside_color;
    }
}