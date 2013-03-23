#version 120

/******************************************************************************

Copyright (C) 2006-2009 Institute for Visualization and Interactive Systems
(VIS), Universität Stuttgart.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice, this
	list of conditions and the following disclaimer in the documentation and/or
	other materials provided with the distribution.

  * Neither the name of the name of VIS, Universität Stuttgart nor the names
	of its contributors may be used to endorse or promote products derived from
	this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

// {2/vpsize[0], 2/vpsize[1], vpsize[0]/2, vpsize[1]/2}
uniform vec4 vpParams;
uniform float psize;

varying	vec3 ellipsoid_Pos;         // pass-through particle position c
varying	vec4 ellipsoid_PEyePos;     // eye point e' in parameter space and its squared 2-norm
varying vec3 ellipsoid_Color;       // color (rgb)

	
// inverse projection matrix
//        x                        y  
//        V                        V
//        | aspect*tan(fov/2)      0           0               0      |
// P^-1 = |        0           tan(fov/2)      0               0      |
//        |        0               0           0              -1      |
//        |        0               0       -(f-n)/(2fn)   (f+n)/(2fn) |
vec2 invP = vec2(gl_ProjectionMatrixInverse[0][0], gl_ProjectionMatrixInverse[1][1]);

void main(void)
{
	// compute eye ray
	vec3 rayDir;

	// window coords -> normalized device coords
	rayDir.xy = gl_FragCoord.xy*vpParams.xy - vec2(1.0, 1.0);

	// clip coordinates -> eye coordinates
	rayDir = vec3(rayDir.xy*invP.xy, -1.0);

	// transform the ray to world coordinates (unnormalized!!)
	rayDir = mat3(gl_ModelViewMatrixInverse)*rayDir;
	
	// transform ray direction into parameter space -> v' = (1/a)*v
	rayDir /= psize;
	
	// compute determinant D=<e',v'>^2-v'^2(e'^2-1)
	float ev = dot(ellipsoid_PEyePos.xyz, rayDir);
	float vv = dot(rayDir, rayDir);
	float D = ev*ev - vv*ellipsoid_PEyePos.w;

	// kill fragment if outside silhouette
	if (D < 0.0) {
		discard;
	}
	
	// compute intersection parameter t = -<e',v'>-sqrt(D)]/<v',v'>
	float t = -(ev + sqrt(D))/vv;
	
	// compute intersection point s' (parameter space) "==" normal in parameter space
	vec3 s = ellipsoid_PEyePos.xyz + t*rayDir;
		
	// transform position to world space c' = O*a*s'+c and write it to output buffer 0
	vec3 worldSpaceS = (s*psize) + ellipsoid_Pos.xyz;		

	// depth correction
	gl_FragDepth = dot(gl_ModelViewProjectionMatrixTranspose[2],
	                   vec4(worldSpaceS, 1.0)) /
	               dot(gl_ModelViewProjectionMatrixTranspose[3],
                       vec4(worldSpaceS, 1.0));

	// depth range
	gl_FragDepth = 0.5*gl_FragDepth + 0.5;
    
	vec3 normal = normalize(gl_NormalMatrix*(s/psize));
    vec3 position = (gl_ModelViewMatrix*vec4(worldSpaceS, 1.0)).rgb;

    vec3 lightVec = vec3(0.0, 0.0, 1.0);
    vec3 reflVec = reflect(lightVec, normal);
    vec3 viewVec = normalize(position);
    float diffuse = max(dot(lightVec, normal), 0.0);
    float specular = pow(max(dot(viewVec, reflVec), 0.0), 16.0);
    gl_FragColor.rgb = ellipsoid_Color*diffuse+specular+0.05*ellipsoid_Color;
}
