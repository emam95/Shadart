#version 330 core

uniform float uTime;
uniform vec2 uResolution;

out vec4 fragCol;

// For guide check raymarchingSDFs.frag

const int MAX_STEPS = 255;
const float NEAR_PLANE = 0.0;
const float FAR_PLANE = 100.0;
const float EPSILON = 0.0001;
const float DIST_BIAS = 0.8;

float sphereSDF(vec3 p, float r)
{
	return length(p) - r;
}

float cubeSDF(vec3 p, vec3 s)
{
	vec3 d = abs(p) - s;
	return min(max(d.x, max(d.y, d.z)), 0.0) + length(max(d, 0.0));
}

// Constructive Solid Geometry, CSG creates complex geometric shapes from simple shapes using boolean
// operations. The 3 primitive operations are intersection, union and difference.

// The point is inside both objects (intersection) if it is negative in both (max is still negative)
float intersectSDF(float distA, float distB)
{
	return max(distA, distB);
}

// The point is inside either object (union) if either of them is negative
float unionSDF(float distA, float distB)
{
	return min(distA, distB);
}

// The point is inside 1 object and outside the other (difference) the 1st is -ve and the 2nd is +ve
float differenceSDF(float distA, float distB)
{
	return max(distA, -distB);
}

// Rotation matrix (around x)
mat3 rotateX(float theta)
{
	float c = cos(theta);
	float s = sin(theta);
	return mat3(
		vec3(1.0, 0.0, 0.0),
		vec3(0.0, c  , -s ),
		vec3(0.0, s  , c  )
	);
}

// Rotation matrix (around y)
mat3 rotateY(float theta)
{
	float c = cos(theta);
	float s = sin(theta);
	return mat3(
		vec3(c  , 0.0, s  ),
		vec3(0.0, 1.0, 0.0),
		vec3(-s , 0.0, c  )
	);
}

// Rotation matrix (around z)
mat3 rotateZ(float theta)
{
	float c = cos(theta);
	float s = sin(theta);
	return mat3(
		vec3(c  , -s , 0.0),
		vec3(s  , c  , 0.0),
		vec3(0.0, 0.0, 1.0)
	);
}

vec2 sceneSDF(vec3 p)
{
	// Note that to apply rigid body transformations we apply inverse transformation to the point that
	// will be evaluated in the SDF
	// For Uniform Scaling we need to multiply the distance by the scale factor since scaling doesn't 
	// preserve the distance while this doesn't affect the sign it does affect the distance which we use
	// in ray marching
	// For non-uniform Scaling we try to underestimate the distance (resulting in more iterations) instead
	// of overestimating (resulting in overshooting) by multiplying by the minimum scale
	// float dist = sphereSDF(p / vec3(sx, sy, sz)) * min(sx, min(sy, sz));

	// Spin whole scene
	p = rotateY(uTime/2.0) * p;

	float matId = 3.0;
	float sdist = sphereSDF(p / 1.2, 1.0) * 1.2;

	float cdist = cubeSDF(p, vec3(1));

	float dist = intersectSDF(sdist, cdist);

	return vec2(dist, matId);
}

vec2 rayMarch(vec3 org, vec3 dir, float start, float end)
{
	float depth = start;

	for(int i = 0; i < MAX_STEPS; i++)
	{
		vec2 result = sceneSDF(org + dir * depth);

		if(result.x < EPSILON) //inside the surface
			return vec2(depth, result.y);

		depth += result.x * DIST_BIAS; //update the step with bias to avoid errors

		if(depth >= end) //reached the end with no hits
			break;
	}

	return vec2(end, 0.0);
}

vec3 approxNormal(vec3 p)
{
	return normalize(vec3(
		sceneSDF(vec3(p.x + EPSILON, p.y, p.z)).x - sceneSDF(vec3(p.x - EPSILON, p.y, p.z)).x,
		sceneSDF(vec3(p.x, p.y + EPSILON, p.z)).x - sceneSDF(vec3(p.x, p.y - EPSILON, p.z)).x,
		sceneSDF(vec3(p.x, p.y, p.z + EPSILON)).x - sceneSDF(vec3(p.x, p.y, p.z - EPSILON)).x
	));
}

vec3 phong(vec3 kd, vec3 ks, float shininess, vec3 p, vec3 cam, vec3 lightPos, vec3 intensity)
{
	vec3 n = approxNormal(p);
	vec3 l = normalize(lightPos - p);
	vec3 v = normalize(cam - p);
	vec3 r = normalize(reflect(-l, n));

	float ln = dot(l, n);
	float rv = dot(r, v);

	return intensity * (kd * max(ln, 0.0) + ks * pow(max(rv, 0.0), shininess));
}

vec3 phongIllumination(vec3 ka, vec3 kd, vec3 ks, float shininess, vec3 p, vec3 cam)
{
	const vec3 ambient = 0.5 * vec3(1.0);
	vec3 col = ambient * ka;

	vec3 posL1 = vec3(4.0, 2.0, 5.0);
	vec3 intL1 = vec3(0.4);

	col += phong(kd, ks, shininess, p, cam, posL1, intL1);

	vec3 posL2 = vec3(0.75, 0.0, 0.0);
	vec3 intL2 = vec3(0.2);

	col += phong(kd, ks, shininess, p, cam, posL2, intL2);

	return col;
}

vec3 rayDir(float fov, vec2 res, vec2 fragCoord)
{
	// converting coords to uv space (fragCoord/res) then moving space between [-1, 1] (uv * 2 - 1)
	vec2 xy = fragCoord - res / 2.0;
	float z = res.y / tan(radians(fov) / 2.0);
	return normalize(vec3(xy, -z));
}

mat4 viewMatrix(vec3 cam, vec3 center, vec3 up)
{
	vec3 f = normalize(center - cam);
	vec3 r = normalize(cross(f, up));
	vec3 u = cross(r, f);

	return mat4(vec4(r, 0.0), vec4(u, 0.0), vec4(-f, 0.0), vec4(vec3(0.0), 1.0));
}

void main()
{
	vec3 vdir = rayDir(45.0, uResolution, gl_FragCoord.xy);
	vec3 cam = vec3(7.0 * 0.5 * cos(uTime), 3.0, 7.0);

	mat4 viewMat = viewMatrix(cam, vec3(0.0), vec3(0.0, 1.0, 0.0));
	vec3 dir = (viewMat * vec4(vdir, 0.0)).xyz;

	vec2 result = rayMarch(cam, dir, NEAR_PLANE, FAR_PLANE);

	vec3 col = vec3(0.0);

	if(result.y == 1.0)
	{
		vec3 p = cam + result.x * dir;

		vec3 ka = vec3(0.2);
		vec3 kd = vec3(0.2, 0.7, 0.2);
		vec3 ks = vec3(1.0);
		float shininess = 10.0;
	
		col = phongIllumination(ka, kd, ks, shininess, p, cam);
	}
	else if(result.y == 2.0)
	{
		vec3 p = cam + result.x * dir;

		vec3 ka = vec3(0.2);
		vec3 kd = vec3(0.7, 0.3, 0.2);
		vec3 ks = vec3(1.0);
		float shininess = 5.0;
	
		col = phongIllumination(ka, kd, ks, shininess, p, cam);
	}
	else if(result.y == 3.0)
	{
		vec3 p = cam + result.x * dir;

		vec3 ka = (approxNormal(p) + vec3(1.0)) / 2.0;
		vec3 kd = ka;
		vec3 ks = vec3(1.0);
		float shininess = 10.0;
	
		col = phongIllumination(ka, kd, ks, shininess, p, cam);
	}

	fragCol = vec4(col, 1.0);
}