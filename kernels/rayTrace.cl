#define SPHERE 1
#define SQUARE 3
#define TRIANGLE 3

#define EPSILON 0.001f

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

// Match CPU-side Vec3 with padding to align to 16 bytes (same as float4)
typedef struct {
    float x, y, z;
    float _padding;
} Vec3;

// GPU Camera structure matching CPU side exactly (64 bytes total)
typedef struct {
    Vec3 origin;      // Camera position (16 bytes)
    Vec3 target;      // What the camera is looking at (16 bytes)  
    Vec3 up;          // Up vector (16 bytes)
    float fov;        // Field of view in degrees (4 bytes)
    float _padding[3]; // Padding for alignment (12 bytes)
} GPUCamera;

// Helper function to convert Vec3 to float3
float3 vec3_to_float3(Vec3 v) {
    return (float3)(v.x, v.y, v.z);
}

struct Ray{
	float3 origin;
	float3 dir;
};

// Simple random number generator (PCG hash)
uint wang_hash(uint seed)
{
	seed = (seed ^ 61) ^ (seed >> 16);
	seed *= 9;
	seed = seed ^ (seed >> 4);
	seed *= 0x27d4eb2d;
	seed = seed ^ (seed >> 15);
	return seed;
}

// Generate random float in [0,1)
float random_float(uint* seed)
{
	*seed = wang_hash(*seed);
	return (float)(*seed) / 4294967296.0f;
}

// Generate random direction in hemisphere oriented around normal
// Uses cosine-weighted importance sampling
float3 random_hemisphere_direction(float3 normal, uint* seed)
{
	// Generate two random numbers
	float r1 = random_float(seed);
	float r2 = random_float(seed);
	
	// Cosine-weighted hemisphere sampling
	float phi = 2.0f * M_PI * r1;
	float cos_theta = sqrt(1.0f - r2);
	float sin_theta = sqrt(r2);
	
	// Create local coordinate system around normal
	float3 tangent;
	if (fabs(normal.x) > 0.1f) {
		tangent = normalize(cross((float3)(0.0f, 1.0f, 0.0f), normal));
	} else {
		tangent = normalize(cross((float3)(1.0f, 0.0f, 0.0f), normal));
	}
	float3 bitangent = cross(normal, tangent);
	
	// Transform from local to world space
	float3 direction = cos(phi) * sin_theta * tangent +
	                   sin(phi) * sin_theta * bitangent +
	                   cos_theta * normal;
	
	return normalize(direction);
}

struct Light {
	float3 pos;
	float3 color;
	float intensity;
};

// Match CPU-side GPUSphere exactly
typedef struct {
    float radius;
    float _padding1[3];  // Padding to align next Vec3
    Vec3 pos;
    Vec3 emi;
    Vec3 color;
} GPUSphere;

// Match CPU-side GPUSquare exactly
typedef struct {
    Vec3 pos;
    Vec3 u_vec;
    Vec3 v_vec;
    Vec3 normal;
    Vec3 emi;
    Vec3 color;
} GPUSquare;

struct Intersection {
	float t;
	float3 hitpoint;
	float3 normal;
	float2 uv;
	int hitShapeIndex;
};

// Match CPU-side GPUShape exactly
typedef struct __attribute__((aligned(16))) {
    int type;           // form type(SPHERE=1, PLANE=2, SQUARE=3, etc.)
    float _padding[3];  // Padding after int to align union to 16 bytes
    union {
        GPUSphere sphere;
        GPUSquare square;
    } data;
} GPUShape;

float3 get_shape_color(__global const GPUShape* shape)
{
	if (shape->type == SPHERE) {
		return vec3_to_float3(shape->data.sphere.color);
	} else if (shape->type == SQUARE) {
		return vec3_to_float3(shape->data.square.color);
	}
	return (float3)(0.0f, 0.0f, 0.0f); /* default color */
}

struct Intersection intersect_sphere(__global const GPUSphere* sphere, const struct Ray* ray, float* t)
{
	float3 sphere_pos = vec3_to_float3(sphere->pos);
	float3 rayToCenter = sphere_pos - ray->origin;

	/* calculate coefficients a, b, c from quadratic equation */
	/* float a = dot(ray->dir, ray->dir); // ray direction is normalised, dotproduct simplifies to 1 */ 
	float b = dot(rayToCenter, ray->dir);
	float c = dot(rayToCenter, rayToCenter) - sphere->radius * sphere->radius;
	float disc = b * b - c; /* discriminant of quadratic formula */

