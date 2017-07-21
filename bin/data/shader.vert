#version 120

varying vec3 v_position;
varying vec3 v_normal;

void main()
{
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
    v_position = gl_Vertex.xyz;
    v_normal = gl_Normal;
    
    // // the sine wave travels along the x-axis (across the screen),
    // // so we use the x coordinate of each vertex for the calculation,
    // // but we displace all the vertex along the y axis (up the screen)/
    // float displacementHeight = 100.0;
    // float displacementY = sin(time + (position.x / 100.0)) * displacementHeight;
	
    // vec4 modifiedPosition = position;
	// modifiedPosition.y += displacementY;
	// gl_Position = modifiedPosition;
    
}