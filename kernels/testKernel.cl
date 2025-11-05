#define SPHERE 1
#define SQUARE 2
#define TRIANGLE 3

struct Ray{
	float3 origin;
	float3 dir;
};

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
};

struct Shape{
    int type;           //  form type(SPHERE=1, PLANE=2, etc.)
    union {
        struct Sphere sphere;
        struct Square square;
        // GPUTriangle tri;   // later
    } data;
};

struct Intersection intersect_sphere(const struct Sphere* sphere, const struct Ray* ray, float* t)
{
	float3 rayToCenter = sphere->pos - ray->origin;

	/* calculate coefficients a, b, c from quadratic equation */

	/* float a = dot(ray->dir, ray->dir); // ray direction is normalised, dotproduct simplifies to 1 */ 
	float b = dot(rayToCenter, ray->dir);
	float c = dot(rayToCenter, rayToCenter) - sphere->radius*sphere->radius;
	float disc = b * b - c; /* discriminant of quadratic formula */

	/* solve for t (distance to hitpoint along ray) */

	struct Intersection result;
	result.t = -1.0f; /* default to no intersection */

	if (disc < 0.0f) return result;
	else *t = b - sqrt(disc);

	if (*t < 0.0f){
		*t = b + sqrt(disc);
		if (*t < 0.0f) return result; 
	}

	result.t = *t;
	result.hitpoint = ray->origin + ray->dir * (*t);
	result.normal = normalize(result.hitpoint - sphere->pos);
	result.uv = (float2)(0.0f, 0.0f); // Placeholder for UV coordinates TODO spheric uv mapping
	return result;
}

struct Intersection intersect_square(const struct Square* square, const struct Ray* ray, float* t)
{
    /* calculate intersection of ray with plane of square */

    struct Intersection result;
    result.t = -1.0f; /* default to no intersection */

    float denom = dot(square->normal, ray->dir);
    if (fabs(denom) < 1e-6f) return result; /* ray is parallel to square plane */

    float3 diff = square->pos - ray->origin;
    *t = dot(diff, square->normal) / denom;

    if (*t < 0.0f) return result; /* square is behind ray */

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
__kernel void render_kernel(__global float* output, int width, int height)
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
	square1.normal = (float3)(0.0f, -1.0f, 0.0f);
	square1.color = (float3)(0.9f, 0.9f, 0.9f);
	squareShape1.data.square = square1;

	struct Shape squareShapeTop;
	squareShapeTop.type = SQUARE;
	struct Square squareTop;
	squareTop.pos = (float3)(0.0f, 0.35f, 3.0f);
	squareTop.u_vec = (float3)(1.5f, 0.0f, 0.0f);
	squareTop.v_vec = (float3)(0.0f, 0.0f, -80.0f);
	squareTop.normal = (float3)(0.0f, 1.0f, 0.0f);
	squareTop.color = (float3)(0.9f, 0.9f, 0.9f);
	squareShapeTop.data.square = squareTop;

	struct Shape squareShapeLeft;
	squareShapeLeft.type = SQUARE;
	struct Square squareLeft;
	squareLeft.pos = (float3)(-0.75f, 0.0f, 0.0f);
	squareLeft.u_vec = (float3)(0.0f, 1.5f, 0.0f);
	squareLeft.v_vec = (float3)(0.0f, 0.0f, -80.0f);
	squareLeft.normal = (float3)(-1.0f, 0.0f, 0.0f);
	squareLeft.color = (float3)(0.9f, 0.1f, 0.1f);
	squareShapeLeft.data.square = squareLeft;

	struct Shape squareShapeRight;
	squareShapeRight.type = SQUARE;
	struct Square squareRight;
	squareRight.pos = (float3)(0.75f, 0.0f, 0.0f);
	squareRight.u_vec = (float3)(0.0f, 1.5f, 0.0f);
	squareRight.v_vec =	 (float3)(0.0f, 0.0f, -80.0f);
	squareRight.normal = (float3)(1.0f, 0.0f, 0.0f);
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

	struct Light lights[1];
	lights[0].pos = (float3)(0.0f, 0.4f, -10.0f);
	lights[0].color = (float3)(1.0f, 1.0f, 1.0f);
	lights[0].intensity = 1.0f;

	int numShapes = sizeof(shapes) / sizeof(shapes[0]);

	/* intersect ray with sphere */
	float t = 1e20;
	int hitShapeIndex = -1;
	for (int i = 0; i < numShapes; i++){
		struct Shape currentShape = shapes[i];
		float t_temp = 1e20;
		struct Intersection intersection;
		intersection = intersect_shape(&currentShape, &camray, &t_temp);

		if (intersection.t > 0.0f && intersection.t < t){
			t = intersection.t;
			hitShapeIndex = i;
		}
	}

	/* if ray misses sphere, return background colour 
	background colour is a blue-ish gradient dependent on image height */
	float3 color,normal,outputPixelColor;
	
    if (t > 1e19) { 
        outputPixelColor = (float3)(fy * 0.1f, fy * 0.3f, 0.3f);
    } else {
		struct Shape hitShape = shapes[hitShapeIndex];
		struct Intersection intersection;
		intersection = intersect_shape(&hitShape, &camray, &t);
		normal = intersection.normal;

		if (hitShape.type == SPHERE){
			float cosine_factor = dot(normal, camray.dir) * -1.0f;
			color = hitShape.data.sphere.color * cosine_factor;
		} else if (hitShape.type == SQUARE){
			float cosine_factor = dot(normal, camray.dir) * -1.0f;
			color = hitShape.data.square.color * cosine_factor;
		}
		
		outputPixelColor = color;
	}
    
    // index *3 for RGB
    int base_idx = work_item_id * 3;
	
    output[base_idx] = outputPixelColor.x;     // R
    output[base_idx + 1] = outputPixelColor.y; // G
    output[base_idx + 2] = outputPixelColor.z; // B

}