	struct Intersection result;
	result.t = -1.0f; /* default to no intersection */

	if (disc < 0.0f) return result; /* no intersection */

	/* solve for t (distance to hitpoint along ray) */
	float sqrt_disc = sqrt(disc);
	float t1 = b - sqrt_disc; /* near intersection */
	float t2 = b + sqrt_disc; /* far intersection */

	/* choose the closest positive t with epsilon threshold */
	*t = -1.0f;
	if (t1 > EPSILON) {
		*t = t1;
	} else if (t2 > EPSILON) {
		*t = t2; /* ray starts inside sphere, use exit point */
	} else {
		return result; /* both intersections behind ray or too close */
	}

	result.t = *t;
	result.hitpoint = ray->origin + ray->dir * (*t);
	
	/* calculate normal - always point outward from sphere center */
	result.normal = normalize(result.hitpoint - sphere_pos);
	
	/* backface culling: if ray and normal point in same direction, reject */
	if (dot(ray->dir, result.normal) > 0.0f && *t == t1) {
		/* hitting sphere from behind (outside->inside), reject this intersection */
		result.t = -1.0f;
		return result;
	}
	
	/* if we're using the far intersection (t2), we're inside the sphere */
	/* so flip the normal to point inward for proper shading */
	if (*t == t2) {
		result.normal = -result.normal;
	}
	
	result.uv = (float2)(0.0f, 0.0f); // Placeholder for UV coordinates TODO spheric uv mapping
	return result;
}

struct Intersection intersect_square(__global const GPUSquare* square, const struct Ray* ray, float* t)
{
    /* calculate intersection of ray with plane of square */

    struct Intersection result;
    result.t = -1.0f; /* default to no intersection */

    float3 square_normal = vec3_to_float3(square->normal);
    float3 square_pos = vec3_to_float3(square->pos);
    float3 square_u_vec = vec3_to_float3(square->u_vec);
    float3 square_v_vec = vec3_to_float3(square->v_vec);

    float denom = dot(square_normal, ray->dir);
    if (fabs(denom) < EPSILON) return result; /* ray is parallel to square plane */

    /* backface culling: if ray and normal point in same direction, reject */
    if (denom > 0.0f) return result; /* hitting square from behind */

    float3 diff = square_pos - ray->origin;
    *t = dot(diff, square_normal) / denom;

    if (*t < EPSILON) return result; /* square is behind ray or too close */

    /* calculate hitpoint */
    float3 hitpoint = ray->origin + ray->dir * (*t);
    result.t = *t;
    result.hitpoint = hitpoint;
    result.normal = square_normal;

    /* check if hitpoint is inside square bounds */
    float3 local_pos = hitpoint - square_pos;

    // Get the actual lengths of the vectors
    float u_length = length(square_u_vec);
    float v_length = length(square_v_vec);
    
    // Normalize the vectors for projection
    float3 u_normalized = square_u_vec / u_length;
    float3 v_normalized = square_v_vec / v_length;

    float u_dist = dot(local_pos, u_normalized);
    if (u_dist < -u_length * 0.5f || u_dist > u_length * 0.5f) {
        result.t = -1.0f;
        return result;
    }

    float v_dist = dot(local_pos, v_normalized);
    if (v_dist < -v_length * 0.5f || v_dist > v_length * 0.5f) {
        result.t = -1.0f;
        return result;
    }

    result.uv = (float2)((u_dist / u_length) + 0.5f, (v_dist / v_length) + 0.5f); // UV coordinates in [0,1] range

    return result;
}


struct Intersection intersect_shape(__global const GPUShape* shape, const struct Ray* ray, float* t)
{
	if (shape->type == SPHERE) {
		return intersect_sphere(&shape->data.sphere, ray, t);
	} else if (shape->type == SQUARE) {
		return intersect_square(&shape->data.square, ray, t);
	}
	struct Intersection result;
	result.t = -1.0f; /* default to no intersection */
	return result;
}

struct Intersection compute_intersection(__global const GPUShape* shapes, int numShapes, const struct Ray* ray)
{
	float t = 1e20;
	int hitShapeIndex = -1;
	struct Intersection finalIntersection;
	finalIntersection.t = -1.0f; /* default to no intersection */

