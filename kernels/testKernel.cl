#define SPHERE 1
#define SQUARE 2
#define TRIANGLE 3

#define EPSILON 0.001f

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

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

struct Sphere{
	float radius;
	float3 pos;
	float3 emi;
	float3 color;
};

struct Square{
	float3 pos;
	float3 u_vec;
	float3 v_vec;
	float3 normal;

	float3 emi;
	float3 color;
};

struct Intersection {
	float t;
	float3 hitpoint;
	float3 normal;
	float2 uv;
	int hitShapeIndex;
};

struct Shape{
    int type;           //  form type(SPHERE=1, PLANE=2, etc.)
    union {
        struct Sphere sphere;
        struct Square square;
        // GPUTriangle tri;   // later
    } data;
};

float3 get_shape_color(const struct Shape* shape)
{
	if (shape->type == SPHERE) {
		return shape->data.sphere.color;
	} else if (shape->type == SQUARE) {
		return shape->data.square.color;
	}
	return (float3)(0.0f, 0.0f, 0.0f); /* default color */
}

struct Intersection intersect_sphere(const struct Sphere* sphere, const struct Ray* ray, float* t)
{
	float3 rayToCenter = sphere->pos - ray->origin;

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
	result.normal = normalize(result.hitpoint - sphere->pos);
	
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

struct Intersection intersect_square(const struct Square* square, const struct Ray* ray, float* t)
{
    /* calculate intersection of ray with plane of square */

    struct Intersection result;
    result.t = -1.0f; /* default to no intersection */

    float denom = dot(square->normal, ray->dir);
    if (fabs(denom) < EPSILON) return result; /* ray is parallel to square plane */

    /* backface culling: if ray and normal point in same direction, reject */
    if (denom > 0.0f) return result; /* hitting square from behind */

    float3 diff = square->pos - ray->origin;
    *t = dot(diff, square->normal) / denom;

    if (*t < EPSILON) return result; /* square is behind ray or too close */

    /* calculate hitpoint */
    float3 hitpoint = ray->origin + ray->dir * (*t);
    result.t = *t;
    result.hitpoint = hitpoint;
    result.normal = square->normal;

    /* check if hitpoint is inside square bounds */
    float3 local_pos = hitpoint - square->pos;

    // Get the actual lengths of the vectors
    float u_length = length(square->u_vec);
    float v_length = length(square->v_vec);
    
    // Normalize the vectors for projection
    float3 u_normalized = square->u_vec / u_length;
    float3 v_normalized = square->v_vec / v_length;

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


struct Intersection intersect_shape(const struct Shape* shape, const struct Ray* ray, float* t)
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

struct Intersection compute_intersection(const struct Shape* shapes, int numShapes, const struct Ray* ray)
{
	float t = 1e20;
	int hitShapeIndex = -1;
	struct Intersection finalIntersection;
	finalIntersection.t = -1.0f; /* default to no intersection */

	for (int i = 0; i < numShapes; i++){
		struct Shape currentShape = shapes[i];
		float t_temp = 1e20;
		struct Intersection intersection;
		intersection = intersect_shape(&currentShape, ray, &t_temp);

		if (intersection.t > EPSILON && intersection.t < t){
			t = intersection.t;
			hitShapeIndex = i;
			finalIntersection = intersection;
			finalIntersection.hitShapeIndex = hitShapeIndex;
		}
	}

	return finalIntersection;
}

bool compute_shadow(const struct Shape* shapes, int numShapes, const struct Ray* shadowRay, float maxDistance)
{
	for (int i = 0; i < numShapes; i++){
		struct Shape currentShape = shapes[i];
		float t_temp = 1e20;
		struct Intersection intersection;
		intersection = intersect_shape(&currentShape, shadowRay, &t_temp);

		if (intersection.t > EPSILON && intersection.t < maxDistance){
			return true; /* in shadow */
		}
	}

	return false; /* not in shadow */
}

// Iterative version to avoid recursion issues with Rusticl driver
// Uses hemisphere sampling for diffuse materials
float3 raytrace_iterative(const struct Ray* initialRay, const struct Shape* shapes, int numShapes, const struct Light* lights, int numLights, int maxBounces, uint* seed)
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

struct Ray createCamRay(const int x_coord, const int y_coord, const int width, const int height){

	float fx = (float)x_coord / (float)width;  /* convert int in range [0 - width] to float in range [0-1] */
	float fy = (float)y_coord / (float)height; /* convert int in range [0 - height] to float in range [0-1] */

	/* calculate aspect ratio */
	float aspect_ratio = (float)(width) / (float)(height);
	float fx2 = (fx - 0.5f) * aspect_ratio;
	float fy2 = fy - 0.5f;

	/* determine position of pixel on screen */
	float3 pixel_pos = (float3)(fx2, -fy2, 0.0f);

	/* create camera ray*/
	struct Ray ray;
	ray.origin = (float3)(0.0f, 0.0f, 40.0f); /* fixed camera position */
	ray.dir = normalize(pixel_pos - ray.origin); /* ray direction is vector from camera to pixel */

