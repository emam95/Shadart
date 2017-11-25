#version 330 core

uniform float uTime;
uniform vec2 uResolution;
//uniform float uMouse;

void main()
{
	vec2 uv = gl_FragCoord.xy / uResolution.xy;
	gl_FragColor = vec4(uv, 0.5+0.5*sin(uTime), 1.0);
}