	for (int i = 0; i < numShapes; i++){
		float t_temp = 1e20;
		struct Intersection intersection;
		intersection = intersect_shape(&shapes[i], ray, &t_temp);

		if (intersection.t > EPSILON && intersection.t < t){
			t = intersection.t;
			hitShapeIndex = i;
			finalIntersection = intersection;
			finalIntersection.hitShapeIndex = hitShapeIndex;
		}
	}

	return finalIntersection;
}

bool compute_shadow(__global const GPUShape* shapes, int numShapes, const struct Ray* shadowRay, float maxDistance)
{
	for (int i = 0; i < numShapes; i++){
		float t_temp = 1e20;
		struct Intersection intersection;
		intersection = intersect_shape(&shapes[i], shadowRay, &t_temp);

		if (intersection.t > EPSILON && intersection.t < maxDistance){
			return true; /* in shadow */
		}
	}

	return false; /* not in shadow */
}

// Iterative version to avoid recursion issues with Rusticl driver
// Uses hemisphere sampling for diffuse materials
float3 raytrace_iterative(const struct Ray* initialRay, __global const GPUShape* shapes, int numShapes, const struct Light* lights, int numLights, int maxBounces, uint* seed)
{
	float3 accumulatedColor = (float3)(0.0f, 0.0f, 0.0f);
	float3 throughput = (float3)(1.0f, 1.0f, 1.0f); // Track how much light can pass through
	struct Ray currentRay = *initialRay;
	
	for (int bounce = 0; bounce < maxBounces; bounce++) {
		struct Intersection intersection = compute_intersection(shapes, numShapes, &currentRay);
		
		if (intersection.t < EPSILON) {
			// No intersection, could add sky color here
			break;
		}

		float3 diffuse = get_shape_color(&shapes[intersection.hitShapeIndex]);
		
		// Direct lighting contribution
		float3 directLight = (float3)(0.0f, 0.0f, 0.0f);
		
		// Ambient term
		directLight += diffuse * 0.25f;

		// Direct lighting from light sources
		for (int i = 0; i < numLights; i++){
			float3 lightDir = normalize(lights[i].pos - intersection.hitpoint);
			float dotLN = dot(lightDir, intersection.normal);

			if (dotLN > EPSILON) {
				struct Ray shadowRay;
				shadowRay.origin = intersection.hitpoint + intersection.normal * EPSILON * 10.0f;
				shadowRay.dir = lightDir;
				float lightDistance = length(lights[i].pos - intersection.hitpoint);
				
				if (!compute_shadow(shapes, numShapes, &shadowRay, lightDistance - EPSILON)) {
					// Not in shadow - add full lighting
					directLight += diffuse * lights[i].color * lights[i].intensity * dotLN;
				} else {
					// In shadow - add reduced lighting
					directLight += diffuse * lights[i].color * lights[i].intensity * dotLN * 0.2f;
				}
			}
		}

		// Add direct lighting modulated by throughput
		accumulatedColor += throughput * directLight;

		// Update throughput for next bounce (material absorption)
		throughput *= diffuse;
		
		// Russian roulette termination for efficiency
		float maxThroughput = fmax(fmax(throughput.x, throughput.y), throughput.z);
		if (maxThroughput < 0.01f) break; // Stop if contribution is too small

		// Prepare next ray with random hemisphere sampling (diffuse BRDF)
		if (bounce < maxBounces - 1) {
			float3 newDir = random_hemisphere_direction(intersection.normal, seed);
			currentRay.origin = intersection.hitpoint + intersection.normal * EPSILON * 10.0f;
			currentRay.dir = newDir;
		}
	}

	return accumulatedColor;
}

// Original function that worked with GPUCamera (for reference)
struct Ray createCamRay(const int x_coord, const int y_coord, const int width, const int height, __global const GPUCamera* camera){

	float fx = (float)x_coord / (float)width;  /* convert int in range [0 - width] to float in range [0-1] */
	float fy = (float)y_coord / (float)height; /* convert int in range [0 - height] to float in range [0-1] */

	/* calculate aspect ratio */
	float aspect_ratio = (float)(width) / (float)(height);
	
	/* Convert FOV from degrees to radians */
	float fov_radians = camera->fov * M_PI / 180.0f;
	float tan_half_fov = tan(fov_radians * 0.5f);
	