	return ray;
}
// __global output -> [R,G,B,R,G,B,...]
// __global accumBuffer -> accumulates samples over frames [R,G,B,R,G,B,...]
// frameCount -> number of frames accumulated so far (resets when camera/scene changes)
__kernel void render_kernel(__global float* output, __global float* accumBuffer, int width, int height, int frameCount, int numShapes)
{
	const int work_item_id = get_global_id(0);		/* id of current pixel that we are working with */
	int x_coord = work_item_id % width;					/* x-coordinate of the pixel */
	int y_coord = work_item_id / width;					/* y-coordinate of the pixel */

    if (work_item_id >= width * height) return;
    
	float fx = (float)x_coord / (float)width;  /* convert int in range [0 - width] to float in range [0-1] */
	float fy = (float)y_coord / (float)height; /* convert int in range [0 - height] to float in range [0-1] */

	/*create a camera ray */
	struct Ray camray = createCamRay(x_coord, y_coord, width, height);

	/* Cornell box uwu :3 */
	struct Shape sphereShape1;
	sphereShape1.type = SPHERE;
	struct Sphere sphere1;
	sphere1.radius = 0.15f;
	sphere1.pos = (float3)(0.25f, -0.2f, -1.25f);
	sphere1.color = (float3)(0.9f, 0.9f, 0.9f);
	sphereShape1.data.sphere = sphere1;

	struct Shape sphereShape2;
	sphereShape2.type = SPHERE;
	struct Sphere sphere2;
	sphere2.radius = 0.1f;
	sphere2.pos = (float3)(-0.25f, -0.25f, -2.25f);
	sphere2.color = (float3)(0.95f, 0.95f, 0.95f);
	sphereShape2.data.sphere = sphere2;

	struct Shape squareShape1;
	squareShape1.type = SQUARE;
	struct Square square1;
	square1.pos = (float3)(0.0f, -0.5f, 0.0f);
	square1.u_vec = (float3)(1.5f, 0.0f, 0.0f);
	square1.v_vec = (float3)(0.0f, 0.0f, -80.0f);
	square1.normal = (float3)(0.0f, 1.0f, 0.0f);
	square1.color = (float3)(0.9f, 0.9f, 0.9f);
	squareShape1.data.square = square1;

	struct Shape squareShapeTop;
	squareShapeTop.type = SQUARE;
	struct Square squareTop;
	squareTop.pos = (float3)(0.0f, 0.35f, 3.0f);
	squareTop.u_vec = (float3)(1.5f, 0.0f, 0.0f);
	squareTop.v_vec = (float3)(0.0f, 0.0f, -80.0f);
	squareTop.normal = (float3)(0.0f, -1.0f, 0.0f);
	squareTop.color = (float3)(0.9f, 0.9f, 0.9f);
	squareShapeTop.data.square = squareTop;

	struct Shape squareShapeLeft;
	squareShapeLeft.type = SQUARE;
	struct Square squareLeft;
	squareLeft.pos = (float3)(-0.75f, 0.0f, 0.0f);
	squareLeft.u_vec = (float3)(0.0f, 1.5f, 0.0f);
	squareLeft.v_vec = (float3)(0.0f, 0.0f, -80.0f);
	squareLeft.normal = (float3)(1.0f, 0.0f, 0.0f);
	squareLeft.color = (float3)(0.9f, 0.1f, 0.1f);
	squareShapeLeft.data.square = squareLeft;

	struct Shape squareShapeRight;
	squareShapeRight.type = SQUARE;
	struct Square squareRight;
	squareRight.pos = (float3)(0.75f, 0.0f, 0.0f);
	squareRight.u_vec = (float3)(0.0f, 1.5f, 0.0f);
	squareRight.v_vec =	 (float3)(0.0f, 0.0f, -80.0f);
	squareRight.normal = (float3)(-1.0f, 0.0f, 0.0f);
	squareRight.color = (float3)(0.1f, 0.9f, 0.1f);
	squareShapeRight.data.square = squareRight;

	struct Shape squareShapeBack;
	squareShapeBack.type = SQUARE;
	struct Square squareBack;
	squareBack.pos = (float3)(0.0f, 0.0f, -60.0f);
	squareBack.u_vec = (float3)(5.0f, 0.0f, 0.0f);
	squareBack.v_vec = (float3)(0.0f, 1.5f, 0.0f);
	squareBack.normal = (float3)(0.0f, 0.0f, 1.0f);
	squareBack.color = (float3)(0.9f, 0.9f, 0.9f);
	squareShapeBack.data.square = squareBack;

	struct Shape shapes[7];
	shapes[0] = sphereShape1;
	shapes[1] = squareShape1;
	shapes[2] = sphereShape2;
	shapes[3] = squareShapeTop;
	shapes[4] = squareShapeLeft;
	shapes[5] = squareShapeRight;
	shapes[6] = squareShapeBack;

	// struct Shape shapes[2];
	// shapes[0] = sphereShape1;
	// shapes[1] = sphereShape2;

	struct Light lights[1];
	lights[0].pos = (float3)(0.0f, 0.2f, 2.5f);
	lights[0].color = (float3)(1.0f, 1.0f, 1.0f);
	lights[0].intensity = 1.0f;

	int numLights = sizeof(lights) / sizeof(lights[0]);
	//int numShapes = sizeof(shapes) / sizeof(shapes[0]);
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