	/* Calculate camera coordinate system */
	float3 camera_origin = vec3_to_float3(camera->origin);
	float3 camera_target = vec3_to_float3(camera->target);
	float3 camera_up = vec3_to_float3(camera->up);
	
	float3 forward = normalize(camera_target - camera_origin);
	float3 right = normalize(cross(forward, camera_up));
	float3 up = cross(right, forward);
	
	/* Calculate pixel position in camera space */
	float px = (fx - 0.5f) * 2.0f * tan_half_fov * aspect_ratio;
	float py = -(fy - 0.5f) * 2.0f * tan_half_fov;  // INVERSION DE L'AXE Y
	
	/* Ray direction in world space */
	float3 ray_dir = normalize(forward + px * right + py * up);

	/* create camera ray*/
	struct Ray ray;
	ray.origin = camera_origin;
	ray.dir = ray_dir;

	return ray;
}

// __global output -> [R,G,B,R,G,B,...]
// __global accumBuffer -> accumulates samples over frames [R,G,B,R,G,B,...]
// frameCount -> number of frames accumulated so far (resets when camera/scene changes)
__kernel void render_kernel(__global float* output, __global float* accumBuffer, int width, int height, int frameCount, 
                           __global GPUShape* shapes, int numShapes,
                           __global GPUCamera* camera)
{
	const int work_item_id = get_global_id(0);		/* id of current pixel that we are working with */
	int x_coord = work_item_id % width;					/* x-coordinate of the pixel */
	int y_coord = work_item_id / width;					/* y-coordinate of the pixel */

    if (work_item_id >= width * height) return;
    
	float fx = (float)x_coord / (float)width;  /* convert int in range [0 - width] to float in range [0-1] */
	float fy = (float)y_coord / (float)height; /* convert int in range [0 - height] to float in range [0-1] */

	/*create a camera ray */
	struct Ray camray = createCamRay(x_coord, y_coord, width, height, camera);

	struct Light lights[1];
	lights[0].pos = (float3)(0.0f, 0.2f, 2.5f);
	lights[0].color = (float3)(1.0f, 1.0f, 1.0f);
	lights[0].intensity = 1.0f;

	int numLights = sizeof(lights) / sizeof(lights[0]);

	int maxbounce = 10;
	
	// Initialize random seed based on pixel position AND frame count for temporal variation
	uint seed = (x_coord * 1973 + y_coord * 9277 + frameCount * 26699) | 1;
	
	float3 outputPixelColor = raytrace_iterative(&camray, shapes, numShapes, lights, numLights, maxbounce, &seed);

	/* If no intersection found, return background colour */
	if (outputPixelColor.x == 0.0f && outputPixelColor.y == 0.0f && outputPixelColor.z == 0.0f) {
		outputPixelColor = (float3)(fy * 0.7f, fy * 0.3f, 0.3f);
	}
	
	// index *3 for RGB
	int base_idx = work_item_id * 3;
	
	// Temporal accumulation: blend new sample with accumulated samples
	float3 accumulatedColor;
	if (frameCount == 0) {
		// First frame: just use current sample
		accumulatedColor = outputPixelColor;
	} else {
		// Progressive accumulation using running average
		float3 previousAccum = (float3)(accumBuffer[base_idx], 
		                                 accumBuffer[base_idx + 1], 
		                                 accumBuffer[base_idx + 2]);
		
		// Running average: new_avg = (old_avg * n + new_sample) / (n + 1)
		float t = (float)frameCount / (float)(frameCount + 1);
		accumulatedColor = previousAccum * t + outputPixelColor * (1.0f - t);
	}
	
	// Store accumulated color (linear space)
	accumBuffer[base_idx] = accumulatedColor.x;
	accumBuffer[base_idx + 1] = accumulatedColor.y;
	accumBuffer[base_idx + 2] = accumulatedColor.z;
	
	// Apply post-processing for display
	float3 displayColor = clamp(accumulatedColor, 0.0f, 1.0f);
	// Apply gamma correction (gamma = 2.2)
	//displayColor = pow(displayColor, (float3)(1.0f / 2.2f));
	
	output[base_idx] = displayColor.x;     // R
	output[base_idx + 1] = displayColor.y; // G
	output[base_idx + 2] = displayColor.z; // B
}